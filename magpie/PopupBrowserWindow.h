#pragma once

#include "resource.h"

class CPopupBrowser;
typedef CWindowImpl<CPopupBrowser, CAxWindow> CPopupBrowserAxWindow;

class CPopupBrowser :
  public CPopupBrowserAxWindow,
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
