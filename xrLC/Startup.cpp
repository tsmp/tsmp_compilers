#include "stdafx.h"
#include "build.h"

extern void RunCompiler(const char *compilerName, const char *commandLine);

CBuild *pBuild = nullptr;
extern HWND logWindow;
char LevelName[256];

static const char *h_str = "The following keys are supported / required:\n"
						   "-? or -h		== this help\n"
						   "-nosun			== disable sun-lighting\n"
						   "-noise			== disable converting to MU\n"
						   "-norgb			== disable common lightmap calculating\n"
						   "-nolmaps		== disable lightmaps calculating\n"
						   "-skipinvalid	== skip crash if invalid faces exists\n"
						   "-lmap_quality	== lightmap quality\n"
						   "-lmap_rgba     == save lightmaps with lossless format\n"
						   "-notess			== disable geometry tesselation\n"
						   "-silent			== do not show success message\n"
						   "-prior			== set highest priority to compiler\n"
						   "-f<NAME>		== compile level in GameData\\Levels\\<NAME>\\\n"
						   "\n"
						   "NOTE: The last key is required for any functionality\n";

void Help() { MessageBox(0, h_str, "Command line options", MB_OK | MB_ICONINFORMATION); }

bool DoCompiler()
{
	// Load project
	string_path prjName;
	FS.update_path(prjName, "$game_levels$", strconcat(sizeof(prjName), prjName, LevelName, "\\build.prj"));

	Phase("Reading project...");
	clMsg("Reading project [%s]", LevelName);
	IReader *F = FS.r_open(prjName);

	if (!F)
	{
		string256 inf;
		sprintf(inf, "Build failed!\nCan't find level: '%s'", LevelName);
		clMsg(inf);
		MessageBox(logWindow, inf, "Error!", MB_OK | MB_ICONERROR);
		return false;
	}

	// Version
	u32 version;
	F->r_chunk(EB_Version, &version);
	clMsg("version: %d", version);
	R_ASSERT(XRCL_CURRENT_VERSION == version);

	// Header
	b_params Params;
	F->r_chunk(EB_Parameters, &Params);
	Params.m_lm_pixels_per_meter = f_lmap_quality;

	// Conversion
	Phase("Converting data structures...");
	pBuild = xr_new<CBuild>();
	pBuild->Load(Params, *F, logWindow);
	FS.r_close(F);

	// Call for builder
	string_path lfn;
	FS.update_path(lfn, _game_levels_, LevelName);
	pBuild->Run(lfn);
	xr_delete(pBuild);
	return true;
}

bool ParseCommandLine(const char* cmd)
{
	if (strstr(cmd, "-?") || strstr(cmd, "-h"))
		return false;

	if (!strstr(cmd, "-f"))
		return false;

	auto GetConsoleParam = [](const char *cmd, const char *param_name, const char *expr, float *param)
	{
		if (strstr(cmd, param_name))
		{
			int sz = xr_strlen(param_name);
			sscanf(strstr(cmd, param_name) + sz, expr, param);
		}
	};

	auto GetConsoleFloat = [&](const char *cmd, const char *param_name, float *param)
	{
		GetConsoleParam(cmd, param_name, "%f", param);
	};

	if (strstr(cmd, "-gi"))
		b_radiosity = TRUE;

	if (strstr(cmd, "-noise"))
		b_noise = TRUE;

	if (strstr(cmd, "-nosun"))
		b_nosun = TRUE;

	if (strstr(cmd, "-lmap_rgba"))
		b_lmap_rgba = TRUE;

	if (strstr(cmd, "-norgb"))
		b_norgb = TRUE;

	if (strstr(cmd, "-nolmaps"))
		b_nolmaps = TRUE;

	if (strstr(cmd, "-skipinvalid"))
		b_skipinvalid = TRUE;

	GetConsoleFloat(cmd, "-lmap_quality ", &f_lmap_quality);

	LevelName[0] = '\0';
	sscanf(strstr(cmd, "-f") + 2, "%s", LevelName);
	return true;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!ParseCommandLine(lpCmdLine))
	{
		Help();
		return 0;
	}

#ifdef _WIN64
	const char* AppName = "xrLC_x64";
#else
	const char *AppName = "xrLC";
#endif

	RunCompiler(AppName, lpCmdLine);
	return 0;
}
