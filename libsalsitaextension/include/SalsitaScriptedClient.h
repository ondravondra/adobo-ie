#pragma once

#include <string>
#include "SalsitaExtensionHelper.h"
#include "ActivationHelper.h"

#include "adoboversion.h"
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='scriptservice' version='" ADOBO_VERSION "' \
language='*'\"")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='magpie' version='" ADOBO_VERSION "' \
language='*'\"")

template <class Timpl>
class CSalsitaScriptedClient
{
protected:
  std::wstring m_ResourcesDir; ///< Directory holding web resources. This directory serves as top namespace using which all CommonJS module ids are resolved.

  CComPtr<IMagpieApplication> m_Magpie;

  CComPtr<IScriptServiceFactory> m_ScriptServiceFactory;
  CComPtr<IScriptServiceInstanceClient> m_ScriptServiceInstance;
  bool m_BackgroundScriptConnected, m_BackgroundScriptFailed;

  INT m_TabId;

  CSalsitaScriptedClient() : m_BackgroundScriptConnected(false), m_BackgroundScriptFailed(false)
  {
  }

  /**
   * Returns HMODULE handle of the dll library containing the extension.
   */
  virtual HMODULE GetLibraryModule() const = 0;

  /**
   * Returns an extension identifier which will be passed to magpie.
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
   * Retrieves path to the directory containing web resources.
   * Example implementation:
   {
#ifdef _DEBUG
     CSalsitaExtensionHelper::ResourcesDirGetDebugPath(_WSTR(RES_DIR_DEBUG_PATH), result);
     return true;
#else
     return CSalsitaExtensionHelper::ResourcesDirReadFromRegistry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Company\\Extension", L"resourcesDir", result);
#endif
   }
   */
  virtual bool ResolveResourcesDir(std::wstring &result) = 0;

  /**
   * Executes content scripts.
   * Gets called inside ReloadContentScript once a magpie instance is created and initialized.
   */
  virtual HRESULT ExecuteContentScript() = 0;

  /**
   * Returns IHTMLWindow2 which will be passed to the content script as "window".
   * Can return S_FALSE to skip adding the window object.
   */
  virtual HRESULT GetWindowForScript(CComQIPtr<IHTMLWindow2> &window) = 0;

  /**
   * Returns the site of the BHO/toolbar which the implementor receives in the SetSite method.
   */
  virtual void GetBrowserSite(CComPtr<IUnknown> &site) = 0;

  virtual HRESULT CreateMagpieInstance()
  {
    HRESULT hr;

    // create a m_Magpie instance
    hr = m_Magpie.CoCreateInstance(CLSID_MagpieApplication);
    if (FAILED(hr))
    {
      return E_FAIL;
    }

    VARIANT vtTabId;
    if (m_BackgroundScriptConnected)
    {
      vtTabId.intVal = m_TabId;
      vtTabId.vt = VT_INT;
    } else {
      vtTabId.vt = VT_ERROR;
    }

    // set the resources directory
    hr = m_Magpie->Init((LPWSTR)GetExtensionId(), (LPWSTR)m_ResourcesDir.c_str(), vtTabId);
    if (FAILED(hr))
    {
      return E_FAIL;
    }

    return S_OK;
  }

  virtual void DestroyMagpieInstance()
  {
    if (m_Magpie)
    {
      m_Magpie->Shutdown();
      m_Magpie.Release();
    }
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

    CComPtr<IUnknown> site;
    GetBrowserSite(site);

    if (site)
    {
      CComPtr<IDispatch> pSalsitaContentApi;
      hr = m_Magpie->CreateSalsitaContentApiImplementation(site, &pSalsitaContentApi.p);
      if (FAILED(hr))
      {
        return hr;
      }

      hr = m_Magpie->CreateSalsitaApi(m_TabId, pApiSrvUnk, CComVariant(pSalsitaContentApi.p));
    } else {
      hr = m_Magpie->CreateSalsitaApi(m_TabId, pApiSrvUnk);
    }

    if (FAILED(hr))
    {
      return hr;
    }
    return S_OK;
  }

  HRESULT ReloadContentScript()
  {
    DestroyMagpieInstance();

    HRESULT hr;

    CActivationHelper actHelper(GetLibraryModule(), L"magpie.dll");

    hr = CreateMagpieInstance();
    if (FAILED(hr))
    {
      return hr;
    }

    if (m_BackgroundScriptConnected)
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

    CComQIPtr<IHTMLWindow2> window;
    hr = GetWindowForScript(window);
    if (hr == S_OK) // can be S_FALSE to skip adding the window named item
    {
      hr = m_Magpie->ScriptAddNamedItem(L"window", window, SCRIPTITEM_ISSOURCE|SCRIPTITEM_ISVISIBLE|SCRIPTITEM_GLOBALMEMBERS);
    }

    if (SUCCEEDED(hr))
    {
      hr = ExecuteContentScript();
    }

    return hr;
  }

  HRESULT ConnectToBackgroundScript()
  {
    HRESULT hr;

    if (m_BackgroundScriptFailed)
    {
      return E_FAIL;
    }

    if (m_BackgroundScriptConnected)
    {
      return S_OK;
    }

    const IID *factoryCLSID = GetBackgroundScriptServiceFactoryCLSID();
    if (!factoryCLSID)
    {
      return S_FALSE;
    }

    m_BackgroundScriptFailed = true;

    CActivationHelper actHelper(GetLibraryModule(), L"scriptservice.dll"); ///< need to access interfaces from scriptservice.dll

    CComPtr<IUnknown> pFactoryObject;
    hr = pFactoryObject.CoCreateInstance(*factoryCLSID);
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
    do {
      instance.Release();
      hr = m_ScriptServiceFactory->GetScriptServiceInstance(&instance.p);
      if (FAILED(hr))
      {
        return hr;
      }
    } while (hr == S_FALSE); ///< we must wait until server initialization is done

    hr = instance.QueryInterface<IScriptServiceInstanceClient>(&m_ScriptServiceInstance.p);
    if (FAILED(hr))
    {
      return hr;
    }

    hr = m_ScriptServiceInstance->RegisterTab(&m_TabId);
    if (FAILED(hr))
    {
      return hr;
    }

    m_BackgroundScriptConnected = true;
    m_BackgroundScriptFailed = false;
    return S_OK;
  }

  void CleanupScriptedClient()
  {
    DestroyMagpieInstance();

    m_ScriptServiceInstance.Release();
    m_ScriptServiceFactory.Release();
  }
};

template <class Timpl>
class CSalsitaScriptedClientWithEvents :
  public CSalsitaScriptedClient<Timpl>,
  public IDispEventImpl<1, Timpl, &DIID__IMagpieLoggerEvents, &LIBID_MagpieLib, 1, 0>
{
protected:
  DWORD m_MagpieEventsCookie;

  CSalsitaScriptedClientWithEvents() : m_MagpieEventsCookie(0)
  {
  }

  virtual HRESULT CreateMagpieInstance()
  {
    HRESULT hr = CSalsitaScriptedClient::CreateMagpieInstance();
    if (FAILED(hr))
    {
      return E_FAIL;
    }

    // catch console events
    if (FAILED(hr = AtlAdvise(m_Magpie, (IUnknown*)(IDispEventImpl<1, Timpl, &DIID__IMagpieLoggerEvents, &LIBID_MagpieLib, 1, 0>*)this, DIID__IMagpieLoggerEvents, &m_MagpieEventsCookie)))
    {
      return E_FAIL;
    }

    return S_OK;
  }

  virtual void DestroyMagpieInstance()
  {
    if (m_Magpie)
    {
      if (m_MagpieEventsCookie)
      {
        AtlUnadvise(m_Magpie, DIID__IMagpieLoggerEvents, m_MagpieEventsCookie);
        m_MagpieEventsCookie = 0;
      }
    }

    CSalsitaScriptedClient::DestroyMagpieInstance();
  }
};
