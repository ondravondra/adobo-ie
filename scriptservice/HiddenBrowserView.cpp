#include "stdafx.h"
#include "resource.h"
#include "HiddenBrowserView.h"

CHiddenBrowserView::CHiddenBrowserView() : m_browserNavigated(false)
{
}

LRESULT CHiddenBrowserView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  DefWindowProc();

  CComPtr<IAxWinHostWindow> spHost;
  HRESULT hr = QueryHost(__uuidof(IAxWinHostWindow), (void**)&spHost);
  if (FAILED(hr))
    return -1;

  hr = spHost->QueryControl(IID_IWebBrowser2, (void**)&m_pWebBrowser.p);
  if (FAILED(hr))
    return -1;

  hr = DispEventAdvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
  if (FAILED(hr))
    return -1;

  return 0;
}

LRESULT CHiddenBrowserView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  m_pWebBrowser.Release();
  return 0;
}

STDMETHODIMP_(void) CHiddenBrowserView::OnBrowserNavigateComplete2(IDispatch *pDisp, VARIANT *URL)
{
  CComQIPtr<IWebBrowser2> caller = pDisp;
  if (caller && caller.IsEqualObject(m_pWebBrowser))
  {
    m_browserNavigated = true;
  }
}
