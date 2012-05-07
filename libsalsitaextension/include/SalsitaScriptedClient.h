#pragma once

#include <string>
#include "SalsitaExtensionHelper.h"

template <class Timpl>
class CSalsitaScriptedClient
{
protected:
  std::wstring m_ResourcesDir; ///< Directory holding web resources. This directory serves as top namespace using which all CommonJS module ids are resolved.

  CComPtr<IMagpieApplication> m_Magpie;

  CComPtr<IScriptService> m_ScriptService;
  CComPtr<IScriptServiceInstance> m_ScriptServiceInstance;
  bool m_BackgroundScriptStarted, m_BackgroundScriptFailed;

  INT m_TabId;

  CSalsitaScriptedClient() : m_BackgroundScriptStarted(false), m_BackgroundScriptFailed(false)
  {
  }

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
   * Retrieves path to the directory containing web resources and stores it in m_ResourcesDir.
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
  virtual bool ResolveResourcesDir() = 0;

  /**
   * Executes content scripts.
   * Gets called inside ReloadContentScriptOnNavigation once a magpie instance is created and initialized.
   */
  virtual HRESULT ExecuteContentScript() = 0;

  /**
   * Executes background script.
   * Called inside RunBackgroundScript.
   */
  virtual HRESULT ExecuteBackgroundScript() = 0;

  /**
   * Returns IHTMLWindow2 which will be passed to the content script as "window".
   * Can return S_FALSE to skip adding the window object.
   */
  virtual HRESULT GetWindowForScript(CComQIPtr<IHTMLWindow2> &window) = 0;

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

    hr = m_Magpie->CreateSalsitaApi(m_TabId, pApiSrvUnk);
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

    hr = m_ScriptService.CoCreateInstance(CLSID_ScriptService);
    if (FAILED(hr))
    {
      return hr;
    }

    CComPtr<IUnknown> instance;
    hr = m_ScriptService->GetServiceFor(CComBSTR(GetExtensionId()), CComBSTR(m_ResourcesDir.c_str()), &instance.p);
    if (FAILED(hr))
    {
      return hr;
    }

    hr = instance.QueryInterface<IScriptServiceInstance>(&m_ScriptServiceInstance.p);
    if (FAILED(hr))
    {
      return hr;
    }

    hr = ExecuteBackgroundScript();
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