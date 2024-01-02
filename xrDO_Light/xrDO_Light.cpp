#include "stdafx.h"
#include "process.h"
#include "global_options.h"

extern void RunCompiler(const char *compilerName, const char *commandLine);

extern void xrCompiler(LPCSTR name);
char LevelName[256];

static const char *h_str = "The following keys are supported / required:\n"
						   "-? or -h		== this help\n"
						   "-f<NAME>		== compile level in gamedata\\levels\\<NAME>\\\n"
						   "-norgb			== disable common lightmap calculating\n"
						   "-nosun			== disable sun-lighting\n"
						   "-silent			== do not show success message\n"
						   "-prior			== set highest priority to compiler\n"
						   "\n"
						   "NOTE: The last key is required for any functionality\n";

void Help() { MessageBox(0, h_str, "Command line options", MB_OK | MB_ICONINFORMATION); }

bool DoCompiler()
{
	// Load project
	FS.get_path("$level$")->_set(LevelName);
	xrCompiler(0);
	return true;
}

bool ParseCommandLine(const char* cmd)
{
	if (strstr(cmd, "-?") || strstr(cmd, "-h"))
		return false;

	if (!strstr(cmd, "-f"))
		return false;

	if (strstr(cmd, "-norgb"))
		b_norgb = TRUE;

	if (strstr(cmd, "-nosun"))
		b_nosun = TRUE;

	LevelName[0] = '\0';
	sscanf(strstr(cmd, "-f") + 2, "%s", LevelName);
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
	const char *AppName = "xrDO_x64";
#else
	const char *AppName = "xrDO";
#endif

	RunCompiler(AppName, lpCmdLine);
	return 0;
}
