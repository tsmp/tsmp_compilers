/*********************************************************************NVMH2****
Path:  C:\Dev\devrel\Nv_sdk_4\Direct3D\Decompress_DXTC
File:  Image_DXTC.h

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:
A class to load and decompress DXT textures to 32-bit raw image data format.
.RAW output files can be loaded into photoshop by specifying the resolution
and 4 color channels of 8-bit, interleaved.

A few approaches to block decompression are in place and a simple code timing
function is called.  Output of timing test is saved to a local .txt file.

******************************************************************************/

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ddsTypes.h"
#include "dds.h"

enum PixFormat
{
	PF_ARGB,
	PF_DXT1,
	PF_DXT2,
	PF_DXT3,
	PF_DXT4,
	PF_DXT5,
	PF_UNKNOWN
};
