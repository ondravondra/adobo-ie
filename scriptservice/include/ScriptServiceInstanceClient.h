#pragma once

/**
 * Used to guard the reference to CScriptServiceInstanceClient
 */
struct IScriptServiceFactoryReleaseCallback
{
  virtual void OnFinalRelease() = 0;
};

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

  IScriptServiceFactoryReleaseCallback *m_releaseCallback;

  void FinalRelease()
  {
    if (m_releaseCallback)
    {
      m_releaseCallback->OnFinalRelease();
      m_releaseCallback = NULL;
    }
  }

  CComPtr<IScriptServiceInstance> m_ScriptServiceInstance;

public:
  static HRESULT CreateObject(IScriptServiceInstance* wrappedInstance, IScriptServiceFactoryReleaseCallback* callback, CComObject<CScriptServiceInstanceClient> *& retVal)
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
    pObject->m_releaseCallback = callback;
    retVal = pObject;
    return S_OK;
  }

  inline CComPtr<IScriptServiceInstance> GetScriptServiceInstance()
  {
    return m_ScriptServiceInstance;
  }

  void UnInit()
  {
    m_ScriptServiceInstance = NULL;
  }

public:
  STDMETHOD(RegisterTab)(INT *tabId) { return m_ScriptServiceInstance->RegisterTab(tabId); }
  STDMETHOD(GetSalsitaApiService)(LPUNKNOWN *pService) { return m_ScriptServiceInstance->GetSalsitaApiService(pService); }
};
