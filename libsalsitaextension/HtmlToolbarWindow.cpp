#include "stdafx.h"
#include "HtmlToolbarWindow.h"

LRESULT CHtmlToolbarWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  DefWindowProc();

  CComPtr<IAxWinHostWindow> spHost;
  HRESULT hr = QueryHost(__uuidof(IAxWinHostWindow), (void**)&spHost);
  if (FAILED(hr))
  {
    return -1;
  }

  spHost->QueryControl(IID_IWebBrowser2, (void**)&m_pWebBrowser.p);
  if (m_pWebBrowser)
  {
    m_pWebBrowser->put_RegisterAsDropTarget(VARIANT_FALSE); ///< disable drag and drop to the page
  }

  return 0;
}

LRESULT CHtmlToolbarWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  m_pWebBrowser.Release();
  return 0;
}
