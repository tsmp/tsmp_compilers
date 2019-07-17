#pragma once

#pragma warning(disable:4595)
#pragma warning(disable:4091)
#pragma warning(disable:4577)

#include "xrCore.h"
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

#define ENGINE_API
#define XR_EPROPS_API
#define ECORE_API

#pragma comment(lib, "nvtt.lib")

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

#pragma comment(lib,"xrShared.lib")
#pragma warning( disable : 4995 )
