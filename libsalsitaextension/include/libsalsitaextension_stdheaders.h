/**
 * Include this file in your stdafx.h to get all the headers you need for a basic extension.
 */
#pragma once

#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>

#include <shlguid.h>
#include <shlobj.h>
#include <exdispid.h>
#include <Mshtml.h>

#include <ActivScp.h>

using namespace ATL;

#import "magpie.dll" named_guids raw_interfaces_only raw_native_types no_smart_pointers
using namespace MagpieLib;

#include "libcomhelper.h"
using namespace LIB_COMHelper;
#include "errorchecking.h"

#import "scriptservice.exe" named_guids no_smart_pointers raw_interfaces_only raw_native_types no_auto_exclude
using namespace ScriptServiceLib;
