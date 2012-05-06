#pragma once

class CHiddenBrowserView :
  public CWindowImpl<CHiddenBrowserView, CAxWindow>,
  public IDispEventImpl<1, CHiddenBrowserView, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>
{
public:
  CHiddenBrowserView();

  CComPtr<IWebBrowser2> m_pWebBrowser;
  DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

  BEGIN_MSG_MAP(CHiddenBrowserView)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  END_MSG_MAP()

  BEGIN_SINK_MAP(CHiddenBrowserView)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnBrowserNavigateComplete2)
  END_SINK_MAP()

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

  STDMETHOD_(void, OnBrowserNavigateComplete2)(IDispatch *pDisp, VARIANT *URL);

  bool m_browserNavigated;
};
