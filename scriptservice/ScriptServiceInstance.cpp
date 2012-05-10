#include "stdafx.h"
#include "ScriptServiceInstance.h"

STDMETHODIMP CScriptServiceInstance::Init(const OLECHAR* extensionId, const OLECHAR* resourcesDir)
{
  if (!m_HiddenWindow.m_view.m_browserNavigated)
  {
    /**
     * We must ensure that no initialization is done before the hidden browser navigates to a page (about:blank).
     * Otherwise jQuery and other js code would fail because there would be no document.documentElement.
     */
    return S_FALSE;
  }

  m_ExtensionId = extensionId;

  HRESULT hr = m_Magpie.CoCreateInstance(CLSID_MagpieApplication);
  if (FAILED(hr))
  {
    return hr;
  }
  hr = m_Magpie->Init((LPWSTR)extensionId, (LPWSTR)resourcesDir);
  if (FAILED(hr))
  {
    return hr;
  }

  hr = m_Magpie->CreateSalsitaApi(-1, m_SalsitaApiServiceImpl);
  if (FAILED(hr))
  {
    return hr;
  }

  CComQIPtr<IHTMLWindow2> window = CComUtil::IWebBrowserToIHTMLWindow(m_HiddenWindow.m_view.m_pWebBrowser);
  hr = m_Magpie->ScriptAddNamedItem(L"window", window, SCRIPTITEM_ISSOURCE|SCRIPTITEM_ISVISIBLE|SCRIPTITEM_GLOBALMEMBERS);
  if (FAILED(hr))
  {
    return hr;
  }

  return S_OK;
}

HRESULT CScriptServiceInstance::FinalConstruct()
{
  m_TabIdCounter = 0;
  if (m_HiddenWindow.CreateEx() == NULL)
  {
    return E_FAIL;
  }

  HRESULT hr = CSalsitaApiServiceImpl::CreateObject(m_SalsitaApiServiceImpl.p);
  return hr;
}

void CScriptServiceInstance::FinalRelease()
{
  if (m_HiddenWindow)
  {
    m_HiddenWindow.DestroyWindow();
  }
}

STDMETHODIMP CScriptServiceInstance::GetApplication(LPUNKNOWN *pApplication)
{
  if (!pApplication)
  {
    return E_POINTER;
  }

  return m_Magpie.QueryInterface<IUnknown>(pApplication);
}

STDMETHODIMP CScriptServiceInstance::RegisterTab(INT *tabId)
{
  *tabId = ++ m_TabIdCounter;
  return S_OK;
}

STDMETHODIMP CScriptServiceInstance::GetSalsitaApiService(LPUNKNOWN *pService)
{
  if (!pService)
  {
    return E_POINTER;
  }

  return m_SalsitaApiServiceImpl.QueryInterface<IUnknown>(pService);
}
