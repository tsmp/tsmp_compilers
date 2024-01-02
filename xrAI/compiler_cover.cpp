#include "stdafx.h"
#include "compiler.h"
#include "..\cl_intersect.h"
#include "CommonCompilers\xrThread.h"
#include <mmsystem.h>

#include "quadtree.h"
#include "cover_point.h"
#include "Object\object_broker.h"

Shader_xrLC_LIB *g_shaders_xrlc;
xr_vector<b_material> g_materials;
xr_vector<b_shader> g_shader_render;
xr_vector<b_shader> g_shader_compile;
xr_vector<b_BuildTexture> g_textures;
xr_vector<b_rc_face> g_rc_faces;

// -------------------------------- Ray pick
typedef Fvector RayCache[3];

IC float getLastRP_Scale(CDB::COLLIDER *DB, RayCache &C)
{
	u32 tris_count = DB->r_count();
	float scale = 1.f;
	Fvector B;

	//	X_TRY
	{
		for (u32 I = 0; I < tris_count; I++)
		{
			CDB::RESULT &rpinf = DB->r_begin()[I];
			// Access to texture
			//			CDB::TRI& clT								=
			Level.get_tris()[rpinf.id];
			b_rc_face &F = g_rc_faces[rpinf.id];

			b_material &M = g_materials[F.dwMaterial];
			b_texture &T = g_textures[M.surfidx];
			Shader_xrLCVec &LIB = g_shaders_xrlc->Library();
			if (M.shader_xrlc >= LIB.size())
				return 0; //. hack
			Shader_xrLC &SH = LIB[M.shader_xrlc];
			if (!SH.flags.bLIGHT_CastShadow)
				continue;

			if (0 == T.pSurface)
				T.bHasAlpha = FALSE;
			if (!T.bHasAlpha)
			{
				// Opaque poly - cache it
				C[0].set(rpinf.verts[0]);
				C[1].set(rpinf.verts[1]);
				C[2].set(rpinf.verts[2]);
				return 0;
			}

			// barycentric coords
			// note: W,U,V order
			B.set(1.0f - rpinf.u - rpinf.v, rpinf.u, rpinf.v);

			// calc UV
			Fvector2 *cuv = F.t;
			Fvector2 uv;
			uv.x = cuv[0].x * B.x + cuv[1].x * B.y + cuv[2].x * B.z;
			uv.y = cuv[0].y * B.x + cuv[1].y * B.y + cuv[2].y * B.z;

			int U = iFloor(uv.x * float(T.dwWidth) + .5f);
			int V = iFloor(uv.y * float(T.dwHeight) + .5f);
			U %= T.dwWidth;
			if (U < 0)
				U += T.dwWidth;
			V %= T.dwHeight;
			if (V < 0)
				V += T.dwHeight;

			u32 pixel = T.pSurface[V * T.dwWidth + U];
			u32 pixel_a = color_get_A(pixel);
			float opac = 1.f - float(pixel_a) / 255.f;
			scale *= opac;
		}
	}
	//	X_CATCH
	//	{
	//		clMsg("* ERROR: getLastRP_Scale");
	//	}

	return scale;
}

IC float rayTrace(CDB::COLLIDER *DB, Fvector &P, Fvector &D, float R, RayCache &C)
{
	R_ASSERT(DB);

	// 1. Check cached polygon
	float _u, _v, range;
	bool res = CDB::TestRayTri(P, D, C, _u, _v, range, false);
	if (res)
	{
		if (range > 0 && range < R)
			return 0;
	}

	// 2. Polygon doesn't pick - real database query
	DB->ray_query(&Level, P, D, R);

	// 3. Analyze polygons and cache nearest if possible
	if (!DB->r_count())
		return 1;

	return getLastRP_Scale(DB, C);
}

IC int calcSphereSector(Fvector &dir)
{
	Fvector2 flat;

	// flatten
	flat.set(dir.x, dir.z);
	flat.norm();

	// analyze
	if (_abs(flat.x) > _abs(flat.y))
	{
		// sector 0,7,3,4
		if (flat.x < 0)
		{
			// sector 3,4
			if (flat.y > 0)
				return 3;
			else
				return 4;
		}
		else
		{
			// sector 0,7
			if (flat.y > 0)
				return 0;
			else
				return 7;
		}
	}
	else
	{
		// sector 1,2,6,5
		if (flat.x < 0)
		{
			// sector 2,5
			if (flat.y > 0)
				return 2;
			else
				return 5;
		}
		else
		{
			// sector 1,6
			if (flat.y > 0)
				return 1;
			else
				return 6;
		}
	}
}

// volumetric query
DEF_VECTOR(Nearest, u32);

class Query
{
public:
	Nearest q_List;
	Nearest q_Clear;
	Marks q_Marks;
	Fvector q_Base;

	IC void Begin(int count)
	{
		q_List.reserve(8192);
		q_Clear.reserve(8192);
		q_Marks.assign(count, false);
	}

	IC void Init(Fvector &P)
	{
		q_Base.set(P);
		q_List.clear();
		q_Clear.clear();
	}

	IC void Perform(u32 ID)
	{
		if (ID == InvalidNode)
			return;
		if (ID >= q_Marks.size())
			return;
		if (q_Marks[ID])
			return;

		q_Marks[ID] = true;
		q_Clear.push_back(ID);

		vertex &N = g_nodes[ID];
		if (q_Base.distance_to_sqr(N.Pos) > cover_sqr_dist)
			return;

		// ok
		q_List.push_back(ID);

		Perform(N.n1);
		Perform(N.n2);
		Perform(N.n3);
		Perform(N.n4);
	}

	IC void Clear()
	{
		for (Nearest_it it = q_Clear.begin(); it != q_Clear.end(); it++)
			q_Marks[*it] = false;
	}
};
struct RC
{
	RayCache C;
};

class CoverThread : public CThread
{
	u32 Nstart, Nend;

public:
	CoverThread(u32 ID, u32 _start, u32 _end) : CThread(ID)
	{
		Nstart = _start;
		Nend = _end;
	}
	virtual void Execute()
	{
		CDB::COLLIDER DB;
		DB.ray_options(CDB::OPT_CULL);

		xr_vector<RC> cache;
		{
			RC rc;
			rc.C[0].set(0, 0, 0);
			rc.C[1].set(0, 0, 0);
			rc.C[2].set(0, 0, 0);

			cache.assign(g_nodes.size() * 2, rc);
		}

		FPU::m24r();
		Query Q;
		Q.Begin(g_nodes.size());

		for (u32 N = Nstart; N < Nend; N++)
		{
			// initialize process
			thProgress = float(N - Nstart) / float(Nend - Nstart);
			vertex &BaseNode = g_nodes[N];

			Fvector &BasePos = BaseNode.Pos;
			Fvector TestPos = BasePos;
			TestPos.y += cover_height;

			float c_total[8] = {0, 0, 0, 0, 0, 0, 0, 0};
			float c_passed[8] = {0, 0, 0, 0, 0, 0, 0, 0};

			// perform volumetric query
			Q.Init(BasePos);
			Q.Perform(N);

			// main cycle: trace rays and compute counts
			for (Nearest_it it = Q.q_List.begin(); it != Q.q_List.end(); it++)
			{
				// calc dir & range
				u32 ID = *it;
				R_ASSERT(ID < g_nodes.size());
				if (N == ID)
					continue;
				vertex &N = g_nodes[ID];
				Fvector &Pos = N.Pos;
				Fvector Dir;
				Dir.sub(Pos, BasePos);
				float range = Dir.magnitude();
				Dir.div(range);

				// raytrace
				int sector = calcSphereSector(Dir);
				c_total[sector] += 1.f;
				c_passed[sector] += rayTrace(&DB, TestPos, Dir, range, cache[ID].C); //
			}
			Q.Clear();

			// analyze probabilities
			float value[8];

			for (int dirs = 0; dirs < 8; dirs++)
			{
				R_ASSERT(c_passed[dirs] <= c_total[dirs]);
				if (c_total[dirs] == 0)
					value[dirs] = 0;
				else
					value[dirs] = float(c_passed[dirs]) / float(c_total[dirs]);
				clamp(value[dirs], 0.f, 1.f);
			}

			if (value[0] < .999f)
			{
				value[0] = value[0];
			}

			BaseNode.cover[0] = (value[2] + value[3] + value[4] + value[5]) / 4.f;
			clamp(BaseNode.cover[0], 0.f, 1.f); // left
			BaseNode.cover[1] = (value[0] + value[1] + value[2] + value[3]) / 4.f;
			clamp(BaseNode.cover[1], 0.f, 1.f); // forward
			BaseNode.cover[2] = (value[6] + value[7] + value[0] + value[1]) / 4.f;
			clamp(BaseNode.cover[2], 0.f, 1.f); // right
			BaseNode.cover[3] = (value[4] + value[5] + value[6] + value[7]) / 4.f;
			clamp(BaseNode.cover[3], 0.f, 1.f); // back
		}
	}
};

extern void mem_Optimize();
int i_ThreadCount = 1;

void xrCover(bool pure_covers)
{
	Status("Calculating...");

	// Start threads, wait, continue --- perform all the work
	u32 start_time = timeGetTime();
	CThreadManager Threads;
	u32 stride = g_nodes.size() / i_ThreadCount;
	u32 last = g_nodes.size() - stride * (i_ThreadCount - 1);

	for (u32 thID = 0; thID < i_ThreadCount; thID++)
		Threads.start(xr_new<CoverThread>(thID, thID * stride,
			thID * stride + ((thID == (i_ThreadCount - 1)) ? last : stride)));
	Threads.wait();
	Msg("%d seconds elapsed.", (timeGetTime() - start_time) / 1000);

	// Smooth
	Status("Smoothing coverage mask...");
	mem_Optimize();
	Nodes Old = g_nodes;

	for (u32 N = 0; N < g_nodes.size(); N++)
	{
		vertex &Base = Old[N];
		vertex &Dest = g_nodes[N];

		for (int dir = 0; dir < 4; dir++)
		{
			float val = 2 * Base.cover[dir];
			float cnt = 2;

			for (int nid = 0; nid < 4; nid++)
			{
				if (Base.n[nid] != InvalidNode)
				{
					val += Old[Base.n[nid]].cover[dir];
					cnt += 1.f;
				}
			}
			Dest.cover[dir] = val / cnt;
		}
	}
}
