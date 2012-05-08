#pragma once

namespace LIB_COMHelper {

/**
 * Handles pushing manifest into the activation stack.
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa374166%28v=vs.85%29.aspx
 * This enables loading of com libraries without the need for registration.
 */
class CActivationHelper
{
private:
  WCHAR path[MAX_PATH];
  DWORD error;
  HANDLE contextHandle;
  ULONG_PTR cookie;
public:
  /**
   * Creates activation context for given required dll.
   * Loads the manifest from the dll's resource identified by its name.
   * The dll is expected to be located in the same directory as the caller whose module handle gets passed to the ctor.
   */
  CActivationHelper(HMODULE callerModule, const wchar_t *requiredDllName, const wchar_t *manifestResourceName = L"activation.manifest");
  ~CActivationHelper();
  inline DWORD GetErrorCode() { return error; }
};

} // namespace LIB_COMHelper