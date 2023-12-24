///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Main file for Opcode.dll.
 *	\file		Opcode.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPCODE_H__
#define __OPCODE_H__

#include "..\xrcore.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Preprocessor
#define OPCODE_API
#ifndef __ICECORE_H__
#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#endif // WIN32

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

#ifndef ASSERT
#define ASSERT assert
#endif

#define Log
#define SetIceError false
#define EC_OUTOFMEMORY "Out of memory"
#define Alignment

#define ICECORE_API OPCODE_API

#endif

#ifndef __ICEMATHS_H__
#include <Math.h>
#define ICEMATHS_API OPCODE_API
namespace IceMaths
{
#include "OPC_Point.h"
}
using namespace IceMaths;
#endif

namespace Meshmerizer
{
#include "OPC_Triangle.h"
#include "OPC_AABB.h"
} // namespace Meshmerizer
using namespace Meshmerizer;

namespace Opcode
{
// Bulk-of-the-work
#include "OPC_Common.h"
#include "OPC_TreeBuilders.h"
#include "OPC_AABBTree.h"
#include "OPC_OptimizedTree.h"
#include "OPC_Model.h"
} // namespace Opcode

#endif // __OPCODE_H__
