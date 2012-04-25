#include "stdafx.h"
#include "ScriptServiceInstance.h"
#include "ScriptService.h"
#include "libcomhelper\libcomhelper.h"
using namespace LIB_COMHelper;

HRESULT CScriptServiceInstance::CreateObject(CScriptServiceCallback* pService, BSTR extensionId, BSTR resourcesDir, CScriptServiceInstanceComObject *& retVal)
{
  ATLASSERT(pService);
  CScriptServiceInstanceComObject* pObject = NULL;
  retVal = NULL;
  HRESULT hr = CScriptServiceInstanceComObject::CreateInstance(&pObject);
  if (FAILED(hr))
  {
    return hr;
  }
  hr = pObject->Init(pService, extensionId, resourcesDir);
  if (FAILED(hr))
  {
    delete pObject;
    return hr;
  }
  retVal = pObject;
  return S_OK;
}

HRESULT CScriptServiceInstance::Init(CScriptServiceCallback* pCallback, BSTR extensionId, BSTR resourcesDir)
{
  m_pScriptServiceCallback = pCallback;
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

void CScriptServiceInstance::UnInit()
{
  if (m_HiddenWindow)
  {
    m_HiddenWindow.DestroyWindow();
  }
  m_pScriptServiceCallback = NULL;
}

HRESULT CScriptServiceInstance::FinalConstruct()
{
  m_pScriptServiceCallback = NULL;
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
  m_CallbackInterfaces.Release();
  if (m_pScriptServiceCallback)
  {
    m_pScriptServiceCallback->OnFinalRelease(m_ExtensionId);
  }
}

STDMETHODIMP CScriptServiceInstance::SetCallback(LPUNKNOWN pUnk)
{
  return m_CallbackInterfaces.Set(pUnk);
}

STDMETHODIMP CScriptServiceInstance::ReleaseCallback()
{
  return m_CallbackInterfaces.Release();
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

STDMETHODIMP CScriptServiceInstance::LoadModule(BSTR moduleID)
{
  ATLASSERT(m_Magpie);
  if (!m_Magpie)
  {
    return E_UNEXPECTED;
  } else {
    return m_Magpie->Run((LPWSTR)moduleID);
  }
}

HRESULT CScriptServiceInstance::MakeJsWindowMemberGlobal(BSTR memberName)
{
  ATLASSERT(m_Magpie);
  if (!m_Magpie)
  {
    return E_UNEXPECTED;
  }

  CComPtr<IDispatch> pDisp;
  HRESULT hr = CComUtil::ExtractWindowMember(m_HiddenWindow.m_view.m_pWebBrowser, memberName, pDisp);
  if (FAILED(hr))
  {
    return hr;
  } else {
    return m_Magpie->ScriptAddNamedItem(memberName, pDisp, SCRIPTITEM_ISSOURCE|SCRIPTITEM_ISVISIBLE);
  }
}
