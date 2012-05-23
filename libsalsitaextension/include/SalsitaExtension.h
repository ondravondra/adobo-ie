#pragma once

#include <string>
#include "SalsitaScriptedClient.h"
#include "SalsitaExtensionEventHandler.h"

template <class Timpl>
class CSalsitaExtension :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CSalsitaScriptedClientWithEvents<Timpl>,
  public SalsitaExtensionEventCallback,
  public IObjectWithSiteImpl<Timpl>
{
protected:
  CComPtr<IWebBrowser2> m_WebBrowser;
  CComObject<CSalsitaExtensionEventHandler> *m_EventHandler;
  bool m_RaiseTabActivatedWhenMagpieIsAvailable;

  CSalsitaExtension() : m_RaiseTabActivatedWhenMagpieIsAvailable(false)
  {
  }

  virtual HRESULT GetWindowForScript(CComQIPtr<IHTMLWindow2> &window)
  {
    window = CComUtil::IWebBrowserToIHTMLWindow(m_WebBrowser);
    if (!window)
    {
      return E_FAIL;
    } else {
      return S_OK;
    }
  }

  virtual HRESULT InternalSetSite()
  {
    ATLASSERT(m_spUnkSite);

    if (!ResolveResourcesDir(m_ResourcesDir))
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

    m_EventHandler->AdviseBrowser(m_WebBrowser);

    return S_OK;
  }

  virtual HRESULT InternalReleaseSite()
  {
    if (m_WebBrowser)
    {
      m_EventHandler->UnadviseBrowser(m_WebBrowser);
      m_WebBrowser.Release();
    }

    if (m_Magpie)
    {
      m_Magpie->RaiseTabEvent(TAB_REMOVED);
    }

    CleanupScriptedClient();

    return S_OK;
  }

  virtual void GetBrowserSite(CComPtr<IUnknown> &site)
  {
    site = m_spUnkSite;
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

  virtual void ReleaseContentScriptBeforeNavigation(IDispatch *pWebBrowser)
  {
    if (!IsFrame(pWebBrowser))
    {
      DestroyMagpieInstance();
    }
  }

  virtual void ReloadScriptsOnNavigation(IDispatch *pWebBrowser)
  {
    HRESULT hr;
    hr = ConnectToBackgroundScript();
    ATLASSERT(SUCCEEDED(hr));

    if (!IsFrame(pWebBrowser))
    {
      hr = ReloadContentScript();
      ATLASSERT(SUCCEEDED(hr));

      if (m_Magpie && m_RaiseTabActivatedWhenMagpieIsAvailable)
      {
        m_RaiseTabActivatedWhenMagpieIsAvailable = false;
        m_Magpie->RaiseTabEvent(TAB_ACTIVATED);
      }
    }
  }

  void ProcessBrowserWindowStateChanged(ULONG dwFlags, ULONG dwValidFlagsMask)
  {
    if ((dwValidFlagsMask & OLECMDIDF_WINDOWSTATE_USERVISIBLE) && (dwFlags & OLECMDIDF_WINDOWSTATE_USERVISIBLE))
    {
      if (m_Magpie)
      {
        m_RaiseTabActivatedWhenMagpieIsAvailable = false;
        m_Magpie->RaiseTabEvent(TAB_ACTIVATED);
      } else {
        m_RaiseTabActivatedWhenMagpieIsAvailable = true;
      }
    }
  }

public:
  HRESULT FinalConstruct()
  {
    HRESULT hr = CComObject<CSalsitaExtensionEventHandler>::CreateInstance(&m_EventHandler);
    if (FAILED(hr))
    {
      return hr;
    }
    m_EventHandler->AddRef();
    m_EventHandler->m_extensionCallback = dynamic_cast<SalsitaExtensionEventCallback *>(this);
    return S_OK;
  }

  void FinalRelease()
  {
    if (m_EventHandler)
    {
      m_EventHandler->m_extensionCallback = NULL;
      m_EventHandler->Release();
      m_EventHandler = NULL;
    }

    ATLASSERT(!m_spUnkSite);
    ATLASSERT(!m_WebBrowser);
  }

  // IObjectWithSite
  STDMETHODIMP SetSite(IUnknown *pUnkSite)
  {
    m_spUnkSite = pUnkSite;

    if (m_spUnkSite)
    {
      HRESULT hr = InternalSetSite();
      ATLASSERT(hr == S_OK);
    }
    else
    {
      HRESULT hr = InternalReleaseSite();
      ATLASSERT(hr == S_OK);
    }
    return S_OK;
  }
};
