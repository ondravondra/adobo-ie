#include "stdafx.h"
#include "SalsitaApiImpl.h"

CSalsitaApiImpl::CSalsitaApiImpl()
{
}

HRESULT CSalsitaApiImpl::CreateObject(CSalsitaApiImplComObject *& pRet)
{
  CSalsitaApiImplComObject *newObject = pRet = NULL;
  IF_FAILED_RET(CSalsitaApiImplComObject::CreateInstance(&newObject));
  newObject->AddRef();
  pRet = newObject;
  return S_OK;
}

HRESULT CSalsitaApiImpl::FinalConstruct()
{
  return S_OK;
}

void CSalsitaApiImpl::FinalRelease()
{
}

STDMETHODIMP CSalsitaApiImpl::getCurrentTabId(INT* tabId)
{
  if (!tabId)
  {
    return E_POINTER;
  }

  *tabId = 1; // TODO: ...
  return S_OK;
}

STDMETHODIMP CSalsitaApiImpl::addRequestListener(LPDISPATCH listener)
{
  // TODO: ...
  return S_OK;
}

STDMETHODIMP CSalsitaApiImpl::sendRequest(VARIANT request, LPDISPATCH requestCallback)
{
  // TODO: ...
  return S_OK;
}

STDMETHODIMP CSalsitaApiImpl::sendRequestToTab(INT tabId, VARIANT request, LPDISPATCH requestCallback)
{
  // TODO: ...
  return S_OK;
}