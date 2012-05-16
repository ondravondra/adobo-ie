#include "stdafx.h"
#include "SalsitaExtensionEventHandler.h"

CSalsitaExtensionEventHandler::CSalsitaExtensionEventHandler() : m_extensionCallback(NULL)
{
}

STDMETHODIMP_(void) CSalsitaExtensionEventHandler::OnBrowserBeforeNavigate2(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel)
{
  if (m_extensionCallback)
  {
    m_extensionCallback->ReleaseContentScriptBeforeNavigation(pDisp);
  }
}

STDMETHODIMP_(void) CSalsitaExtensionEventHandler::OnBrowserDocumentComplete(LPDISPATCH pDisp, VARIANT *pURL)
{
  if (m_extensionCallback)
  {
    m_extensionCallback->ReleaseContentScriptBeforeNavigation(pDisp);
  }
}

STDMETHODIMP_(void) CSalsitaExtensionEventHandler::OnBrowserWindowStateChanged(ULONG dwFlags, ULONG dwValidFlagsMask)
{
  if (m_extensionCallback)
  {
    m_extensionCallback->ProcessBrowserWindowStateChanged(dwFlags, dwValidFlagsMask);
  }
}
