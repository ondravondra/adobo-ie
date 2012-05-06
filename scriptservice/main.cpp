#include "stdafx.h"
#include "service.h"
#include "service_i.c"
#include "resource.h"

class CScriptingSrvcModule : public CAtlDllModuleT<CScriptingSrvcModule>
{
public :
  DECLARE_LIBID(LIBID_ScriptServiceLib)
  DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SCRIPTSERVICE, "{95034CAB-82B8-41FA-9FCA-7B0AE7E87FAF}")
};

CScriptingSrvcModule _AtlModule;

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
  return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
  return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
  // registers object, typelib and all interfaces in typelib
  HRESULT hr = _AtlModule.DllRegisterServer();
  return hr;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
  HRESULT hr = _AtlModule.DllUnregisterServer();
  return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user
//              per machine.	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
  HRESULT hr = E_FAIL;
  static const wchar_t szUserSwitch[] = _T("user");

  if (pszCmdLine != NULL)
  {
    if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    {
      AtlSetPerUserRegistration(true);
    }
  }

  if (bInstall)
  {	
    hr = DllRegisterServer();
    if (FAILED(hr))
    {	
      DllUnregisterServer();
    }
  }
  else
  {
    hr = DllUnregisterServer();
  }

  return hr;
}
