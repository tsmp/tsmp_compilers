#pragma once
#include "xrCore.h"

#pragma warning(disable : 4995)
#include <d3dx9.h>
#include <commctrl.h>
#pragma warning(default : 4995)

#define ENGINE_API
#define ECORE_API
#define XR_EPROPS_API

#include "clsid.h"
#include "..\xr_3da\defines.h"
#include "CommonCompilers\cl_log.h"
#include "CDB\xrCDB.h"
#include "..\xr_3da\_d3d_extensions.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#define READ_IF_EXISTS(ltx, method, section, name, default_value)                                  \
	(ltx->line_exist(section, name)) ? ltx->method(section, name) : default_value

#undef THROW

#if XRAY_EXCEPTIONS
IC xr_string string2xr_string(LPCSTR s) { return s ? s : ""; }
#define THROW(xpr)                                                                                 \
	if (!(xpr))                                                                                    \
	{                                                                                              \
		throw __FILE__LINE__ "\"" #xpr "\"";                                                       \
	}
#define THROW2(xpr, msg0)                                                                          \
	if (!(xpr))                                                                                    \
	{                                                                                              \
		throw xr_string(__FILE__LINE__).append(" \"").append(#xpr).append(string2xr_string(msg0)); \
	}
#define THROW3(xpr, msg0, msg1)                                                                    \
	if (!(xpr))                                                                                    \
	{                                                                                              \
		throw xr_string(__FILE__LINE__)                                                            \
			.append(" \"")                                                                         \
			.append(#xpr)                                                                          \
			.append(string2xr_string(msg0))                                                        \
			.append(", ")                                                                          \
			.append(string2xr_string(msg1));                                                       \
	}
#else
#define THROW VERIFY
#define THROW2 VERIFY2
#define THROW3 VERIFY3
#endif
