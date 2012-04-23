#include "stdafx.h"
#include "ScriptServiceInstance.h"
#include "ScriptService.h"
#include "libcomhelper\libcomhelper.h"
using namespace LIB_COMHelper;

HRESULT CScriptServiceInstance::CreateObject(CScriptServiceCallback* pService, BSTR serviceIdentifier, BSTR resourcesDir, CScriptServiceInstanceComObject *& retVal)
{
  ATLASSERT(pService);
  CScriptServiceInstanceComObject* pObject = NULL;
  retVal = NULL;
  HRESULT hr = CScriptServiceInstanceComObject::CreateInstance(&pObject);
  if (FAILED(hr))
  {
    return hr;
  }
  hr = pObject->Init(pService, serviceIdentifier, resourcesDir);
  if (FAILED(hr))
  {
    delete pObject;
    return hr;
  }
  retVal = pObject;
  return S_OK;
}

HRESULT CScriptServiceInstance::Init(CScriptServiceCallback* pCallback, BSTR serviceIdentifier, BSTR resourcesDir)
{
  m_pScriptServiceCallback = pCallback;
  m_serviceIdentifier = serviceIdentifier;

  if(m_HiddenWindow.CreateEx() == NULL)
  {
    return E_FAIL;
  }

  HRESULT hr = m_Magpie.CoCreateInstance(CLSID_MagpieApplication);
  if (FAILED(hr))
  {
    return hr;
  }
  hr = m_Magpie->Init((LPWSTR)resourcesDir);
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
  return S_OK;
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
    m_pScriptServiceCallback->OnFinalRelease(m_serviceIdentifier);
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
  CIDispatchHelper window = CComUtil::IWebBrowserToIHTMLWindow(m_HiddenWindow.m_view.m_pWebBrowser);
  HRESULT hr = window.Get<IDispatch*, VT_DISPATCH>(memberName, pDisp.p);
  if (FAILED(hr))
  {
    return hr;
  } else {
    return m_Magpie->ScriptAddNamedItem(memberName, pDisp, SCRIPTITEM_ISSOURCE|SCRIPTITEM_ISVISIBLE);
  }
}
