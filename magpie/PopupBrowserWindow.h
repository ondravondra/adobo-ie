#pragma once

#include "resource.h"

class CPopupBrowserWindow;
typedef CFrameWindowImpl<CPopupBrowserWindow, CWindow, CWinTraits<WS_POPUP, WS_EX_TOOLWINDOW> > CPopupBrowserFrame;

class CPopupBrowser :
  public CWindowImpl<CPopupBrowser, CAxWindow>,
  public IDispEventImpl<1, CPopupBrowser, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>
{
public:
  CPopupBrowser(INT id, IDispatch *onReady);

  CComPtr<IWebBrowser2> m_pWebBrowser;
  DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

  BEGIN_MSG_MAP(CPopupBrowser)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  END_MSG_MAP()

  BEGIN_SINK_MAP(CPopupBrowser)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnBrowserDocumentComplete)
  END_SINK_MAP()

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

  STDMETHOD_(void, OnBrowserDocumentComplete)(IDispatch *pDisp, VARIANT *URL);

private:
  INT m_Id;
  CIDispatchHelper m_OnReady;
  bool m_OnReadyInvoked;
};

class CPopupBrowserWindow: public CPopupBrowserFrame
{
public:
  DECLARE_FRAME_WND_CLASS(NULL, IDS_POPUPWND)

  CPopupBrowserWindow(INT id, const OLECHAR *url, IDispatch *onReady);

  CPopupBrowser m_view;

  BEGIN_MSG_MAP(CPopupBrowserWindow)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    CHAIN_MSG_MAP(CPopupBrowserFrame)
  END_MSG_MAP()

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
  CString m_Url;
};
