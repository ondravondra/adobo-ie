
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
  public IScriptServiceInstanceAdmin,
  public IScriptServiceInstance
{
private:
  // My id
  CComBSTR                    m_ExtensionId;
  CString                     m_MainModuleID;
  CComPtr<IMagpieApplication> m_Magpie;
  CMainFrame                  m_HiddenWindow;

  // The scripting service. Have to notify him
  // when I'm about to destroy myself
  CScriptServiceCallback*   m_pScriptServiceCallback;

  // Callback interfaces
  struct
  {
    CComQIPtr<IScriptServiceScriptSource> scriptSource;
    HRESULT Set(IUnknown* pUnk)
    {
      Release();
      scriptSource = pUnk;
      return S_OK;
    }
    HRESULT Release()
    {
      scriptSource.Release();
      return S_OK;
    }
  } m_CallbackInterfaces;

public:

  DECLARE_NOT_AGGREGATABLE(CScriptServiceInstance)

  BEGIN_COM_MAP(CScriptServiceInstance)
    COM_INTERFACE_ENTRY(IScriptServiceInstance)
    COM_INTERFACE_ENTRY(IScriptServiceInstanceAdmin)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateObject(CScriptServiceCallback* pService, BSTR extensionId, BSTR resourcesDir, CScriptServiceInstanceComObject *& retVal);
  HRESULT Init(CScriptServiceCallback* pCallback, BSTR extensionId, BSTR resourcesDir);
  void UnInit();

  HRESULT FinalConstruct();
  void FinalRelease();

public:
// IScriptServiceInstanceAdmin
  STDMETHOD(SetCallback)(LPUNKNOWN pUnk);
  STDMETHOD(ReleaseCallback)();

// IScriptServiceInstance
  STDMETHOD(RegisterTab)(INT *tabId);
  STDMETHOD(GetSalsitaApiService)(LPUNKNOWN *pService);
  STDMETHOD(LoadModule)(BSTR moduleID);
  STDMETHOD(MakeJsWindowMemberGlobal)(BSTR memberName);

protected:
  CScriptServiceInstance()
  {
  }

  INT m_TabIdCounter;
  CComPtr<CSalsitaApiServiceImplComObject> m_SalsitaApiServiceImpl;

};
