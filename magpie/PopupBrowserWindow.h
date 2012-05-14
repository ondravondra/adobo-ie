#pragma once

#include "libhtmlpanel/include/HtmlPanel.h"

class CPopupBrowser :
  public CHtmlPanel
{
public:
  CPopupBrowser();
  void Init(INT id, IDispatch *onReady);

protected:
  virtual void OnBrowserDocumentComplete(VARIANT *URL);

private:
  INT m_Id;
  CIDispatchHelper m_OnReady;
  bool m_OnReadyInvoked;
};
