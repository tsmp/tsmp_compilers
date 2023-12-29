#include "stdafx.h"
#include "xr_ini.h"
#include "process.h"
#include "xrAI.h"
#include "xr_graph_merge.h"
#include "game_spawn_constructor.h"
#include "xrCrossTable.h"
#include "game_graph_builder.h"
#include <mmsystem.h>
#include "spawn_patcher.h"
#include "FactoryApi.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "IMAGEHLP.LIB")
#pragma comment(lib, "winmm.LIB")
#pragma comment(lib, "MagicFM.LIB")
#pragma comment(lib, "xrCore.LIB")

typedef void DUMMY_STUFF(const void*, const u32&, void*);
XRCORE_API DUMMY_STUFF* g_temporary_stuff;

#define TRIVIAL_ENCRYPTOR_DECODER
#include "../xr_3da/trivial_encryptor.h"

extern LPCSTR LEVEL_GRAPH_NAME;

extern void xrCompiler(LPCSTR name, bool draft_mode, bool pure_covers, LPCSTR out_name);
extern void logThread(void *dummy);
extern volatile bool bClose;
extern void test_smooth_path(LPCSTR name);
extern void test_hierarchy(LPCSTR name);
extern void xrConvertMaps();
extern void test_goap();
extern void smart_cover(LPCSTR name);
extern void verify_level_graph(LPCSTR name, bool verbose);
//extern void connectivity_test	(LPCSTR);
extern void compare_graphs(LPCSTR level_name);
extern void test_levels();

static const char *h_str =
	"The following keys are supported / required:\n"
	"-? or -h   == this help\n"
	"-f<NAME>   == compile level in gamedata/levels/<NAME>/\n"
	"-o         == modify build options\n"
	"-g<NAME>   == build off-line AI graph and cross-table to ai-map in gamedata/levels/<NAME>/\n"
	"-m         == merge level graphs\n"
	"-s         == build game spawn data\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help() { MessageBox(0, h_str, "Command line options", MB_OK | MB_ICONINFORMATION); }

string_path INI_FILE;
extern HWND logWindow;
extern LPCSTR GAME_CONFIG;


void execute(LPSTR cmd)
{
	// Load project
	string4096 name;
	name[0] = 0;

	if (strstr(cmd, "-patch"))
	{
		string256 spawn_id, previous_spawn_id;
		sscanf(strstr(cmd, "-patch") + xr_strlen("-patch"), "%s %s", spawn_id, previous_spawn_id);
		spawn_patcher a(spawn_id, previous_spawn_id);
		return;
	}
	else
	{
		if (strstr(cmd, "-f"))
			sscanf(strstr(cmd, "-f") + 2, "%s", name);
		else if (strstr(cmd, "-g"))
			sscanf(strstr(cmd, "-g") + 2, "%s", name);
		else if (strstr(cmd, "-s"))
			sscanf(strstr(cmd, "-s") + 2, "%s", name);
		else if (strstr(cmd, "-t"))
			sscanf(strstr(cmd, "-t") + 2, "%s", name);
		else if (strstr(cmd, "-verify"))
			sscanf(strstr(cmd, "-verify") + xr_strlen("-verify"), "%s", name);

		if (xr_strlen(name))
			strcat(name, "\\");
	}

	string_path prjName;
	prjName[0] = 0;
	bool can_use_name = false;

	if (xr_strlen(name) < sizeof(string_path))
	{
		can_use_name = true;
		FS.update_path(prjName, "$game_levels$", name);
	}

	FS.update_path(INI_FILE, "$game_config$", GAME_CONFIG);

	if (strstr(cmd, "-f"))
	{
		R_ASSERT3(can_use_name, "Too big level name", name);

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
	}
	else if (strstr(cmd, "-g"))
	{
		R_ASSERT3(can_use_name, "Too big level name", name);
		CGameGraphBuilder().build_graph(prjName);
	}
	else
	{
		if (strstr(cmd, "-m"))
		{
			xrMergeGraphs(prjName, !!strstr(cmd, "-rebuild"));
		}
		else
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
			}
			else if (strstr(cmd, "-verify"))
			{
				R_ASSERT3(can_use_name, "Too big level name", name);
				verify_level_graph(prjName, !strstr(cmd, "-noverbose"));
			}
	}
}

void Startup(LPSTR lpCmdLine)
{
	string4096 cmd;
	BOOL bModifyOptions = FALSE;

	strcpy(cmd, lpCmdLine);
	strlwr(cmd);
	if (strstr(cmd, "-?") || strstr(cmd, "-h"))
	{
		Help();
		return;
	}
	if ((strstr(cmd, "-f") == 0) && (strstr(cmd, "-g") == 0) && (strstr(cmd, "-m") == 0) &&
		(strstr(cmd, "-s") == 0) && (strstr(cmd, "-t") == 0) && (strstr(cmd, "-c") == 0) &&
		(strstr(cmd, "-verify") == 0) && (strstr(cmd, "-patch") == 0))
	{
		Help();
		return;
	}
	if (strstr(cmd, "-o"))
		bModifyOptions = TRUE;

	// Give a LOG-thread a chance to startup
	InitCommonControls();
	Sleep(150);
	thread_spawn(logThread, "log-update", 1024 * 1024, 0);
	while (!logWindow)
		Sleep(150);

	u32 dwStartupTime = timeGetTime();
	execute(cmd);
	// Show statistic
	char stats[256];
	extern std::string make_time(u32 sec);
	extern HWND logWindow;
	u32 dwEndTime = timeGetTime();
	sprintf(stats, "Time elapsed: %s", make_time((dwEndTime - dwStartupTime) / 1000).c_str());
	MessageBox(logWindow, stats, "Congratulation!", MB_OK | MB_ICONINFORMATION);

	bClose = TRUE;
	FlushLog();
	Sleep(500);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Core._initialize("xrai", 0);
	
	g_temporary_stuff = &trivial_encryptor::decode;
	string_path systemLtxPath;
	FS.update_path(systemLtxPath, "$game_config$", "system.ltx");
	pSettings = xr_new<CInifile>(systemLtxPath);

	Startup(lpCmdLine);

	xr_delete(pSettings);
	Core._destroy();
	return 0;
}
