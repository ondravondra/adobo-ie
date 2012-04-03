
#pragma once

#include "service.h"
#include "resource.h"

struct CScriptServiceCallback;

class ATL_NO_VTABLE CScriptServiceInstance :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CScriptServiceInstance>,
  public IScriptServiceInstanceAdmin,
  public IDispatchImpl<IScriptServiceInstance, &IID_IScriptServiceInstance, &LIBID_scriptservicelib>
{
private:
  // My id
  CComBSTR                  m_bsID;
  //CMainFrame                m_HiddenWindow;
  CAtlMap<void*, CComPtr<IDispatch> >
    m_Handlers;

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
  CScriptServiceInstance()
  {
  }

  DECLARE_NOT_AGGREGATABLE(CScriptServiceInstance)

  BEGIN_COM_MAP(CScriptServiceInstance)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IScriptServiceInstance)
    COM_INTERFACE_ENTRY(IScriptServiceInstanceAdmin)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT Init(CScriptServiceCallback* pCallback, BSTR bsID);
  void UnInit();

  HRESULT FinalConstruct();
  void FinalRelease();

public:
  // IScriptServiceInstanceAdmin
  STDMETHOD(SetCallback)(LPUNKNOWN pUnk);
  STDMETHOD(ReleaseCallback)();

  // IScriptServiceInstance
  STDMETHOD(addMsgHandler)(LPDISPATCH handler, LONG* pvtRet);
  STDMETHOD(removeMsgHandler)(LONG handlerID);
  STDMETHOD(sendMsg)(LONG handlerID, VARIANT data);
};
typedef CComObject<CScriptServiceInstance> CScriptServiceInstanceComObject;
