#pragma once

#include <string>
#include "SalsitaScriptedClient.h"

template <class Timpl>
class CSalsitaExtension :
  public CSalsitaScriptedClientWithEvents<Timpl>,
  public IObjectWithSiteImpl<Timpl>,
  public IDispEventImpl<1, Timpl, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>
{
protected:
  DWORD m_WebBrowserEventsCookie;
  CComPtr<IWebBrowser2> m_WebBrowser;

  CSalsitaExtension() :
    m_WebBrowserEventsCookie(0)
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

  HRESULT ReloadContentScriptOnNavigation(IDispatch *pWebBrowser)
  {
    if (IsFrame(pWebBrowser))
    {
      return S_FALSE;
    }

    return ReloadContentScript();
  }

  // invoked from BrowserWindowStateChanged event handler
  // TODO: handle this automatically
  void ProcessBrowserWindowStateChanged(ULONG dwFlags, ULONG dwValidFlagsMask)
  {
    if (m_Magpie && (dwValidFlagsMask & OLECMDIDF_WINDOWSTATE_USERVISIBLE) && (dwFlags & OLECMDIDF_WINDOWSTATE_USERVISIBLE))
    {
      m_Magpie->RaiseTabEvent(TAB_ACTIVATED);
    }
  }

public:
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

