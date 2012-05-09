#include "stdafx.h"
#include "SalsitaFramework.h"
#include "CustomInternetSecurityImpl.h"
#include "MagpieActiveScript.h"

CSalsitaFramework::CSalsitaFramework() : m_MagpieActiveScript(NULL)
{
}

HRESULT CSalsitaFramework::CreateObject(CSalsitaFrameworkComObject *& pRet, CMagpieActiveScript *magpieActiveScript)
{
  if (!magpieActiveScript)
  {
    return E_INVALIDARG;
  }

  CSalsitaFrameworkComObject *newObject = pRet = NULL;
  IF_FAILED_RET(CSalsitaFrameworkComObject::CreateInstance(&newObject));
  newObject->AddRef();
  newObject->m_MagpieActiveScript = magpieActiveScript;
  pRet = newObject;
  return S_OK;
}

void CSalsitaFramework::Shutdown()
{
  m_MagpieActiveScript = NULL;
}

HRESULT CSalsitaFramework::FinalConstruct()
{
  return S_OK;
}

void CSalsitaFramework::FinalRelease()
{
}

/**
 * Add CustomInternetSecurity to XMLHTTPRequest to allow cross-domain requests
 */
HRESULT CSalsitaFramework::AddCustomInternetSecurity(CComPtr<IXMLHttpRequest> pRequest)
{
  CComObject<CCustomInternetSecurityImpl>* pSecurityImpl = NULL;
  CComPtr<IUnknown> pUnkSecurity;
  CComPtr<IObjectWithSite> pObjWithSite;

  IF_FAILED_RET(CComObject<CCustomInternetSecurityImpl>::CreateInstance(&pSecurityImpl));
  IF_FAILED_RET(pSecurityImpl->QueryInterface(&pUnkSecurity));
  IF_FAILED_RET(pRequest->QueryInterface(&pObjWithSite));
  IF_FAILED_RET(pObjWithSite->SetSite(pUnkSecurity));

  return S_OK;
}

STDMETHODIMP CSalsitaFramework::createXMLHTTPRequest(IDispatch** ppVal)
{
  ENSURE_RETVAL(ppVal);

  CComPtr<IXMLHttpRequest> pRequest;
  IF_FAILED_RET(pRequest.CoCreateInstance(__uuidof(XMLHTTPRequest)));

  // try to add the security object
  HRESULT hrAddSecurity = AddCustomInternetSecurity(pRequest);

  IF_FAILED_RET(pRequest->QueryInterface(IID_IDispatch, (void**)ppVal));

  if (FAILED(hrAddSecurity)) // if we fail to add the security object, return the XMLHTTPRequest anyway
  {
    return S_FALSE;
  } else {
    return S_OK;
  }
}

STDMETHODIMP CSalsitaFramework::makeGlobalSymbol(LPDISPATCH pVal, BSTR globalName)
{
  if (!pVal)
  {
    return E_INVALIDARG;
  }

  if (!m_MagpieActiveScript)
  {
    return S_FALSE; // don't cause script to throw an exception, it is going to be terminated anyway because shutdown was called
  } else {
    return m_MagpieActiveScript->AddNamedItem(globalName, pVal, SCRIPTITEM_ISSOURCE|SCRIPTITEM_ISVISIBLE);
  }
}
