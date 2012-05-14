#include "stdafx.h"
#include "PopupBrowserWindow.h"

CPopupBrowser::CPopupBrowser(INT id, IDispatch *onReady)
{
  m_Id = id;
  m_OnReady = onReady;
  m_OnReadyInvoked = false;
}

void CPopupBrowser::OnBrowserDocumentComplete(VARIANT *URL)
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
