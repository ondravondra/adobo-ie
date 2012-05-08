#pragma once

#include <string>

#include "ScriptServiceInstanceClient.h"

template <class TFactoryImpl, const CLSID* pclsid>
class ATL_NO_VTABLE CScriptServiceFactory :
  public IScriptServiceFactoryReleaseCallback,
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TFactoryImpl, pclsid>,
  public IScriptServiceFactory
{
public:
  DECLARE_CLASSFACTORY_SINGLETON(TFactoryImpl)
  DECLARE_NOT_AGGREGATABLE(TFactoryImpl)

  BEGIN_COM_MAP(TFactoryImpl)
    COM_INTERFACE_ENTRY(IScriptServiceFactory)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
    if (m_ScriptServiceInstance)
    {
      // we got destroyed before m_ScriptServiceInstance
      // remove the callback from it
      // it will get deleted automatically
      m_ScriptServiceInstance->UnInit();
      m_ScriptServiceInstance = NULL;
    }
  }

  typedef CComObject<CScriptServiceInstanceClient<TFactoryImpl>> CScriptServiceInstanceClientComObject;

private:
  HRESULT InitScriptServiceInstance(CComPtr<IScriptServiceInstance> pInstance)
  {
    std::wstring resourcesDir;
    if (!ResolveResourcesDir(resourcesDir))
    {
      return E_FAIL;
    }

    HRESULT hr = pInstance->Init((LPWSTR)GetExtensionId(), (LPWSTR)resourcesDir.c_str());
    if (FAILED(hr) || (hr == S_FALSE))
    {
      return hr;
    }

    CComPtr<IUnknown> app;
    hr = pInstance->GetApplication(&app.p);
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

  HRESULT CreateNewScriptServiceInstance()
  {
    m_ScriptServiceInstanceIsInitialized = false;
    CComPtr<IScriptServiceInstance> pServiceInstance;
    HRESULT hr = pServiceInstance.CoCreateInstance(ScriptServiceLib::CLSID_ScriptServiceInstance);
    if (FAILED(hr))
    {
      return hr;
    }

    hr = CScriptServiceInstanceClient<TFactoryImpl>::CreateObject(pServiceInstance.p, this, m_ScriptServiceInstance);
    return hr;
  }

public:
  STDMETHOD(GetScriptServiceInstance)(LPUNKNOWN* ppUnk)
  {
    if (!m_ScriptServiceInstance)
    {
      HRESULT hr = CreateNewScriptServiceInstance();
      if (FAILED(hr))
      {
        return hr;
      }
    }

    if (!m_ScriptServiceInstanceIsInitialized)
    {
      HRESULT hr = InitScriptServiceInstance(m_ScriptServiceInstance->GetScriptServiceInstance());
      if (FAILED(hr))
      {
        delete m_ScriptServiceInstance;
        m_ScriptServiceInstance = NULL;
        return hr;
      }
      if (hr == S_FALSE) // Init returned S_FALSE, client must call us again
      {
        m_ScriptServiceInstanceIsInitialized = false;
        *ppUnk = NULL;
        return hr;
      } else {
        m_ScriptServiceInstanceIsInitialized = true;
      }
    }

    return m_ScriptServiceInstance->QueryInterface<IUnknown>(ppUnk);
  }

public:
  virtual void OnFinalRelease()
  {
    // m_ScriptServiceInstance got deleted (all clients disconnected)
    // remove the reference
    m_ScriptServiceInstance = NULL;
  }

protected:
  CScriptServiceFactory() : m_ScriptServiceInstanceIsInitialized(false), m_ScriptServiceInstance(NULL)
  {
  }

  CScriptServiceInstanceClientComObject *m_ScriptServiceInstance;
  bool m_ScriptServiceInstanceIsInitialized;
  
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

  /**
   * Returns HMODULE handle of the com exe server process containing the factory.
   */
  virtual HMODULE GetServerModule() const
  {
    return GetModuleHandle(NULL);
  }
};
