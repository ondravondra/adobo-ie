#pragma once

#include <string>
#include "SalsitaExtensionHelper.h"

template <class Timpl>
class CSalsitaExtension :
  public IObjectWithSiteImpl<Timpl>,
  public IDispEventImpl<1, Timpl, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>,
  public IDispEventImpl<1, Timpl, &DIID__IMagpieLoggerEvents, &LIBID_MagpieLib, 1, 0>
{
protected:
  DWORD m_WebBrowserEventsCookie;
  CComPtr<IWebBrowser2> m_WebBrowser;

  std::wstring m_ResourcesDir; ///< Directory holding web resources. This directory serves as top namespace using which all CommonJS module ids are resolved.

  CComPtr<IMagpieApplication> m_Magpie;
  DWORD m_MagpieEventsCookie;

  CComPtr<IScriptServiceFactory> m_ScriptServiceFactory;
  CComPtr<IScriptServiceInstance> m_ScriptServiceInstance;
  bool m_BackgroundScriptStarted, m_BackgroundScriptFailed;

  INT m_TabId;

  /**
   * Returns an identifier which will be passed to magpie.
   */
  virtual const wchar_t *GetExtensionId() const = 0;

  /**
   * Can the content script be executed when there is no background script available?
   * By default it cannot.
   */
  virtual bool CanRunWithoutBackgroundScript() const
  {
    return false;
  }

  /**
   * Returns CLSID of the background script service factory for this extension.
   */
  virtual const IID *GetBackgroundScriptServiceFactoryCLSID() const = 0;

  /**
   * Retrieves path to the directory containing web resources and stores it in m_ResourcesDir.
   */
  virtual bool ResolveResourcesDir() = 0;
  /**
   * Example implementation:
  {
#ifdef _DEBUG
    CSalsitaExtensionHelper::ResourcesDirGetDebugPath(_WSTR(RES_DIR_DEBUG_PATH), m_ResourcesDir);
    return true;
#else
    return CSalsitaExtensionHelper::ResourcesDirReadFromRegistry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Company\\Extension", L"resourcesDir", m_ResourcesDir);
#endif
  }
  */
 
  /**
   * Executes content scripts.
   * Gets called inside RunContentScriptOnNavigation once a magpie instance is created and initialized.
   */
  virtual HRESULT ExecuteContentScript() = 0;

  CSalsitaExtension() :
    m_WebBrowserEventsCookie(0), m_MagpieEventsCookie(0),
    m_BackgroundScriptStarted(false), m_BackgroundScriptFailed(false)
  {
  }

  HRESULT CreateMagpieInstance()
  {
    HRESULT hr;

    // create a m_Magpie instance
    m_Magpie.CoCreateInstance(CLSID_MagpieApplication);
    if (!m_Magpie.p)
      return E_FAIL;

    VARIANT vtTabId;
    if (m_BackgroundScriptStarted)
    {
      vtTabId.intVal = m_TabId;
      vtTabId.vt = VT_INT;
    } else {
      vtTabId.vt = VT_ERROR;
    }

    // set the resources directory
    hr = m_Magpie->Init((LPWSTR)GetExtensionId(), (LPWSTR)m_ResourcesDir.c_str(), vtTabId);
    if (FAILED(hr))
      return E_FAIL;

    // catch console events
    if (FAILED(hr = AtlAdvise(m_Magpie, (IUnknown*)(IDispEventImpl<1, Timpl, &DIID__IMagpieLoggerEvents, &LIBID_MagpieLib, 1, 0>*)this, DIID__IMagpieLoggerEvents, &m_MagpieEventsCookie)))
      return E_FAIL;

    return S_OK;
  }

  void DestroyMagpieInstance()
  {
    if (m_Magpie)
    {
      if (m_MagpieEventsCookie)
      {
        AtlUnadvise(m_Magpie, DIID__IMagpieLoggerEvents, m_MagpieEventsCookie);
        m_MagpieEventsCookie = 0;
      }
      m_Magpie->Shutdown();
      m_Magpie.Release();
    }
  }

  virtual HRESULT InternalSetSite()
  {
    ATLASSERT(m_spUnkSite);

    if (!ResolveResourcesDir())
    {
      return E_FAIL;
    }

    // get IServiceProvider to get IWebBrowser2 and IOleWindow
    CComQIPtr<IServiceProvider> pServiceProvider = m_spUnkSite;
    if (!pServiceProvider)
    {
      return E_FAIL;
    }

    // get IWebBrowser2
    pServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (LPVOID*)&m_WebBrowser.p);
    if (!m_WebBrowser)
    {
      return E_FAIL;
    }

    // advise DWebBrowserEvents2
    ATLASSERT(!m_WebBrowserEventsCookie);
    AtlAdvise(m_WebBrowser, (IUnknown*)(IDispEventImpl<1, Timpl, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>*)this, DIID_DWebBrowserEvents2, &m_WebBrowserEventsCookie);

    return S_OK;
  }

  virtual HRESULT InternalReleaseSite()
  {
    if (m_WebBrowser)
    {
      if (m_WebBrowserEventsCookie)
      {
        AtlUnadvise(m_WebBrowser, DIID_DWebBrowserEvents2, m_WebBrowserEventsCookie);
        m_WebBrowserEventsCookie = 0;
      }
      m_WebBrowser.Release();
    }

    DestroyMagpieInstance();

    m_ScriptServiceInstance.Release();
    m_ScriptServiceFactory.Release();

    return S_OK;
  }

  HRESULT CreateSalsitaApi()
  {
    HRESULT hr;

    CComPtr<IUnknown> pApiSrvUnk;
    hr = m_ScriptServiceInstance->GetSalsitaApiService(&pApiSrvUnk.p);
    if (FAILED(hr))
    {
      return hr;
    }

    hr = m_Magpie->CreateSalsitaApi(m_TabId, pApiSrvUnk);
    if (FAILED(hr))
    {
      return hr;
    }
    return S_OK;
  }

  BOOL IsFrame(IDispatch* pDisp)
  {
    ATLASSERT(m_WebBrowser);
    if (!m_WebBrowser)
    {
      return TRUE;
    }
    CComQIPtr<IWebBrowser2> thisBrowser = pDisp;
    return (!thisBrowser || !thisBrowser.IsEqualObject(m_WebBrowser));
  }

  HRESULT RunContentScriptOnNavigation(IDispatch *pWebBrowser)
  {
    if (IsFrame(pWebBrowser))
    {
      return S_FALSE;
    }

    DestroyMagpieInstance();

    HRESULT hr;

    hr = CreateMagpieInstance();
    if (FAILED(hr))
    {
      return hr;
    }

    if (m_BackgroundScriptStarted)
    {
      // Some extensions can run without background scripts.
      // If they do, however, then the content script cannot use the salsita api as there is no event handling mechanism available.
      hr = CreateSalsitaApi();
      if (FAILED(hr))
      {
        return hr;
      }
    } else {
      if (!CanRunWithoutBackgroundScript())
      {
        return E_FAIL;
      }
    }

    CComQIPtr<IHTMLWindow2> window = CComUtil::IWebBrowserToIHTMLWindow(m_WebBrowser);
    hr = m_Magpie->ScriptAddNamedItem(L"window", window, SCRIPTITEM_ISSOURCE|SCRIPTITEM_ISVISIBLE|SCRIPTITEM_GLOBALMEMBERS);

    if (SUCCEEDED(hr))
    {
      hr = ExecuteContentScript();
    }

    return hr;
  }

  HRESULT RunBackgroundScript()
  {
    HRESULT hr;

    if (m_BackgroundScriptFailed)
    {
      return E_FAIL;
    }

    if (m_BackgroundScriptStarted)
    {
      return S_OK;
    }

    m_BackgroundScriptFailed = true;

    CComPtr<IUnknown> pFactoryObject;
    hr = pFactoryObject.CoCreateInstance(*GetBackgroundScriptServiceFactoryCLSID());
    if (FAILED(hr))
    {
      return hr;
    }

    hr = pFactoryObject.QueryInterface<IScriptServiceFactory>(&m_ScriptServiceFactory.p);
    if (FAILED(hr))
    {
      return hr;
    }

    CComPtr<IUnknown> instance;
    hr = m_ScriptServiceFactory->GetScriptServiceInstance(&instance.p);
    if (FAILED(hr))
    {
      return hr;
    }

    hr = instance.QueryInterface<IScriptServiceInstance>(&m_ScriptServiceInstance.p);
    if (FAILED(hr))
    {
      return hr;
    }

    hr = m_ScriptServiceInstance->RegisterTab(&m_TabId);
    if (FAILED(hr))
    {
      return hr;
    }

    m_BackgroundScriptStarted = true;
    m_BackgroundScriptFailed = false;
    return S_OK;
  }

public:
  // IObjectWithSite
  STDMETHODIMP SetSite(IUnknown *pUnkSite)
  {
    m_spUnkSite = pUnkSite;

    if (m_spUnkSite)
    {
      InternalSetSite();
    }
    else
    {
      InternalReleaseSite();
    }
    return S_OK;
  }
};
