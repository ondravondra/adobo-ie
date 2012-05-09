#include "stdafx.h"
#include "include\ActivationHelper.h"

namespace LIB_COMHelper {

CActivationHelper::CActivationHelper(HMODULE callerModule, const wchar_t *requiredDllName, const wchar_t *manifestResourceName) :
  contextHandle(INVALID_HANDLE_VALUE), cookie(0), error(0)
{
#ifdef USE_ACTIVATION_MANIFESTS // see root CMakeLists.txt for explanation
  if (!GetModuleFileNameW(callerModule, path, MAX_PATH))
  {
    error = GetLastError();
    return;
  }

  wchar_t * lastSlash = wcsrchr(path, L'\\');
  if (lastSlash)
  {
    lastSlash[1] = 0;
  }

  if (wcscat_s(path, requiredDllName) != 0)
  {
    error = -1;
    return;
  }

  ACTCTXW actCtx;
  memset(&actCtx, 0, sizeof(actCtx));
  actCtx.cbSize = sizeof(actCtx);
  actCtx.lpSource = path;
  actCtx.lpResourceName = manifestResourceName;
  actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;

  contextHandle = ::CreateActCtx(&actCtx);
  if (contextHandle == INVALID_HANDLE_VALUE)
  {
    error = GetLastError();
    return;
  }

  if (!::ActivateActCtx(contextHandle, &cookie))
  {
    error = GetLastError();
  }
#endif
}

CActivationHelper::~CActivationHelper()
{
#ifdef USE_ACTIVATION_MANIFESTS
  if (!error)
  {
    ::DeactivateActCtx(0, cookie);
  }

  if (contextHandle != INVALID_HANDLE_VALUE)
  {
    ::ReleaseActCtx(contextHandle);
  }
#endif
}

}// namespace LIB_COMHelper