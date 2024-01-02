#include "stdafx.h"
#include "build.h"

int affected = 0;

void Face::OA_Unwarp()
{
	if (pDeflector)
		return;
	if (!Deflector->OA_Place(this))
		return;

	// now iterate on all our neigbours
	for (int i = 0; i < 3; i++)
	{
		for (vecFaceIt it = v[i]->adjacent.begin(); it != v[i]->adjacent.end(); ++it)
		{
			affected += 1;
			(*it)->OA_Unwarp();
		}
	}
}

void Detach(xr_vector<Face*> &faces)
{
	xr_map<Vertex*, Vertex*> verts;

	// Collect vertices
	for (Face* face: faces)
	{
		for (int i = 0; i < 3; i++)
		{
			Vertex *V = face->v[i];
			Vertex *VC;
			auto it = verts.find(V);

			if (it == verts.end())
			{								// where is no such-vertex
				VC = V->CreateCopy_NOADJ(); // make copy
				verts.insert(mk_pair(V, VC));
			}
			else
			{
				// such vertex(key) already exists - update its adjacency
				VC = it->second;
			}
			VC->prep_add(face);
			V->prep_remove(face);
			face->v[i] = VC;
			// No need to remove V here, it is registered in all vertices container
		}
	}
}

void CBuild::xrPhase_UVmap(xr_vector<vecFace> &splits, xr_vector<CDeflector*> &outDeflectors)
{
	Status("Processing...");
	outDeflectors.reserve(64 * 1024);

	float p_cost = 1.f / float(splits.size());
	float p_total = 0.f;
	vecFace faces_affected;

	for (int SP = 0; SP < int(splits.size()); SP++)
	{
		Progress(1.f * SP / splits.size());

		// Detect vertex-lighting and avoid this subdivision
		R_ASSERT(!splits[SP].empty());
		Face *Fvl = splits[SP].front();
		if (Fvl->Shader().flags.bLIGHT_Vertex)
			continue; // do-not touch (skip)
		if (!Fvl->Shader().flags.bRendering)
			continue; // do-not touch (skip)
		if (Fvl->hasImplicitLighting())
			continue; // do-not touch (skip)

		//   find first poly that doesn't has mapping and start recursion
		while (TRUE)
		{
			// Select maximal sized poly
			Face *msF = NULL;
			float msA = 0;
			for (vecFaceIt it = splits[SP].begin(); it != splits[SP].end(); ++it)
			{
				if ((*it)->pDeflector == NULL)
				{
					float a = (*it)->CalcArea();
					if (a > msA)
					{
						msF = (*it);
						msA = a;
					}
				}
			}

			if (msF)
			{
				outDeflectors.push_back(xr_new<CDeflector>());

				// Start recursion from this face
				affected = 1;
				Deflector->OA_SetNormal(msF->N);
				msF->OA_Unwarp();

				// break the cycle to startup again
				Deflector->OA_Export();

				// Detach affected faces
				faces_affected.clear();
				for (int i = 0; i < int(splits[SP].size()); i++)
				{
					Face *F = (splits[SP])[i];
					if (F->pDeflector == Deflector)
					{
						faces_affected.push_back(F);
						splits[SP].erase(splits[SP].begin() + i);
						i--;
					}
				}

				// detaching itself
				Detach(faces_affected);
				splits.emplace_back(std::move(faces_affected));
			}
			else
			{
				if (splits[SP].empty())
				{
					splits.erase(splits.begin() + SP);
					SP--;
				}
				// Cancel infine loop (while)
				break;
			}
		}
	}

	clMsg("%d subdivisions...", splits.size());
}

void CBuild::mem_Compact() { Memory.mem_compact(); }
