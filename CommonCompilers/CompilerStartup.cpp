#include "stdafx.h"
#include "thread"
#include <CommCtrl.h>

XRCORE_API void ComputeBuildID(LPCSTR Date);
extern bool DoCompiler();

extern void logThread(void *dummy);
extern volatile bool bClose;
extern HWND logWindow;

bool IsSilent = false;
extern BOOL b_highest_priority;
extern int i_ThreadCount;

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "IMAGEHLP.LIB")
#pragma comment(lib, "winmm.LIB")
#pragma comment(lib, "xrCore.lib")

void ParseCommonCmdOptions(const char *cmd) 
{
	if (strstr(cmd, "-prior"))
		b_highest_priority = true;

	if (strstr(cmd, "-silent"))
		IsSilent = true;
}

void InitCompiler(const char* compilerName)
{
	Debug._initialize(false);
	ComputeBuildID(__DATE__);
	Core._initialize(compilerName);
}

void DeinitCompiler()
{
	Core._destroy();
}

void RunCompiler(const char *compilerName, const char *commandLine)
{
	ParseCommonCmdOptions(commandLine);
	InitCompiler(compilerName);

	// Give a LOG-thread a chance to startup
	InitCommonControls();
	thread_spawn(logThread, "log-update", 1024 * 1024, 0);
	Sleep(150);

	if (b_highest_priority)
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	i_ThreadCount = std::thread::hardware_concurrency();

	CTimer dwStartupTime;
	dwStartupTime.Start();

	if (DoCompiler())
	{
		// Show statistic
		extern std::string make_time(u32 sec);
		u32 dwEndTime = dwStartupTime.GetElapsed_ms();
		char inf[256];
		sprintf(inf, "Time elapsed: %s", make_time(dwEndTime / 1000).c_str());
		clMsg("Build succesful!\n%s", inf);

		if (!IsSilent)
			MessageBox(logWindow, inf, "Congratulation!", MB_OK | MB_ICONINFORMATION);
	}

	// Close log
	bClose = true;
	Sleep(500);

	DeinitCompiler();
}
