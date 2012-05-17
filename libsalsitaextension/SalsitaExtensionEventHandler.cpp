#include "stdafx.h"
#include "SalsitaExtensionEventHandler.h"

CSalsitaExtensionEventHandler::CSalsitaExtensionEventHandler() : m_extensionCallback(NULL), m_eventsCookie(0)
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
    m_extensionCallback->ReloadScriptsOnNavigation(pDisp);
  }
}

STDMETHODIMP_(void) CSalsitaExtensionEventHandler::OnBrowserWindowStateChanged(ULONG dwFlags, ULONG dwValidFlagsMask)
{
  if (m_extensionCallback)
  {
    m_extensionCallback->ProcessBrowserWindowStateChanged(dwFlags, dwValidFlagsMask);
  }
}

void CSalsitaExtensionEventHandler::AdviseBrowser(IUnknown *browser)
{
  ATLASSERT(!m_eventsCookie);
  AtlAdvise(browser, (IUnknown*)(IDispEventImpl<1, CSalsitaExtensionEventHandler, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>*)this, DIID_DWebBrowserEvents2, &m_eventsCookie);
}

void CSalsitaExtensionEventHandler::UnadviseBrowser(IUnknown *browser)
{
  if (m_eventsCookie)
  {
    AtlUnadvise(browser, DIID_DWebBrowserEvents2, m_eventsCookie);
    m_eventsCookie = 0;
  }
}
