#include "stdafx.h"
#include "include\ActivationHelper.h"

namespace LIB_COMHelper {

CActivationHelper::CActivationHelper(HMODULE callerModule) :
  contextHandle(INVALID_HANDLE_VALUE), cookie(0), error(0)
{
#ifndef DISABLE_REGFREE_ACTIVATION // see root CMakeLists.txt for explanation
  if (!GetModuleFileNameW(callerModule, path, MAX_PATH))
  {
    error = GetLastError();
    goto leaveCtor;
  }

  wchar_t * lastSlash = wcsrchr(path, L'\\');
  if (!lastSlash)
  {
    lastSlash = wcsrchr(path, L'/');
  }

  if (lastSlash)
  {
    wchar_t ch = *lastSlash;
    *lastSlash = 0;
    wcscpy(dir, path);
    *lastSlash = ch;
  } else {
    wcscpy(dir, path);
  }

  ACTCTXW actCtx;
  memset(&actCtx, 0, sizeof(actCtx));
  actCtx.cbSize = sizeof(actCtx);
  actCtx.lpSource = path;
  actCtx.lpAssemblyDirectory = dir;
  actCtx.lpResourceName = MAKEINTRESOURCE(2);
  actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID;

  contextHandle = ::CreateActCtx(&actCtx);
  if (contextHandle == INVALID_HANDLE_VALUE)
  {
    error = GetLastError();
    goto leaveCtor;
  }

  if (!::ActivateActCtx(contextHandle, &cookie))
  {
    error = GetLastError();
    goto leaveCtor;
  }

leaveCtor:
  ATLASSERT(error == ERROR_SUCCESS);
#endif // not DISABLE_REGFREE_ACTIVATION
}

CActivationHelper::~CActivationHelper()
{
#ifndef DISABLE_REGFREE_ACTIVATION
  if (!error)
  {
    ::DeactivateActCtx(0, cookie);
  }

  if (contextHandle != INVALID_HANDLE_VALUE)
  {
    ::ReleaseActCtx(contextHandle);
  }
#endif // not DISABLE_REGFREE_ACTIVATION
}

}// namespace LIB_COMHelper