#pragma once

class CSector
{
	u32 SelfID;
	OGF_Base *TreeRoot;
	xr_vector<u16> Portals;
	xr_vector<u16> Glows;
	xr_vector<u16> Lights;

public:
	void add_portal(u16 P) { Portals.push_back(P); }
	void add_glow(u16 G) { Glows.push_back(G); }
	void add_light(u16 L) { Lights.push_back(L); }

	void BuildHierrarhy(xr_vector<OGF_Base*> &ogfTree);
	void Validate();
	void Save(IWriter &fs, const xr_vector<OGF_Base*> &ogfTree);

	CSector(u32 ID);
	~CSector();
};

extern xr_vector<CSector*> g_sectors;
