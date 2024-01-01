#pragma once

#include "CommonCompilers\communicate.h"
#include "b_globals.h"
#include "fs.h"
#include "..\xr_3da\xrLevel.h"
#include "CommonCompilers\ETextureParams.h"
#include "..\xr_3da\shader_xrlc.h"
#include "xrMU_Model.h"

#pragma comment(lib, "dxt.lib")

struct b_BuildTexture : public b_texture
{
	STextureParams THM;

	u32 &Texel(u32 x, u32 y) { return pSurface[y * dwWidth + x]; }
	void Vflip()
	{
		R_ASSERT(pSurface);
		for (u32 y = 0; y < dwHeight / 2; y++)
		{
			u32 y2 = dwHeight - y - 1;
			for (u32 x = 0; x < dwWidth; x++)
			{
				u32 t = Texel(x, y);
				Texel(x, y) = Texel(x, y2);
				Texel(x, y2) = t;
			}
		}
	}
};

// tesselator callbacks
typedef int tesscb_estimator(Face *F); // -1 = none, 0,1,2 = edge-number
typedef void tesscb_face(Face *F);	   // new face
typedef void tesscb_vertex(Vertex *V); // new vertex

class CBuild
{
public:
	CMemoryWriter err_invalid;
	CMemoryWriter err_tjunction;
	CMemoryWriter err_multiedge;
	void err_save();

	Fbox scene_bb;

	xr_vector<b_material> materials;
	xr_vector<b_shader> shader_render;
	xr_vector<b_shader> shader_compile;
	xr_vector<b_BuildTexture> textures;

	xr_vector<b_glow> glows;
	xr_vector<b_portal> portals;
	xr_vector<b_lod> lods;

	base_lighting L_static;
	xr_vector<b_light_dynamic> L_dynamic;

	xr_vector<xrMU_Model *> mu_models;
	xr_vector<xrMU_Reference *> mu_refs;

	Shader_xrLC_LIB shaders;
	string_path path;

	void mem_Compact();

public:
	void Load(const b_params &P, const IReader &fs, HWND Window);
	void Run(LPCSTR path);

	void PreOptimize();
	void CorrectTJunctions();

	void xrPhase_AdaptiveHT();
	void u_Tesselate(tesscb_estimator *E, tesscb_face *F, tesscb_vertex *V);
	void u_SmoothVertColors(int count);

	void CalcNormals();
	void xrPhase_TangentBasis();

	void BuildCForm();
	void BuildPortals(IWriter &fs);
	void BuildRapid(BOOL bSave);

	void xrPhase_Radiosity();
	void xrPhase_MU_light();

	void IsolateVertices(BOOL bProgress);

	// Split all faces into groups by materials
	void xrPhase_ResolveMaterials(const xr_vector<Face*> &inputFaces, xr_vector<vecFace*> &outputSplits);

	void xrPhase_UVmap(xr_vector<vecFace*> &splits, xr_vector<CDeflector*> &outDeflectors);

	void xrPhase_Subdivide(xr_vector<vecFace*> &splits, xr_vector<CDeflector*> &deflectors);

	void ImplicitLighting();
	void Light_prepare();
	void Light(xr_vector<CDeflector*> &deflectors);
	void LightVertex();
	void xrPhase_MergeLM(const xr_vector<CDeflector*> &deflectors);
	void xrPhase_MergeGeometry(xr_vector<vecFace*> &splits);

	void Flex2OGF(xr_vector<vecFace*> &inputSplits, xr_vector<OGF_Base*> &outputOgfTree);
	void BuildSectors(xr_vector<OGF_Base*> &ogfTree);

	void SaveLights(IWriter &fs);
	void SaveTREE(IWriter &fs, xr_vector<OGF_Base*> &ogfTree);
	void SaveSectors(IWriter &fs, const xr_vector<OGF_Base*> &ogfTree);

	// Writes in log when split has more faces than limit
	void ValidateSplits(const xr_vector<vecFace*> &splits);

	CBuild() = default;
	~CBuild() = default;
};

extern CBuild *pBuild;
