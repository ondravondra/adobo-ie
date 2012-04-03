#include "stdafx.h"
#include "resource.h"

#include "HiddenBrowserView.h"
#include "MainFrm.h"

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//TODO: Replace with a URL of your choice or call m_pWebBrowser->Navigate(..) in CHiddenBrowserView::OnCreate
  m_hWndClient = m_view.Create(m_hWnd, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
	return 0;
}
