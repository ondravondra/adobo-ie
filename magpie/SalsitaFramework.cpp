#include "stdafx.h"
#include "SalsitaFramework.h"
#include "CustomInternetSecurityImpl.h"
#include "MagpieActiveScript.h"

CSalsitaFramework::CSalsitaFramework(CMagpieActiveScript & magpieScript) : m_MagpieActiveScript(magpieScript)
{
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

  return m_MagpieActiveScript.AddNamedItem(globalName, pVal, SCRIPTITEM_ISSOURCE|SCRIPTITEM_ISVISIBLE);
}
