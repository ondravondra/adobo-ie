#include "stdafx.h"
#include "ScriptServiceInstance.h"
#include "ScriptService.h"
#include "libcomhelper\libcomhelper.h"
using namespace LIB_COMHelper;

HRESULT CScriptServiceInstance::CreateObject(CScriptServiceCallback* pService, BSTR bsID, CScriptServiceInstanceComObject *& retVal)
{
  ATLASSERT(pService);
  CScriptServiceInstanceComObject* pObject = NULL;
  retVal = NULL;
  HRESULT hr = CScriptServiceInstanceComObject::CreateInstance(&pObject);
  if (FAILED(hr))
  {
    return hr;
  }
  hr = pObject->Init(pService, bsID);
  if (FAILED(hr))
  {
    delete pObject;
    return hr;
  }
  retVal = pObject;
  return S_OK;
}

HRESULT CScriptServiceInstance::Init(CScriptServiceCallback* pCallback, BSTR bsID)
{
  m_pScriptServiceCallback = pCallback;
  m_bsID = bsID;

  CRegKey regKey;
  CString s, sPath, sScript;
  s.Format(_T("Software\\Salsita\\ScriptingService\\%s"), bsID);
  if (ERROR_SUCCESS != regKey.Open(HKEY_CURRENT_USER, s))
  {
    return E_FAIL;
  }

  ULONG nChars = _MAX_PATH;
  if (ERROR_SUCCESS != regKey.QueryStringValue(_T("ScriptRoot"), sPath.GetBuffer(_MAX_PATH+1), &nChars))
  {
    sPath.ReleaseBuffer();
    return E_FAIL;
  }
  sPath.ReleaseBuffer();

  nChars = _MAX_PATH;
  if (ERROR_SUCCESS != regKey.QueryStringValue(_T("ScriptMain"), sScript.GetBuffer(_MAX_PATH+1), &nChars))
  {
    sScript.ReleaseBuffer();
    return E_FAIL;
  }
  sScript.ReleaseBuffer();
  m_MainModuleID = sScript;

  HRESULT hr = m_Magpie.CoCreateInstance(CLSID_MagpieApplication);
  if (FAILED(hr))
  {
    return hr;
  }
  hr = m_Magpie->Init((LPWSTR)(LPCWSTR)sPath);
  if (FAILED(hr))
  {
    return hr;
  }
  hr = m_Magpie->Run((LPWSTR)(LPCWSTR)sScript);
  if (FAILED(hr))
  {
    return hr;
  }

  return S_OK;
}

void CScriptServiceInstance::UnInit()
{
  m_pScriptServiceCallback = NULL;
}

HRESULT CScriptServiceInstance::FinalConstruct()
{
  m_pScriptServiceCallback = NULL;
  return S_OK;
}

void CScriptServiceInstance::FinalRelease()
{
  m_CallbackInterfaces.Release();
  if (m_pScriptServiceCallback)
  {
    m_pScriptServiceCallback->OnFinalRelease(m_bsID);
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

STDMETHODIMP CScriptServiceInstance::get_main(VARIANT * pRet)
{
  ATLASSERT(m_Magpie);
  if (!m_Magpie)
  {
    return E_UNEXPECTED;
  } else {
    return m_Magpie->FindExportsFor((LPWSTR)(LPCWSTR)m_MainModuleID, pRet);
  }
}
