#pragma once

#include <atlwin.h>
#include "HtmlPanel.h"

struct SalsitaToolbarCallback {
  virtual void ToolbarWindowReady(VARIANT *pURL) = 0; 
};

class CHtmlToolbarWindow : public CHtmlPanel
{
public:
  CHtmlToolbarWindow();

  SalsitaToolbarCallback *m_toolbarCallback;

protected:
  virtual void OnBrowserDocumentComplete(VARIANT *URL);
};
