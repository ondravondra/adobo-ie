
#pragma once
class CScriptServiceInstance;
typedef CComObject<CScriptServiceInstance> CScriptServiceInstanceComObject;

#include "service.h"
#include "resource.h"
#include "MainFrm.h"

struct CScriptServiceCallback;

class ATL_NO_VTABLE CScriptServiceInstance :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CScriptServiceInstance>,
  public IScriptServiceInstanceAdmin,
  public IDispatchImpl<IScriptServiceInstance, &IID_IScriptServiceInstance, &LIBID_ScriptingSrvcLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
private:
  // My id
  CComBSTR                    m_bsID;
  CString                     m_MainModuleID;
  CComPtr<IMagpieApplication> m_Magpie;

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
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IScriptServiceInstance)
    COM_INTERFACE_ENTRY(IScriptServiceInstanceAdmin)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  static HRESULT CreateObject(CScriptServiceCallback* pService, BSTR bsID, CScriptServiceInstanceComObject *& retVal);
  HRESULT Init(CScriptServiceCallback* pCallback, BSTR bsID);
  void UnInit();

  HRESULT FinalConstruct();
  void FinalRelease();

public:
// IScriptServiceInstanceAdmin
  STDMETHOD(SetCallback)(LPUNKNOWN pUnk);
  STDMETHOD(ReleaseCallback)();

// IScriptServiceInstance
  STDMETHOD(get_main)(VARIANT * pRet);

protected:
  CScriptServiceInstance()
  {
  }

};

