#include "stdafx.h"
#include "xr_ini.h"
#include "xrAI.h"
#include "xr_graph_merge.h"
#include "game_spawn_constructor.h"
#include "xrCrossTable.h"
#include "game_graph_builder.h"
#include <mmsystem.h>
#include "spawn_patcher.h"
#include "FactoryApi.h"

#pragma comment(lib, "MagicFM.LIB")

extern LPCSTR LEVEL_GRAPH_NAME;

extern void xrCompiler(LPCSTR name, bool draft_mode, bool pure_covers, LPCSTR out_name);
extern void test_smooth_path(LPCSTR name);
extern void test_hierarchy(LPCSTR name);
extern void xrConvertMaps();
extern void test_goap();
extern void smart_cover(LPCSTR name);
extern void verify_level_graph(LPCSTR name, bool verbose);
//extern void connectivity_test	(LPCSTR);
extern void compare_graphs(LPCSTR level_name);
extern void test_levels();

extern void RunCompiler(const char *compilerName, const char *commandLine);

static const char *h_str =
	"The following keys are supported / required:\n"
	"-? or -h   == this help\n"
	"-f<NAME>   == compile level in gamedata/levels/<NAME>/\n"
	"-g<NAME>   == build off-line AI graph and cross-table to ai-map in gamedata/levels/<NAME>/\n"
	"-m         == merge level graphs\n"
	"-s         == build game spawn data\n"
	"-silent	== do not show success message\n"
	"-prior		== set highest priority to compiler\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help() { MessageBox(0, h_str, "Command line options", MB_OK | MB_ICONINFORMATION); }

string_path INI_FILE;
extern LPCSTR GAME_CONFIG;

void execute(LPSTR cmd)
{
	// Load project
	if (strstr(cmd, "-patch"))
	{
		string256 spawn_id, previous_spawn_id;
		sscanf(strstr(cmd, "-patch") + xr_strlen("-patch"), "%s %s", spawn_id, previous_spawn_id);
		spawn_patcher a(spawn_id, previous_spawn_id);
		return;
	}

	string4096 name;
	name[0] = '\0';

	auto ExtractNameFromParam = [&name, &cmd](const char* param)
	{
		sscanf(strstr(cmd, param) + xr_strlen(param), "%s", name);
	};

	if (strstr(cmd, "-f"))
		ExtractNameFromParam("-f");
	else if (strstr(cmd, "-g"))
		ExtractNameFromParam("-g");
	else if (strstr(cmd, "-s"))
		ExtractNameFromParam("-s");
	else if (strstr(cmd, "-t"))
		ExtractNameFromParam("-t");
	else if (strstr(cmd, "-verify"))
		ExtractNameFromParam("-verify");

	if (xr_strlen(name))
		strcat(name, "\\");

	string_path prjName;
	prjName[0] = '\0';
	bool canUseName = false;

	if (xr_strlen(name) < sizeof(string_path))
	{
		canUseName = true;
		FS.update_path(prjName, "$game_levels$", name);
	}

	FS.update_path(INI_FILE, "$game_config$", GAME_CONFIG);

	if (strstr(cmd, "-f"))
	{
		R_ASSERT3(canUseName, "Too big level name", name);

		char *output = strstr(cmd, "-out");
		string256 temp0;
		if (output)
		{
			output += xr_strlen("-out");
			sscanf(output, "%s", temp0);
			_TrimLeft(temp0);
			output = temp0;
		}
		else
			output = (pstr)LEVEL_GRAPH_NAME;

		xrCompiler(prjName, !!strstr(cmd, "-draft"), !!strstr(cmd, "-pure_covers"), output);
		return;
	}

	if (strstr(cmd, "-g"))
	{
		R_ASSERT3(canUseName, "Too big level name", name);
		CGameGraphBuilder().build_graph(prjName);
		return;
	}

	if (strstr(cmd, "-m"))
	{
		xrMergeGraphs(prjName, !!strstr(cmd, "-rebuild"));
		return;
	}

	if (strstr(cmd, "-s"))
	{
		if (xr_strlen(name))
			name[xr_strlen(name) - 1] = 0;

		char *output = strstr(cmd, "-out");
		string256 temp0, temp1;

		if (output)
		{
			output += xr_strlen("-out");
			sscanf(output, "%s", temp0);
			_TrimLeft(temp0);
			output = temp0;
		}

		char *start = strstr(cmd, "-start");
		if (start)
		{
			start += xr_strlen("-start");
			sscanf(start, "%s", temp1);
			_TrimLeft(temp1);
			start = temp1;
		}

		char *no_separator_check = strstr(cmd, "-no_separator_check");
		CGameSpawnConstructor(name, output, start, !!no_separator_check);
		return;
	}

	if (strstr(cmd, "-verify"))
	{
		R_ASSERT3(canUseName, "Too big level name", name);
		verify_level_graph(prjName, !strstr(cmd, "-noverbose"));
	}
}

bool DoCompiler()
{
	string_path systemLtxPath;
	FS.update_path(systemLtxPath, "$game_config$", "system.ltx");
	pSettings = xr_new<CInifile>(systemLtxPath);
	execute(GetCommandLineA());
	xr_delete(pSettings);
	return true;
}

bool ParseCommandLine(const char* cmd)
{
	if (strstr(cmd, "-?") || strstr(cmd, "-h"))
		return false;

	if (!strstr(cmd, "-f") && !strstr(cmd, "-g") && !strstr(cmd, "-m") &&
		!strstr(cmd, "-s") && !strstr(cmd, "-t") && !strstr(cmd, "-c") &&
		!strstr(cmd, "-verify") && !strstr(cmd, "-patch"))
	{
		return false;
	}

	return true;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!ParseCommandLine(lpCmdLine))
	{
		Help();
		return 0;
	}

#ifdef _WIN64
	const char *AppName = "xrAi_x64";
#else
	const char *AppName = "xrAi";
#endif

	RunCompiler(AppName, lpCmdLine);
	return 0;
}
