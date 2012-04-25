#pragma once

#include "service.h"
#include "resource.h"

template <class T, const CLSID* pclsid>
class ATL_NO_VTABLE CScriptServiceFactory :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<T, pclsid>,
  IScriptServiceFactory
{
public:
  CScriptServiceFactory()
  {
  }
  
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

  virtual HRESULT CreateScriptServiceInstance() = 0;

  STDMETHOD(GetScriptServiceInstance)(LPUNKNOWN* ppUnk)
  {
    if (!m_ScriptServiceInstance)
    {
      HRESULT hr = CreateScriptServiceInstance();
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
  CComPtr<IScriptServiceInstance> m_ScriptServiceInstance;
};

class ATL_NO_VTABLE CTestFactory :
  public CScriptServiceFactory<CTestFactory, &CLSID_TestFactory>
{
public:
  CTestFactory()
  {
  }

  DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTSERVICE)

  virtual HRESULT CreateScriptServiceInstance();
};

OBJECT_ENTRY_AUTO(__uuidof(TestFactory), CTestFactory)