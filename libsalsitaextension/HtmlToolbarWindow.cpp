#include "stdafx.h"
#include "HtmlToolbarWindow.h"

CHtmlToolbarWindow::CHtmlToolbarWindow() : m_toolbarCallback(NULL)
{
}

void CHtmlToolbarWindow::OnBrowserDocumentComplete(VARIANT *URL)
{
  if (m_toolbarCallback)
  {
    m_toolbarCallback->ToolbarWindowReady(URL);
  }
}
