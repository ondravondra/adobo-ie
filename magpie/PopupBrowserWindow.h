#pragma once

#include "libhtmlpanel/include/HtmlPanel.h"

struct PopupBrowserEventCallback {
  virtual bool GetActivatedWindowPopupId(HWND activatedWindow, INT &popupId) = 0;
};

class CPopupBrowser :
  public CHtmlPanel
{
public:
  CPopupBrowser();
  void Init(INT id, IDispatch *onReady, IDispatch *onDeactivated, PopupBrowserEventCallback *eventCallback);
  void Dispose();

  virtual LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

protected:
  virtual void OnBrowserDocumentComplete(VARIANT *URL);

private:
  INT m_Id;
  CIDispatchHelper m_OnReady, m_OnDeactivated;
  bool m_OnReadyInvoked;
  PopupBrowserEventCallback *m_EventCallback;

  void InvokeOnDeactivated(INT *activatedId);
};
