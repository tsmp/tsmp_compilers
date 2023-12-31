// Sector.cpp: implementation of the CSector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "build.h"
#include "Sector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSector::CSector(u32 ID)
{
	SelfID = ID;
	TreeRoot = 0;
}

CSector::~CSector() {}

IC BOOL ValidateMerge(Fbox &bb_base, Fbox &bb, float &volume, float SLimit)
{
	// Size
	Fbox merge;
	merge.merge(bb_base, bb);
	Fvector sz;
	merge.getsize(sz);
	sz.add(EPS_L);
	if (sz.x > SLimit)
		return FALSE; // Don't exceed limits (4/3 GEOM)
	if (sz.y > SLimit)
		return FALSE;
	if (sz.z > SLimit)
		return FALSE;

	// Volume
	volume = merge.getvolume();

	// OK
	return TRUE;
}

void CSector::BuildHierrarhy(xr_vector<OGF_Base*> &ogfTree)
{
	Fvector scene_size;
	float delimiter;
	BOOL bAnyNode = FALSE;

	// calc scene BB
	Fbox &scene_bb = pBuild->scene_bb;
	scene_bb.invalidate();
	for (int I = 0; I < s32(ogfTree.size()); I++)
		scene_bb.merge(ogfTree[I]->bbox);
	scene_bb.grow(EPS_L);

	scene_bb.getsize(scene_size);
	delimiter = _max(scene_size.x, _max(scene_size.y, scene_size.z));
	delimiter *= 2;

	int iLevel = 2;
	float SizeLimit = c_SS_maxsize / 4.f;
	if (SizeLimit < 4.f)
		SizeLimit = 4.f;
	if (delimiter <= SizeLimit)
		delimiter *= 2; // just very small level

	for (; SizeLimit <= delimiter; SizeLimit *= 2)
	{
		int iSize = ogfTree.size();

		for (int I = 0; I < iSize; I++)
		{
			if (ogfTree[I]->bConnected)
				continue;

			if (ogfTree[I]->Sector != SelfID)
				continue;

			OGF_Node *pNode = xr_new<OGF_Node>(iLevel, u16(SelfID));
			pNode->AddChild(I, ogfTree);

			// Find best object to connect with
			for (;;)
			{
				// Find best object to connect with
				int best_id = -1;
				float best_volume = flt_max;

				for (int J = 0; J < iSize; J++)
				{
					OGF_Base *candidate = ogfTree[J];
					if (candidate->bConnected)
						continue;
					if (candidate->Sector != SelfID)
						continue;

					float V;
					if (ValidateMerge(pNode->bbox, candidate->bbox, V, SizeLimit))
					{
						if (V < best_volume)
						{
							best_volume = V;
							best_id = J;
						}
					}
				}

				// Analyze
				if (best_id < 0)
					break;
				pNode->AddChild(best_id, ogfTree);
			}

			if (pNode->children.size() > 1)
			{
				pNode->CalcBounds(ogfTree);
				ogfTree.push_back(pNode);
				bAnyNode = TRUE;
			}
			else
			{
				ogfTree[I]->bConnected = false;
				xr_delete(pNode);
			}
		}

		if (iSize != (int)ogfTree.size())
			iLevel++;
	}
	TreeRoot = 0;
	if (bAnyNode)
		TreeRoot = ogfTree.back();
	else
	{
		for (u32 I = 0; I < ogfTree.size(); I++)
		{
			if (ogfTree[I]->bConnected)
				continue;

			if (ogfTree[I]->Sector != SelfID)
				continue;

			R_ASSERT(0 == TreeRoot);
			TreeRoot = ogfTree[I];
		}
	}
	if (0 == TreeRoot)
	{
		clMsg("Can't build hierrarhy for sector #%d", SelfID);
	}
}

void CSector::Validate()
{
	std::sort(Portals.begin(), Portals.end());
	R_ASSERT(std::unique(Portals.begin(), Portals.end()) == Portals.end());
	R_ASSERT(TreeRoot);
	R_ASSERT(TreeRoot->Sector == SelfID);
}

void CSector::Save(IWriter &fs, const xr_vector<OGF_Base*> &ogfTree)
{
	// Root
	auto rootIt = std::find(ogfTree.begin(), ogfTree.end(), TreeRoot);
	R_ASSERT(rootIt != ogfTree.end());
	u32 ID = u32(rootIt - ogfTree.begin());
	fs.w_chunk(fsP_Root, &ID, sizeof(u32));

	// Portals
	fs.w_chunk(fsP_Portals, &*Portals.begin(), Portals.size() * sizeof(u16));
}
