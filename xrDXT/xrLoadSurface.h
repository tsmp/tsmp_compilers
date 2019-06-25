#pragma once

#ifdef DXT_EXPORTS
#define DXT_API __declspec(dllexport)
#else
#define DXT_API __declspec(dllimport)
#endif

DXT_API void Surface_Init();
DXT_API u32* Surface_Load(char* name, u32& w, u32& h);