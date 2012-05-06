#include "stdafx.h"
#include "ScriptServiceInstance.h"
using namespace LIB_COMHelper;

STDMETHODIMP CScriptServiceInstance::Init(const OLECHAR* extensionId, const OLECHAR* resourcesDir)
{
  m_ExtensionId = extensionId;

  if(m_HiddenWindow.CreateEx() == NULL)
  {
    return E_FAIL;
  }

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

STDMETHODIMP CScriptServiceInstance::LoadModule(const OLECHAR* moduleID)
{
  ATLASSERT(m_Magpie);
  if (!m_Magpie)
  {
    return E_UNEXPECTED;
  } else {
    return m_Magpie->Run((LPWSTR)moduleID);
  }
}

STDMETHODIMP CScriptServiceInstance::ScriptCreateNamedItem(const OLECHAR *name, GUID clsId, ULONG dwFlags)
{
  ATLASSERT(m_Magpie);
  if (!m_Magpie)
  {
    return E_UNEXPECTED;
  }

  CComPtr<IDispatch> pDisp;
  HRESULT hr = pDisp.CoCreateInstance(clsId);
  if (FAILED(hr))
  {
    return hr;
  } else {
    return m_Magpie->ScriptAddNamedItem((LPWSTR)name, pDisp, dwFlags);
  }
}