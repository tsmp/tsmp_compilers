#pragma once

#include "xrPool.h"

class xrMU_Model
{
public:
	struct VertexMu;
	struct FaceMu;

	struct _subdiv
	{
		OGF *ogf;

		u32 material;
		u32 start;
		u32 count;

		u32 vb_id;
		u32 vb_start;

		u32 ib_id;
		u32 ib_start;

		u32 sw_id;
	};

	typedef xr_vector<VertexMu *> v_vertices;
	typedef v_vertices::iterator v_vertices_it;
	typedef xr_vector<FaceMu *> v_faces;
	typedef v_faces::iterator v_faces_it;
	typedef xr_vector<_subdiv> v_subdivs;
	typedef v_subdivs::iterator v_subdivs_it;

	struct VertexMu : public ::BaseVertex
	{
		v_faces adjacent;

	public:
		void prep_add(FaceMu *F);
		void prep_remove(FaceMu *F);
		void calc_normal_adjacent();

		VertexMu() = default;
		~VertexMu() = default;
	};

	struct FaceMu : public ::base_Face
	{
	public:
		VertexMu *v[3];
		Fvector2 tc[3];
		Fvector N;

	public:
		virtual Fvector2 *getTC0() { return tc; };

		bool VContains(VertexMu *pV);			   // Does the face contains this vertex?
		void VReplace(VertexMu *what, VertexMu *to); // Replace ONE vertex by ANOTHER
		void VReplace_NoRemove(VertexMu *what, VertexMu *to);
		int VIndex(VertexMu *pV);
		void VSet(int idx, VertexMu *V);
		void VSet(VertexMu *V1, VertexMu *V2, VertexMu *V3);
		BOOL isDegenerated();
		BOOL isEqual(FaceMu &F);
		void EdgeVerts(int e, VertexMu **A, VertexMu **B);
		void CalcNormal();
		void CalcNormal2();
		float CalcArea();
		float CalcMaxEdge();
		void CalcCenter(Fvector &C);
		BOOL RenderEqualTo(Face *F);

		FaceMu(){};
		virtual ~FaceMu(){};
	};

public:
	shared_str m_name;
	u16 m_lod_ID;
	v_vertices m_vertices;
	v_faces m_faces;
	v_subdivs m_subdivs;

	xr_vector<base_color> color;

private:
	FaceMu *create_face(VertexMu *v0, VertexMu *v1, VertexMu *v2, b_face &F);
	VertexMu *create_vertex(Fvector &P);
	FaceMu *load_create_face(Fvector &P1, Fvector &P2, Fvector &P3, b_face &F);
	VertexMu *load_create_vertex(Fvector &P);

public:
	void Load(IReader &fs);
	void calc_normals();
	void calc_materials();
	void calc_faceopacity();
	void calc_lighting(xr_vector<base_color> &dest, Fmatrix &xform, CDB::MODEL *M,
		base_lighting &lights, u32 flags);
	void calc_lighting();
	void CalcOgf(xr_vector<OGF_Base*> &ogfTree);
	void export_geometry();
	void export_cform_rcast(CDB::CollectorPacked &CL, Fmatrix &xform);
};

class xrMU_Reference
{
public:
	xrMU_Model *model;
	Fmatrix xform;
	Flags32 flags;
	u16 sector;

	xr_vector<base_color> color;

	base_color_c c_scale;
	base_color_c c_bias;

public:
	void Load(IReader &fs);
	void calc_lighting();

	void export_cform_game(CDB::CollectorPacked &CL);
	void export_cform_rcast(CDB::CollectorPacked &CL);
	void ExportOgf(xr_vector<OGF_Base*> &ogfTree);
};

extern poolSS<xrMU_Model::VertexMu, 8 * 1024> mu_vertices;
extern poolSS<xrMU_Model::FaceMu, 8 * 1024> mu_faces;
