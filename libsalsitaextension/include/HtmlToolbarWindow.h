#pragma once

#include <atlwin.h>

class CHtmlToolbarWindow : public CWindowImpl<CHtmlToolbarWindow, CAxWindow>
{
public:
  CComPtr<IWebBrowser2> m_pWebBrowser;
  DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

  BEGIN_MSG_MAP(CHtmlToolbarWindow)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  END_MSG_MAP()

  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
};
