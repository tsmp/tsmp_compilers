#include "stdafx.h"
#include "build.h"
#include "OGF_Face.h"
#include "vbm.h"
#include "std_classes.h"
#include "lightmap.h"

#define TRY(a)                                                                                     \
	try                                                                                            \
	{                                                                                              \
		a;                                                                                         \
	}                                                                                              \
	catch (...)                                                                                    \
	{                                                                                              \
		clMsg("* E: %s", #a);                                                                      \
	}

void CBuild::ValidateSplits(const xr_vector<vecFace> &splits)
{
	u32 splitIndex = 0;
	const u32 MaxSplitSize = c_SS_HighVertLimit * 2;

	for (const vecFace &split : splits)
	{
		if (split.size() > MaxSplitSize)
			clMsg("! ERROR: subdiv #%d has more than %d faces (%d)", splitIndex, MaxSplitSize, split.size());

		splitIndex++;
	}
}

void CBuild::Flex2OGF(xr_vector<vecFace> &inputSplits, xr_vector<OGF_Base*> &outputOgfTree)
{
	float p_total = 0;
	float p_cost = 1 / float(inputSplits.size());
	float progr_time = 0;

	ValidateSplits(inputSplits);
	outputOgfTree.reserve(4096);

	CTimer start_ogf;
	start_ogf.Start();

	const float SplitsCount = static_cast<float>(inputSplits.size());
	u32 counter = 0;

	for (auto &split: inputSplits)
	{
		R_ASSERT(!split.empty());
		u32 MODEL_ID = counter;

		OGF *pOGF = xr_new<OGF>();
		Face *F = split[0];				 // first face
		b_material *M = &(materials[F->dwMaterial]); // and it's material
		R_ASSERT(F && M);

		try
		{
			// Common data
			pOGF->Sector = M->sector;
			pOGF->material = F->dwMaterial;

			// Collect textures
			OGF_Texture T;

			TRY(T.name = textures[M->surfidx].name);
			TRY(T.pSurface = &(textures[M->surfidx]));
			TRY(pOGF->textures.push_back(T));

			try
			{
				if (F->hasImplicitLighting())
				{
					// specific lmap
					string_path tn;
					strconcat(sizeof(tn), tn, *T.name, "_lm.dds");
					T.name = tn;
					T.pSurface = T.pSurface; // Leave surface intact
					R_ASSERT(pOGF);
					pOGF->textures.push_back(T);
				}
				else
				{
					// If lightmaps persist
					CLightmap *LM = F->lmap_layer;
					if (LM)
					{
						string_path fn;
						sprintf_s(fn, "%s_1", LM->lm_texture.name);
						T.name = fn;
						T.pSurface = &(LM->lm_texture);
						R_ASSERT(T.pSurface);
						R_ASSERT(pOGF);
						pOGF->textures.push_back(T); //.
						sprintf(fn, "%s_2", LM->lm_texture.name);
						T.name = fn;
						pOGF->textures.push_back(T);
					}
				}
			}
			catch (...)
			{
				clMsg("* ERROR: Flex2OGF, model# %d, *textures*", MODEL_ID);
			}

			// Collect faces & vertices
			F->CacheOpacity();
			bool _tc_ = !(F->flags.bOpaque);

			try
			{
				for (Face *face: split)
				{
					OGF_Vertex V[3];
					R_ASSERT(face);

					// Geometry
					for (u32 fv = 0; fv < 3; fv++)
					{
						V[fv].P.set(face->v[fv]->P);
						V[fv].N.set(face->v[fv]->N);
						V[fv].T = face->basis_tangent[fv];
						V[fv].B = face->basis_binormal[fv];
						V[fv].Color = face->v[fv]->C;
					}

					// Normal order
					svector<_TCF, 2>::iterator TC = face->tc.begin();
					for (; TC != face->tc.end(); TC++)
					{
						V[0].UV.push_back(TC->uv[0]);
						V[1].UV.push_back(TC->uv[1]);
						V[2].UV.push_back(TC->uv[2]);
					}

					// build face
					TRY(pOGF->_BuildFace(V[0], V[1], V[2], _tc_));
					V[0].UV.clear();
					V[1].UV.clear();
					V[2].UV.clear();
				}
			}
			catch (...)
			{
				clMsg("* ERROR: Flex2OGF, model# %d, *faces*", MODEL_ID);
			}
		}
		catch (...)
		{
			clMsg("* ERROR: Flex2OGF, 1st part, model# %d", MODEL_ID);
		}

		try
		{
			pOGF->Optimize();
			pOGF->CalcBounds(outputOgfTree);

			CTimer start_pr;
			start_pr.Start();

			if (!b_noise)
				pOGF->MakeProgressive(c_PM_MetricLimit_static);

			progr_time += start_pr.GetElapsed_sec();

			pOGF->Stripify();
		}
		catch (...)
		{
			clMsg("* ERROR: Flex2OGF, 2nd part, model# %d", MODEL_ID);
		}

		outputOgfTree.push_back(pOGF);
		//		xr_delete			(*it);
		Progress(p_total += p_cost);
	}

	clMsg("%f seconds", start_ogf.GetElapsed_sec());
	clMsg("%f seconds (progressive)", progr_time);
}
