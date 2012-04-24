#include "stdafx.h"
#include "SalsitaApiImpl.h"

CSalsitaApiImpl::CSalsitaApiImpl()
{
}

HRESULT CSalsitaApiImpl::CreateObject(CSalsitaApiImplComObject *& pRet, INT tabId, LPUNKNOWN pSalsitaApi)
{
  if (!pSalsitaApi)
  {
    return E_POINTER;
  }

  CSalsitaApiImplComObject *newObject = pRet = NULL;
  IF_FAILED_RET(CSalsitaApiImplComObject::CreateInstance(&newObject));
  newObject->AddRef();
  newObject->m_TabId = tabId;
  newObject->m_ApiService = pSalsitaApi;
  newObject->m_ApiService->connectClient(tabId);
  pRet = newObject;
  return S_OK;
}

HRESULT CSalsitaApiImpl::FinalConstruct()
{
  return S_OK;
}

void CSalsitaApiImpl::FinalRelease()
{
  if (m_ApiService)
  {
    m_ApiService->releaseClient(m_TabId);
  }
}

STDMETHODIMP CSalsitaApiImpl::getCurrentTabId(INT* tabId)
{
  if (!tabId)
  {
    return E_POINTER;
  }

  *tabId = m_TabId;
  return S_OK;
}

STDMETHODIMP CSalsitaApiImpl::addRequestListener(LPDISPATCH listener)
{
  if (!listener)
  {
    return S_OK; // silently return because nobody can call null
  }
  return m_ApiService->addRequestListener(m_TabId, listener);
}

STDMETHODIMP CSalsitaApiImpl::sendRequest(VARIANT request, LPDISPATCH requestCallback)
{
  return m_ApiService->sendRequest(m_TabId, -1, request, requestCallback);
}

STDMETHODIMP CSalsitaApiImpl::sendRequestToTab(INT tabId, VARIANT request, LPDISPATCH requestCallback)
{
  return m_ApiService->sendRequest(m_TabId, tabId, request, requestCallback);
}