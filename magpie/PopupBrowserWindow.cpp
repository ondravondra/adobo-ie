#include "stdafx.h"
#include "PopupBrowserWindow.h"

CPopupBrowser::CPopupBrowser(INT id, IDispatch *onReady)
{
  m_Id = id;
  m_OnReady = onReady;
  m_OnReadyInvoked = false;
}

LRESULT CPopupBrowser::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

LRESULT CPopupBrowser::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  if (m_pWebBrowser)
  {
    DispEventUnadvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
    m_pWebBrowser.Release();
  }
  return 0;
}

STDMETHODIMP_(void) CPopupBrowser::OnBrowserDocumentComplete(IDispatch *pDisp, VARIANT *URL)
{
  CComQIPtr<IWebBrowser2> caller = pDisp;
  if (caller && caller.IsEqualObject(m_pWebBrowser))
  {
    CComQIPtr<IHTMLWindow2> wnd = CComUtil::IWebBrowserToIHTMLWindow(m_pWebBrowser);
    if (m_OnReady && wnd && (!m_OnReadyInvoked))
    {
      CComVariant varId(m_Id, VT_INT);
      CComVariant varWnd(wnd);
      CComVariant varArgs[2] = { varWnd, varId };
      m_OnReady.InvokeN((DISPID)0, varArgs, _countof(varArgs));
      m_OnReadyInvoked = true;
    }
  }
}
