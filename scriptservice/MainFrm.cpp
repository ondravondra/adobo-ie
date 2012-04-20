#include "stdafx.h"
#include "resource.h"

#include "HiddenBrowserView.h"
#include "MainFrm.h"

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  LPCTSTR url = m_initialUrl.IsEmpty() ? _T("about:blank") : (LPCTSTR)m_initialUrl;
  //TODO: Replace with a URL of your choice or call m_pWebBrowser->Navigate(..) in CHiddenBrowserView::OnCreate
  m_hWndClient = m_view.Create(m_hWnd, rcDefault, url, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
  return 0;
}
