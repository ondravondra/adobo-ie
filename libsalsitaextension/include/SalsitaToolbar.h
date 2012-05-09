#pragma once

#include "SalsitaToolbarBrowserCustomization.h"
#include "SalsitaScriptedClient.h"
#include "HtmlToolbarWindow.h"

template <class Timpl, const CLSID* pclsid>
class ATL_NO_VTABLE CSalsitaToolbar :
  public CSalsitaScriptedClient<Timpl>,
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<Timpl, pclsid>,
  public IObjectWithSiteImpl<Timpl>,
  public IDeskBand,
  public IInputObject,
  public IDispEventImpl<1, Timpl, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>,
  public CSalsitaToolbarBrowserCustomization<Timpl>
{
public:
  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(Timpl)
    COM_INTERFACE_ENTRY(IObjectWithSite)
    COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
    COM_INTERFACE_ENTRY(IOleWindow)
    COM_INTERFACE_ENTRY_IID(IID_IDockingWindow, IDockingWindow)
    COM_INTERFACE_ENTRY(IInputObject)
    COM_INTERFACE_ENTRY(IDocHostUIHandlerDispatch)
  END_COM_MAP()

  BEGIN_SINK_MAP(Timpl)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2)
  END_SINK_MAP()

private:
  HWND m_hWndParent;
  CHtmlToolbarWindow m_HtmlWindow;
  DWORD m_WebBrowserEventsCookie;
  std::wstring m_PageUrl;

protected:
  CSalsitaToolbar() : m_dwBandID(0), m_dwViewMode(0), m_hWndParent(NULL), m_WebBrowserEventsCookie(0)
  {
  }

  DWORD m_dwBandID, m_dwViewMode;
  /**
   * Fills in values for IDeskBand::GetBandInfo.
   */
  virtual void GetBandInfoValues(const wchar_t *& title, POINTL &minSize) = 0;

  /**
   * Retrieves url for the toolbar html page.
   * An url to local file (in the resources directory) can be constructed using CSalsitaExtensionHelper::ResourcesDirMakeUrl helper function.
   */
  virtual void GetToolbarPage(std::wstring &pageUrl) = 0;

protected:
  // CSalsitaScriptedClient
  virtual HRESULT GetWindowForScript(CComQIPtr<IHTMLWindow2> &window)
  {
    window = CComUtil::IWebBrowserToIHTMLWindow(m_HtmlWindow.m_pWebBrowser);
    if (!window)
    {
      return E_FAIL;
    } else {
      return S_OK;
    }
  }

public:
  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
  }

  // IObjectWithSite
public:
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

private:

  HRESULT RunToolbarPage()
  {
    GetToolbarPage(m_PageUrl);

    m_HtmlWindow.Create(m_hWndParent, CWindow::rcDefault, L"about:blank",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);

    if (!m_HtmlWindow.m_pWebBrowser)
    {
      return E_FAIL;
    }

    m_HtmlWindow.SetExternalUIHandler((IDocHostUIHandlerDispatch *)this);

    AtlAdvise(m_HtmlWindow.m_pWebBrowser,
      (IUnknown*)(IDispEventImpl<1, Timpl, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>*)this,
      DIID_DWebBrowserEvents2, &m_WebBrowserEventsCookie);

    return m_HtmlWindow.m_pWebBrowser->Navigate(CComBSTR(m_PageUrl.c_str()), NULL, NULL, NULL, NULL);
  }

  STDMETHOD_(void, OnNavigateComplete2)(IDispatch *pDisp, VARIANT *URL)
  {
    CComQIPtr<IWebBrowser2> caller = pDisp;
    if (caller && caller.IsEqualObject(m_HtmlWindow.m_pWebBrowser))
    {
      RunBackgroundScript();
      ReloadContentScript();
    }
  }

protected:

  virtual HRESULT InternalSetSite()
  {
    if (!ResolveResourcesDir(m_ResourcesDir))
    {
      return E_FAIL;
    }

    // Get the parent window.
    IOleWindow *pOleWindow = NULL;
    m_hWndParent = NULL;

    if (SUCCEEDED(m_spUnkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&pOleWindow)))
    {
      pOleWindow->GetWindow(&m_hWndParent);
      pOleWindow->Release();
    }

    if (!::IsWindow(m_hWndParent))
    {
      return E_FAIL;
    }

    return RunToolbarPage();
  }

  virtual HRESULT InternalReleaseSite()
  {
    if (m_HtmlWindow.IsWindow())
    {
      m_HtmlWindow.DestroyWindow();
    }

    CleanupScriptedClient();

    return S_OK;
  }

  // IDeskBand
public:
  STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi)
  {
    if (pdbi)
    {
      m_dwBandID = dwBandID;
      m_dwViewMode = dwViewMode;

      const wchar_t * title;
      POINTL minSize;
      GetBandInfoValues(title, minSize);

      if (pdbi->dwMask & DBIM_MINSIZE)
      {
        pdbi->ptMinSize.x = minSize.x;
        pdbi->ptMinSize.y = minSize.y;
      }

      if (pdbi->dwMask & DBIM_MAXSIZE)
      {
        pdbi->ptMaxSize.x = -1;
        pdbi->ptMaxSize.y = -1;
      }

      if (pdbi->dwMask & DBIM_INTEGRAL)
      {
        pdbi->ptIntegral.x = 1;
        pdbi->ptIntegral.y = 1;
      }

      if (pdbi->dwMask & DBIM_ACTUAL)
      {
        pdbi->ptActual.x = 0;
        pdbi->ptActual.y = 0;
      }

      if (pdbi->dwMask & DBIM_TITLE)
      {
        wcscpy_s(pdbi->wszTitle, _countof(pdbi->wszTitle), title);
      }

      if (pdbi->dwMask & DBIM_MODEFLAGS)
      {
        pdbi->dwModeFlags = DBIMF_VARIABLEHEIGHT;
      }

      if (pdbi->dwMask & DBIM_BKCOLOR)
      {
        pdbi->dwMask &= ~DBIM_BKCOLOR;
      }

      return S_OK;
    }

    return E_INVALIDARG;
  }

  // IOleWindow
public:
  STDMETHOD(GetWindow)(HWND* phwnd)
  {
    if (!phwnd)
    {
      return E_POINTER;
    }

    *phwnd = m_HtmlWindow.m_hWnd;
    return S_OK;
  }

  STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode)
  {
    return S_OK;
  }

  // IDockingWindow
public:
  STDMETHOD(CloseDW)(unsigned long dwReserved)
  {
    return ShowDW(FALSE);
  }

  STDMETHOD(ResizeBorderDW)(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(ShowDW)(BOOL fShow)
  {
    m_HtmlWindow.ShowWindow(fShow ? SW_SHOW : SW_HIDE);
    return S_OK;
  }

  // IInputObject
public:
  STDMETHOD(HasFocusIO)(void)
  {
    return m_HtmlWindow.m_hWnd == ::GetFocus() ? S_OK : S_FALSE;
  }

  STDMETHOD(TranslateAcceleratorIO)(LPMSG lpMsg)
  {
    return S_FALSE;
  }

  STDMETHOD(UIActivateIO)(BOOL fActivate, LPMSG lpMsg)
  {
    if (fActivate)
    {
      m_HtmlWindow.SetFocus();
    }
    return S_OK;
  }
};
