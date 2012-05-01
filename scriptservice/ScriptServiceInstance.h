
#pragma once
class CScriptServiceInstance;
typedef CComObject<CScriptServiceInstance> CScriptServiceInstanceComObject;

#include "service.h"
#include "resource.h"
#include "MainFrm.h"
#include "SalsitaApiServiceImpl.h"

struct CScriptServiceCallback;

class ATL_NO_VTABLE CScriptServiceInstance :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CScriptServiceInstance>,
  public IScriptServiceInstance
{
private:
  // My id
  CString                     m_ExtensionId;
  CString                     m_MainModuleID;
  CComPtr<IMagpieApplication> m_Magpie;
  CMainFrame                  m_HiddenWindow;

  // The scripting service. Have to notify him
  // when I'm about to destroy myself
  CScriptServiceCallback*   m_pScriptServiceCallback;

public:

  DECLARE_NOT_AGGREGATABLE(CScriptServiceInstance)

  BEGIN_COM_MAP(CScriptServiceInstance)
    COM_INTERFACE_ENTRY(IScriptServiceInstance)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateObject(CScriptServiceCallback* pService, const OLECHAR* extensionId, const OLECHAR* resourcesDir, CScriptServiceInstanceComObject *& retVal);
  HRESULT Init(CScriptServiceCallback* pCallback, const OLECHAR* extensionId, const OLECHAR* resourcesDir);
  void UnInit();

  HRESULT FinalConstruct();
  void FinalRelease();

public:
// IScriptServiceInstance
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
