#pragma once

#include <string>

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

private:
  HRESULT InitScriptServiceInstance()
  {
    std::wstring resourcesDir;
    if (!ResolveResourcesDir(resourcesDir))
    {
      return E_FAIL;
    }

    HRESULT hr = m_ScriptServiceInstance->Init((LPWSTR)GetExtensionId(), (LPWSTR)resourcesDir.c_str());
    if (FAILED(hr) || (hr == S_FALSE))
    {
      return hr;
    }
    
    m_ScriptServiceInstanceIsInitialized = true;

    CComPtr<IUnknown> app;
    hr = m_ScriptServiceInstance->GetApplication(&app.p);
    if (FAILED(hr))
    {
      return hr;
    }

    CComPtr<IMagpieApplication> mpapp;
    hr = app->QueryInterface<IMagpieApplication>(&mpapp.p);
    if (FAILED(hr))
    {
      return hr;
    }

    return SetupScriptServiceInstance(mpapp);
  }

public:
  STDMETHOD(GetScriptServiceInstance)(LPUNKNOWN* ppUnk)
  {
    if (!m_ScriptServiceInstance)
    {
      m_ScriptServiceInstanceIsInitialized = false;
      HRESULT hr = m_ScriptServiceInstance.CoCreateInstance(ScriptServiceLib::CLSID_ScriptServiceInstance);
      if (FAILED(hr))
      {
        return hr;
      }
    }

    if (!m_ScriptServiceInstanceIsInitialized)
    {
      HRESULT hr = InitScriptServiceInstance();
      if (FAILED(hr))
      {
        m_ScriptServiceInstance.Release();
        m_ScriptServiceInstance = NULL;
        return hr;
      }
      if (hr == S_FALSE)
      {
        // Init returned S_FALSE, we must call it again
        *ppUnk = NULL;
        return hr;
      }
    }

    return m_ScriptServiceInstance.QueryInterface<IUnknown>(ppUnk);
  }

protected:
  CScriptServiceFactory() : m_ScriptServiceInstanceIsInitialized(false)
  {
  }

  CComPtr<IScriptServiceInstance> m_ScriptServiceInstance;
  bool m_ScriptServiceInstanceIsInitialized; ///< true iff m_ScriptServiceInstance::Init returned S_OK
  
  /**
   * Performs loading of modules into a newly created script service instance.
   * The instance has already been successfuly initialized using the Init method.
   * @param pApplication pointer to the script application running in the instance.
   */
  virtual HRESULT SetupScriptServiceInstance(CComPtr<IMagpieApplication> pApplication) = 0;

  /**
   * Returns an extension identifier which will be passed to magpie.
   */
  virtual const wchar_t *GetExtensionId() const = 0;

  /**
   * Retrieves path to the directory containing web resources.
   * For example implementation see similar method in libsalsitaextension.
   */
  virtual bool ResolveResourcesDir(std::wstring &result) const = 0;
};
