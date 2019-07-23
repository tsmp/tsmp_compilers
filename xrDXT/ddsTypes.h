#pragma once
#include <windows.h>
#include <memory.h>

#if defined(WIN32_LEAN_AND_MEAN)
#include <mmsystem.h>	// MAKEFOURCC
#endif

#include "tPixel.h"

typedef	unsigned char	BYTE;
typedef	unsigned short	WORD;
typedef	unsigned long	DWORD;


// filled in by reading a dds file
struct DDS_PIXELFORMAT
{
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwRGBAlphaBitMask;
};
