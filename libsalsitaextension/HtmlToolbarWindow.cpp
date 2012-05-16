#include "stdafx.h"
#include "HtmlToolbarWindow.h"

CHtmlToolbarWindow::CHtmlToolbarWindow() : m_toolbarCallback(NULL)
{
}

void CHtmlToolbarWindow::OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel)
{
  if (m_toolbarCallback)
  {
    m_toolbarCallback->ToolbarWindowBeforeNavigate();
  }
}

void CHtmlToolbarWindow::OnBrowserDocumentComplete(VARIANT *URL)
{
  if (m_toolbarCallback)
  {
    m_toolbarCallback->ToolbarWindowReady(URL);
  }
}
