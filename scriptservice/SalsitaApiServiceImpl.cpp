#include "stdafx.h"
#include "SalsitaApiServiceImpl.h"

HRESULT CSalsitaApiServiceImpl::CreateObject(CSalsitaApiServiceImplComObject *& pRet)
{
  CSalsitaApiServiceImplComObject *newObject = pRet = NULL;
  HRESULT hr = CSalsitaApiServiceImplComObject::CreateInstance(&newObject);
  if (FAILED(hr))
  {
    return hr;
  }
  newObject->AddRef();
  pRet = newObject;
  return S_OK;
}

HRESULT CSalsitaApiServiceImpl::FinalConstruct()
{
  return S_OK;
}

void CSalsitaApiServiceImpl::FinalRelease()
{
}

STDMETHODIMP CSalsitaApiServiceImpl::connectClient(INT tabId)
{
  return S_OK;
}

STDMETHODIMP CSalsitaApiServiceImpl::releaseClient(INT tabId)
{
  return S_OK;
}

STDMETHODIMP CSalsitaApiServiceImpl::addRequestListener(INT tabId, LPDISPATCH listener)
{
  return S_OK;
}

STDMETHODIMP CSalsitaApiServiceImpl::sendRequest(INT senderTabId, INT recipientTabId, VARIANT request, LPDISPATCH requestCallback)
{
  return S_OK;
}