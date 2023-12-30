#include "stdafx.h"
#include "build.h"

void Detach(xr_vector<Face*> &faces);

void setup_bbs(Fbox &b1, Fbox &b2, const Fbox &bb, int edge)
{
	Fvector size;
	b1.set(bb);
	b2.set(bb);
	size.sub(bb.max, bb.min);
	switch (edge)
	{
	case 0:
		b1.max.x -= size.x / 2;
		b2.min.x += size.x / 2;
		break;
	case 1:
		b1.max.y -= size.y / 2;
		b2.min.y += size.y / 2;
		break;
	case 2:
		b1.max.z -= size.z / 2;
		b2.min.z += size.z / 2;
		break;
	}
};

const u32 MinSubdivisionSize = c_SS_LowVertLimit * 2;
const u32 MaxDeflectorLayerSize = c_LMAP_size - 2 * BORDER;

bool NeedSplitSubdivision(const xr_vector<Face*> &subdiv, Fbox &bbox, Fvector &size)
{
	// Skip if subdivision is already too small
	if (subdiv.size() < MinSubdivisionSize)
		return false;

	// Calc bounding box
	bbox.invalidate();

	for (const Face *face : subdiv)
	{
		bbox.modify(face->v[0]->P);
		bbox.modify(face->v[1]->P);
		bbox.modify(face->v[2]->P);
	}

	// Analyze if we need to split
	size.sub(bbox.max, bbox.min);

	if (size.x > c_SS_maxsize || size.y > c_SS_maxsize || size.z > c_SS_maxsize)
		return true;

	if (subdiv.size() > c_SS_HighVertLimit)
		return true;

	if (const auto deflector = reinterpret_cast<CDeflector*>(subdiv[0]->pDeflector))
	{
		if (deflector->layer.width >= MaxDeflectorLayerSize)
			return true;

		if (deflector->layer.height >= MaxDeflectorLayerSize)
			return true;
	}

	return false;
}

int GetLongestEdgeIdx(const Fvector &size)
{
	if (size.x >= size.y && size.x >= size.z)
		return 0;

	if (size.y >= size.x && size.y >= size.z)
		return 1;

	return 2;
}

bool SplitSubdivision(const xr_vector<Face *> &subdiv, const Fbox &bb, Fvector &size, vecFace &s1, vecFace &s2)
{
	Fbox b1, b2;
	int box_edge = GetLongestEdgeIdx(size);
	setup_bbs(b1, b2, bb, box_edge);

	// Process all faces and rearrange them
	u32 Edge = 0; // up to 3
	u32 IterationPerEdge = 0; // up to 10
	bool resplit;

	do
	{
		resplit = false;
		IterationPerEdge++;

		for (Face *face : subdiv)
		{
			Fvector center;
			face->CalcCenter(center);

			if (b1.contains(center))
				s1.emplace_back(face);
			else
				s2.emplace_back(face);
		}

		if (s1.size() >= c_SS_LowVertLimit && s2.size() >= c_SS_LowVertLimit)
			return true;

		// Splitting failed
		clMsg("! ERROR: model - split fail, faces: %d, s1/s2:%d/%d", subdiv.size(), s1.size(), s2.size());

		if (IterationPerEdge < 10)
		{
			if (subdiv.size() > c_SS_LowVertLimit * 4)
			{
				if (s2.size() > s1.size())
				{
					// b2-less, b1-grow
					size.sub(b2.max, b2.min);
					b1.max[box_edge] += size[box_edge] / 2;
					b2.min[box_edge] = b1.max[box_edge];
				}
				else
				{
					// b2-grow, b1-less
					size.sub(b1.max, b1.min);
					b2.min[box_edge] -= size[box_edge] / 2;
					b1.max[box_edge] = b2.min[box_edge];
				}

				resplit = true;
			}
		}
		else
		{
			// Switch edge
			Edge++;
			IterationPerEdge = 0;

			if (Edge < 3)
			{
				box_edge = (box_edge + 1) % 3;
				setup_bbs(b1, b2, bb, box_edge);
				resplit = true;
			}
		}

		if (resplit)
		{
			s1.clear();
			s2.clear();
		}
	}
	while (resplit);

	return false;
}

void CBuild::xrPhase_Subdivide(xr_vector<vecFace*> &splits, xr_vector<CDeflector*> deflectors)
{
	Status("Subdividing in space...");

	for (u32 i = 0; i < splits.size(); i++)
	{
		// Remove if empty
		if (splits[i]->empty())
		{
			xr_delete(splits[i]);
			splits.erase(splits.begin() + i);
			i--;
			continue;
		}

		Progress(static_cast<float>(i) / static_cast<float>(splits.size()));

		// Perform subdivide if needed
		Fbox bb;
		Fvector size;

		if (!NeedSplitSubdivision(*splits[i], bb, size))
			continue;

		vecFace newSubdiv1, newSubdiv2;

		if (SplitSubdivision(*splits[i], bb, size, newSubdiv1, newSubdiv2))
		{
			// Split deflector into TWO
			if (const auto deflector = reinterpret_cast<CDeflector*>(splits[i]->front()->pDeflector))
			{
				// Delete old deflector
				for (u32 it = 0; it < deflectors.size(); it++)
				{
					if (deflectors[it] == deflector)
					{
						deflectors.erase(deflectors.begin() + it);
						xr_delete(deflector);
						break;
					}
				}

				// Create new deflectors
				CDeflector *D1 = deflectors.emplace_back(xr_new<CDeflector>());
				D1->OA_Place(newSubdiv1);
				D1->OA_Export();
				CDeflector *D2 = deflectors.emplace_back(xr_new<CDeflector>());
				D2->OA_Place(newSubdiv2);
				D2->OA_Export();
			}

			// Delete old SPLIT and push two new
			xr_delete(splits[i]);
			splits.erase(splits.begin() + i);
			i--;
			splits.emplace_back(xr_new<vecFace>(newSubdiv1));
			Detach(newSubdiv1);
			splits.emplace_back(xr_new<vecFace>(newSubdiv2));
			Detach(newSubdiv2);

			// Detach duplicates vertices, on super huge maps could run out of memory.
			// To avoid that we will clean unused vertices periodically
			if (i % 5000 == 0)
				IsolateVertices(false);
		}
	}

	clMsg("%d subdivisions.", splits.size());
	ValidateSplits(splits);
}
