#pragma once

#include <atlwin.h>
#include "HtmlPanel.h"

struct SalsitaToolbarCallback {
  virtual void ToolbarWindowBeforeNavigate() = 0;
  virtual void ToolbarWindowReady(VARIANT *pURL) = 0;
};

class CHtmlToolbarWindow : public CHtmlPanel
{
public:
  CHtmlToolbarWindow();

  SalsitaToolbarCallback *m_toolbarCallback;

protected:
  virtual void OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);
  virtual void OnBrowserDocumentComplete(VARIANT *URL);
};
