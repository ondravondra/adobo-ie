#pragma once

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <atlapp.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlcoll.h>
#include <atlhost.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlframe.h>

using namespace ATL;

#include <exdispid.h>
#include <ActivScp.h>

#include "libcomhelper\include\libcomhelper.h"
using namespace LIB_COMHelper;

#import "magpie.dll" named_guids no_smart_pointers raw_interfaces_only raw_native_types no_auto_exclude
using namespace MagpieLib;
