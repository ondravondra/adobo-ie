#pragma once

template <class T, const CLSID* pclsid>
class ATL_NO_VTABLE CScriptServiceFactory :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<T, pclsid>,
  IScriptServiceFactory
{
public:
  DECLARE_CLASSFACTORY_SINGLETON(T)
  DECLARE_NOT_AGGREGATABLE(T)

  BEGIN_COM_MAP(T)
    COM_INTERFACE_ENTRY(IScriptServiceFactory)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
  }

  virtual HRESULT InitScriptServiceInstance() = 0;

  STDMETHOD(GetScriptServiceInstance)(LPUNKNOWN* ppUnk)
  {
    if (!m_ScriptServiceInstance)
    {
      HRESULT hr = m_ScriptServiceInstance.CoCreateInstance(__uuidof(IScriptServiceInstance));
      if (FAILED(hr))
      {
        return hr;
      }
      hr = InitScriptServiceInstance();
      if (FAILED(hr))
      {
        m_ScriptServiceInstance.Release();
        m_ScriptServiceInstance = NULL;
        return hr;
      }
    }
    return m_ScriptServiceInstance.QueryInterface<IUnknown>(ppUnk);
  }

protected:
  CScriptServiceFactory()
  {
  }

  CComPtr<IScriptServiceInstance> m_ScriptServiceInstance;
};
