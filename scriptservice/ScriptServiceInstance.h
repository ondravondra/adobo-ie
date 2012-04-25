
#pragma once

#include "service.h"
#include "resource.h"
#include "MainFrm.h"
#include "SalsitaApiServiceImpl.h"

struct CScriptServiceCallback;

class ATL_NO_VTABLE CScriptServiceInstance :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CScriptServiceInstance, &CLSID_ScriptServiceInstance>,
  public IScriptServiceInstance
{
private:
  CString                     m_ExtensionId;
  CString                     m_MainModuleID;
  CComPtr<IMagpieApplication> m_Magpie;
  CMainFrame                  m_HiddenWindow;

public:

  DECLARE_NOT_AGGREGATABLE(CScriptServiceInstance)
   DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTSERVICEINSTANCE)

  BEGIN_COM_MAP(CScriptServiceInstance)
    COM_INTERFACE_ENTRY(IScriptServiceInstance)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct();
  void FinalRelease();

public:
  STDMETHOD(Init)(const OLECHAR* extensionId, const OLECHAR* resourcesDir);
  STDMETHOD(RegisterTab)(INT *tabId);
  STDMETHOD(GetSalsitaApiService)(LPUNKNOWN *pService);
  STDMETHOD(LoadModule)(const OLECHAR* moduleID);
  STDMETHOD(ScriptCreateNamedItem)(const OLECHAR *name, GUID clsId, ULONG dwFlags);

protected:
  CScriptServiceInstance()
  {
  }

  INT m_TabIdCounter;
  CComPtr<CSalsitaApiServiceImplComObject> m_SalsitaApiServiceImpl;
};

OBJECT_ENTRY_AUTO(__uuidof(ScriptServiceInstance), CScriptServiceInstance)
