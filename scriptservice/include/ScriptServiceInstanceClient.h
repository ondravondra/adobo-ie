#pragma once

template <class TScriptServiceFactory>
class ATL_NO_VTABLE CScriptServiceInstanceClient :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TScriptServiceFactory>,
  public IScriptServiceInstanceClient
{
  DECLARE_NOT_AGGREGATABLE(CScriptServiceInstanceClient)

  BEGIN_COM_MAP(CScriptServiceInstanceClient)
    COM_INTERFACE_ENTRY(IScriptServiceInstanceClient)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
  }

  CComPtr<IScriptServiceInstance> m_ScriptServiceInstance;

public:
  static HRESULT CreateObject(IScriptServiceInstance* wrappedInstance, CComObject<CScriptServiceInstanceClient> *& retVal)
  {
    ATLASSERT(wrappedInstance);
    CComObject<CScriptServiceInstanceClient>* pObject = NULL;
    retVal = NULL;
    HRESULT hr = CComObject<CScriptServiceInstanceClient>::CreateInstance(&pObject);
    if (FAILED(hr))
    {
      return hr;
    }
    pObject->m_ScriptServiceInstance = wrappedInstance;
    retVal = pObject;
    return S_OK;
  }

  inline CComPtr<IScriptServiceInstance> GetScriptServiceInstance()
  {
    return m_ScriptServiceInstance;
  }

public:
  STDMETHOD(RegisterTab)(INT *tabId) { return m_ScriptServiceInstance->RegisterTab(tabId); }
  STDMETHOD(GetSalsitaApiService)(LPUNKNOWN *pService) { return m_ScriptServiceInstance->GetSalsitaApiService(pService); }
};
