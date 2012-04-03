#include "stdafx.h"
#include "resource.h"

#include "HiddenBrowserView.h"

LRESULT CHiddenBrowserView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DefWindowProc();

  CComPtr<IAxWinHostWindow> spHost;
  HRESULT hr = QueryHost(__uuidof(IAxWinHostWindow), (void**)&spHost);
  if (FAILED(hr))
    return -1;

  spHost->QueryControl(IID_IWebBrowser2, (void**)&m_pWebBrowser.p);
	return 0;
}
LRESULT CHiddenBrowserView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  m_pWebBrowser.Release();
	return 0;
}
