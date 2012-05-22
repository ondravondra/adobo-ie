/**
 * Include this file in your stdafx.h to get all the headers you need for a basic extension.
 */
#pragma once

#define ISOLATION_AWARE_ENABLED 1

#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>

#include <shlguid.h>
#include <shlobj.h>
#include <exdispid.h>
#include <Mshtml.h>

#include <ActivScp.h>

using namespace ATL;

#include "libcomhelper.h"
using namespace LIB_COMHelper;
#include "errorchecking.h"

#pragma comment(linker, "/include:_library_version_libsalsitaextension")
extern "C" {
  extern const wchar_t *library_version_libsalsitaextension;
}
