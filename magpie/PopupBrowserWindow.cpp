#include "stdafx.h"
#include "PopupBrowserWindow.h"

CPopupBrowser::CPopupBrowser() : m_EventCallback(0)
{
}

void CPopupBrowser::Init(INT id, IDispatch *onReady, IDispatch *onDeactivated, PopupBrowserEventCallback *eventCallback)
{
  m_Id = id;
  m_OnReady = onReady;
  m_OnDeactivated = onDeactivated;
  m_OnReadyInvoked = false;
  m_EventCallback = eventCallback;
}

void CPopupBrowser::Dispose()
{
  m_EventCallback = NULL;
  m_OnReady = NULL;
}

void CPopupBrowser::InvokeOnDeactivated(INT *activatedId)
{
  if (!m_OnDeactivated)
  {
    return;
  }

  CComVariant varId(m_Id, VT_INT);
  if (activatedId)
  {
    CComVariant varActivatedId(*activatedId, VT_INT);
    CComVariant varArgs[2] = { varId, varActivatedId };
    m_OnDeactivated.InvokeN((DISPID)0, varArgs, _countof(varArgs));
  } else {
    CComVariant varArgs[1] = { varId };
    m_OnDeactivated.InvokeN((DISPID)0, varArgs, _countof(varArgs));
  }
}

LRESULT CPopupBrowser::OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  if (!m_OnDeactivated)
  {
    bHandled = FALSE;
    return 0;
  }

  if (LOWORD(wParam) == WA_INACTIVE)
  {
    INT activatedId;
    if (m_EventCallback && m_EventCallback->GetActivatedWindowPopupId((HWND)lParam, activatedId))
    {
      InvokeOnDeactivated(&activatedId);
    } else {
      InvokeOnDeactivated(NULL);
    }
  } else {
    bHandled = FALSE;
  }
  return 0;
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
