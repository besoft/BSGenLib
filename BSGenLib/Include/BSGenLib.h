// This is part of the BeSoft General library.
// Copyright (C) 2003-2006 Josef Kohout.
// All rights reserved.
//
// mailto:besoft@kiv.zcu.cz
//
// This source code can be used, modified and redistributed
// under the terms of the CGDV license agreement.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL CGDV BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef __BSGENLIB
#define __BSGENLIB

#define MAKESTRING2(x)	#x							//trick
#ifdef _UNICODE
#define MAKESTRING(x)	_T(MAKESTRING2(x))
#define MAKESTRING_ANSI(x)	MAKESTRING2(x)
#else
#define MAKESTRING(x)	MAKESTRING2(x)
#define MAKESTRING_ANSI(x)	MAKESTRING2(x)
#endif

#define __BSGEN_VER		1000
#define __BSGEN_VER_S	MAKESTRING_ANSI(__BSGEN_VER)

#pragma message( "BeSoft General Library (BSGen), version " __BSGEN_VER_S )
/////////////// LIBRARY DEFINITIONS  ///////////
#pragma region "Library definitions"
#if (!defined __BSGENLIB_IMPL__)
	//file included by user application

	#if !(defined(__BSGEN_MFC) || defined(__BSGEN_ATL) || defined(__BSGEN_PUREWIN))
		//version to be used is not specified => determine the version to be used
		#if defined(__AFX_H__) || defined(_AFXDLL)
			#define __BSGEN_MFC
			
			#pragma message("Autodetected BSGen MFC version")
		#elif defined(__ATLBASE_H__) || defined(_ATL_DLL)
			#define __BSGEN_ATL

			#pragma message("Autodetected BSGen ATL version")
		#elif defined(_WIN32) || defined(_WIN64)
			#define __BSGEN_PUREWIN

			#pragma message("Autodetected BSGen PURE version. ")
			#pragma message("Use of PURE version is discouraged. ")
		#else
			#error "BSGen requires Microsoft Windows platform"
		#endif
	#endif
#else
	#if !(defined(__BSGEN_MFC) || defined(__BSGEN_ATL) || defined(__BSGEN_PUREWIN))
		#error "BSGen version not specified! (MFC, ATL or PUREWIN)"
	#endif
#endif

#if defined(__BSGEN_MFC)
	#if defined(_DEBUG)
		#if defined(_UNICODE)
			#define __BSGEN_VER_SNAME		"MFC Unicode DEBUG"
			#define __BSGEN_LIBNAME			"BSGenLibumd.lib"
		#else
			#define __BSGEN_VER_SNAME		"MFC DEBUG"
			#define __BSGEN_LIBNAME			"BSGenLibmd.lib"
		#endif
	#else
		#if defined(_UNICODE)
			#define __BSGEN_VER_SNAME		"MFC Unicode RELEASE"
			#define __BSGEN_LIBNAME			"BSGenLibum.lib"
		#else
			#define __BSGEN_VER_SNAME		"MFC RELEASE"
			#define __BSGEN_LIBNAME			"BSGenLibm.lib"
		#endif
	#endif
#elif defined(__BSGEN_ATL)
	#if defined(_DEBUG)
		#if defined(_UNICODE)
			#define __BSGEN_VER_SNAME		"ATL Unicode DEBUG"
			#define __BSGEN_LIBNAME			"BSGenLibaud.lib"
		#else
			#define __BSGEN_VER_SNAME		"ATL DEBUG"
			#define __BSGEN_LIBNAME			"BSGenLibad.lib"
		#endif
	#else
		#if defined(_UNICODE)
			#define __BSGEN_VER_SNAME		"ATL Unicode RELEASE"
			#define __BSGEN_LIBNAME			"BSGenLibau.lib"
		#else
			#define __BSGEN_VER_SNAME		"ATL RELEASE"
			#define __BSGEN_LIBNAME			"BSGenLiba.lib"
		#endif
	#endif
#else
	#if defined(_DEBUG)
		#if defined(_UNICODE)
			#define __BSGEN_VER_SNAME		"PUREWin Unicode DEBUG"
			#define __BSGEN_LIBNAME			"BSGenLibwud.lib"
		#else
			#define __BSGEN_VER_SNAME		"PUREWin DEBUG"
			#define __BSGEN_LIBNAME			"BSGenLibwd.lib"
		#endif
	#else
		#if defined(_UNICODE)
			#define __BSGEN_VER_SNAME		"PUREWin Unicode RELEASE"
			#define __BSGEN_LIBNAME			"BSGenLibwu.lib"
		#else
			#define __BSGEN_VER_SNAME		"PUREWin RELEASE"
			#define __BSGEN_LIBNAME			"BSGenLibw.lib"
		#endif
	#endif
#endif

#if (!defined __BSGENLIB_IMPL__)
	//file included by user application
	#pragma message("Automatically linking with BSGen library: " __BSGEN_VER_SNAME )	
	#pragma comment( lib, __BSGEN_LIBNAME ) 

#else
	#pragma message("Creating BSGen library: " __BSGEN_VER_SNAME )	
#endif

//INCLUDES
#if defined(__BSGEN_MFC)
	#include <afxwin.h>         // MFC core and standard components
	#include <afxext.h>         // MFC extensions		
	#include <atlcoll.h>
	#include <atlcomtime.h>
#elif defined(__BSGEN_ATL)
	#include <atlbase.h>
	#include <atlcoll.h>
	#include <atlcomtime.h>
#else	
	#include <windows.h>

	struct __POSITION { int unused; };
	typedef __POSITION* POSITION;

	#define ATL_NO_VTABLE	__declspec(novtable)
#endif

#include <comdef.h>
#include <tchar.h>
#pragma endregion

/////////////// DEFINES ///////////
#pragma region "DEFINES"
#ifndef BES_ASSERT
	#if defined(__BSGEN_MFC)
		#define BES_ASSERT		ASSERT
		#define BES_VERIFY		VERIFY
	#elif defined(__BSGEN_ATL)
		#define BES_ASSERT		_ASSERTE
		#if defined(_DEBUG)
			#define BES_VERIFY		_ASSERTE
		#else
			#define BES_VERIFY		__noop
		#endif
	#else
		#if defined(_DEBUG)
			#include <assert.h>
			#define BES_ASSERT	assert
			#define BES_VERIFY	assert
		#else
			#define BES_ASSERT __noop
			#define BES_VERIFY __noop
		#endif

		#ifndef ASSERT
			#define ASSERT BES_ASSERT
		#endif

		#ifndef _ASSERTE
			#define _ASSERTE BES_ASSERT
		#endif
	#endif
#endif

#ifndef TRACE
#if defined(__BSGEN_ATL)
#define TRACE	ATLTRACE2
#else
#define TRACE __noop
#endif
#endif

#if !defined(DEBUG_NEW) && defined(_DEBUG) && defined(__BSGEN_ATL)
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)		
#endif

#if _MSC_VER < 1400
	//MS Visual Studio 2003 and lower
	//#error "asgsdag"
#define _ftprintf_s _ftprintf
#define _ftime64_s _ftime
#endif

#pragma endregion

///////////// INCLUDES ///////////////////
#pragma region "INCLUDES"
#define RMCMNAPI

#ifdef __BSGEN_PUREWIN
#pragma warning(push)
#pragma warning(disable : 4995)
#endif

#include "Common/Enum.h"
#include "Common/RefObj.h"
#include "Common/Array.h"
#include "Common/ArrayUtils.h"
#include "Common/Stack.h"
#include "Common/String.h"
#include "Common/SimpleList.h"
#include "Common/List.h"
#include "Common/Path.h"
#include "Common/IniFile.h"
#include "Common/RedBlackTree.h"
#include "Common/ArraysDefs.h"
#include "Common/LogFile.h"
#include "Common/Console.h"
#include "Common/Queue.h"
#include "Common/Buffer.h"
#include "Common/MemMng.h"
#include "Common/MemUtils.h"
#include "Common/DynPool.h"
#include "Common/HashTable.h"
#include "Common/HashTableDefs.h"
#include "Common/PriorityQueue.h"
#include "Common/BSUtil.h"
#include "Common/Convert.h"
#include "Common/Directory.h"
#include "Common/Delegate.h"
#include "Common/RUList.h"
#include "Common/LargeArray.h"

#include "ParSync/Interlocked.h"
#include "ParSync/ObjPtr.h"
#include "ParSync/SharedObj.h"
#include "ParSync/CS.h"
#include "ParSync/WaitHandle.h"
#include "ParSync/Thread.h"
#include "ParSync/ShortCS.h"
#include "ParSync/SyncEvnts.h"
#include "ParSync/SyncLock.h"
#include "ParSync/SyncQueue.h"
#include "ParSync/SyncLogFile.h"
#include "ParSync/LogFileEx.h"
#include "ParSync/Profiling.h"

#include "Utils/FileUtils.h"
#include "Utils/ImageUtils.h"

#ifdef __BSGEN_PUREWIN
#pragma warning(pop)
#endif

#pragma endregion


#endif		//__BSGENLIB