#include "stdafx.h"
#include "build.h"

void Detach(xr_vector<Face*> &faces);

struct MateraialCounter
{
	u16 dwMaterial;
	u32 dwCount;
};

void CBuild::xrPhase_ResolveMaterials(const xr_vector<Face*> &inputFaces, xr_vector<vecFace*> &outputSplits)
{
	// Count number of materials
	Status("Calculating materials/subdivs...");
	xr_vector<MateraialCounter> materialCounts;
	materialCounts.reserve(256);

	u32 iteration = 0;
	const float facesCount = static_cast<float>(inputFaces.size());

	for (Face *face : inputFaces)
	{
		if (!face->Shader().flags.bRendering)
		{
			iteration++;
			continue;
		}

		auto it = std::find_if(materialCounts.begin(), materialCounts.end(), [face](const MateraialCounter &counter)
		{
			return counter.dwMaterial == face->dwMaterial;
		});

		if (it == materialCounts.end())
		{
			auto &newMtl = materialCounts.emplace_back();
			newMtl.dwCount = 1;
			newMtl.dwMaterial = face->dwMaterial;
		}
		else
			it->dwCount++;

		Progress(static_cast<float>(iteration) / facesCount);
		iteration++;
	}

	Status("Perfroming subdivisions...");
	outputSplits.resize(materialCounts.size());

	for (u32 i = 0, cnt = materialCounts.size(); i < cnt; i++)
	{
		outputSplits[i] = xr_new<vecFace>();
		outputSplits[i]->reserve(materialCounts[i].dwCount);
	}

	iteration = 0;
	const u32 materialsCnt = materialCounts.size();

	for (Face* face: inputFaces)
	{
		for (u32 i = 0; i < materialsCnt; i++)
		{
			if (face->dwMaterial == materialCounts[i].dwMaterial)
				outputSplits[i]->push_back(face);
		}

		Progress(static_cast<float>(iteration) / facesCount);
		iteration++;
	}

	Status("Detaching subdivs...");

	for (auto split : outputSplits)
		Detach(*split);

	clMsg("%d subdivisions.", outputSplits.size());
}
