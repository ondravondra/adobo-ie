#pragma once

#include "HtmlPanelUIHandler.h"

class CHtmlPanel;
typedef CWindowImpl<CHtmlPanel, CAxWindow> CHtmlPanelAxWindow;

class CHtmlPanel :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CHtmlPanel>,
  public CHtmlPanelAxWindow,
  public IDispEventImpl<1, CHtmlPanel, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>,
  public CHtmlPanelUIHandler<CHtmlPanel>
{
public:
  CComPtr<IWebBrowser2> m_pWebBrowser;
  DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

  BEGIN_MSG_MAP(CHtmlPanel)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  END_MSG_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_COM_MAP(CHtmlPanel)
    COM_INTERFACE_ENTRY(IDocHostUIHandlerDispatch)
  END_COM_MAP()

  BEGIN_SINK_MAP(CHtmlPanel)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, BrowserBeforeNavigate2Event)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, BrowserDocumentCompleteEvent)
  END_SINK_MAP()

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

  STDMETHOD_(void, BrowserBeforeNavigate2Event)(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);
  STDMETHOD_(void, BrowserDocumentCompleteEvent)(IDispatch *pDisp, VARIANT *URL);

protected:
  CHtmlPanel();

  virtual void OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);
  virtual void OnBrowserDocumentComplete(VARIANT *URL);
};
