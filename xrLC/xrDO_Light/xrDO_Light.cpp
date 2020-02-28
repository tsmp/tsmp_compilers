#include "stdafx.h"
#include "process.h"
#include "global_options.h"

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"xrShared.lib")

extern void	xrCompiler(LPCSTR name);
extern void logThread(void *dummy);
extern volatile bool bClose;

XRCORE_API void ComputeBuildID(LPCSTR Date);

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h	== this help\n"
	"-f<NAME>	== compile level in gamedata\\levels\\<NAME>\\\n"
	"-norgb			== disable common lightmap calculating\n"
	"-nosun			== disable sun-lighting\n"
	"-o			== modify build options\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help()
{	
	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION); 
}

void Startup(LPSTR lpCmdLine)
{
	char cmd[512],name[256];
	BOOL bModifyOptions = FALSE;

	strcpy(cmd,lpCmdLine);
	strlwr(cmd);

	if (strstr(cmd,"-?") || strstr(cmd,"-h"))			{ Help(); return; }
	if (strstr(cmd,"-f")==0)							{ Help(); return; }
	if (strstr(cmd,"-o"))								bModifyOptions = TRUE;

// KD: new options
	if (strstr(cmd,"-norgb"))							b_norgb			= TRUE;
	if (strstr(cmd,"-nosun"))							b_nosun			= TRUE;

	// Give a LOG-thread a chance to startup
	InitCommonControls	();
	thread_spawn		(logThread,	"log-update", 1024*1024,0);
	Sleep				(150);
	
	// Load project
	name[0]=0; sscanf	(strstr(cmd,"-f")+2,"%s",name);
	//FS.update_path	(name,"$game_levels$",name);
	FS.get_path			("$level$")->_set	(name);

	CTimer				dwStartupTime; dwStartupTime.Start();
	xrCompiler			(0);

	// Show statistic
	char	stats[256];
	extern	std::string make_time(u32 sec);
	extern  HWND logWindow;
	sprintf				(stats,"Time elapsed: %s",make_time((dwStartupTime.GetElapsed_ms())/1000).c_str());
	MessageBox			(logWindow,stats,"Congratulation!",MB_OK|MB_ICONINFORMATION);

	bClose				= TRUE;
	Sleep				(500);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	// KD: let's init debug to enable exception handling
	Debug._initialize	(false);

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	// KD: custom log name
	char app_name[10];
#ifdef _WIN64
	strcpy(app_name, "xrDO_x64");
#else
	strcpy(app_name, "xrDO");
#endif
	// KD: let it be build number like in game
	ComputeBuildID(__DATE__);
	
	Core._initialize	(app_name);
	Startup				(lpCmdLine);
	Core._destroy		();
	
	return 0;
}
