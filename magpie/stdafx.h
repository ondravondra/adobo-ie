// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include "resource.h"
#include <atlbase.h>
#include <atlapp.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlcoll.h>
#include <atlfile.h>
#include <activdbg.h>
#include <atlwin.h>
#include <atlframe.h>

#include <shlguid.h>
#include <exdispid.h>

using namespace ATL;

extern HINSTANCE g_hDllInstance;

#include "Misc.h"
using namespace Misc;

#include "libcomhelper\include\libcomhelper.h"
using namespace LIB_COMHelper;

#include "libcomhelper\include\errorchecking.h"

// see msdn for IDispatchImpl, this constant makes it possible to lookup typeinfo in the dll instead of requiring registry entries
#define IDISP_MAGPIE_LIBVERSION 0xFFFF, 0xFFFF