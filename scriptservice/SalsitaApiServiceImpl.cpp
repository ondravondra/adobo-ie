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
  // Currently does nothing.
  return S_OK;
}

STDMETHODIMP CSalsitaApiServiceImpl::releaseClient(INT tabId)
{
  CAtlArray<RequestListenerT *> newListenerArray;
  for (size_t i = 0; i < m_listeners.GetCount(); i ++)
  {
    RequestListenerT *r = m_listeners.GetAt(i);
    if (r->tabId != tabId)
    {
      newListenerArray.Add(r);
    } else {
      delete r;
    }
  }
  m_listeners.RemoveAll();
  m_listeners.InsertArrayAt(0, &newListenerArray);
  return S_OK;
}

STDMETHODIMP CSalsitaApiServiceImpl::addRequestListener(INT tabId, LPDISPATCH listener)
{
  RequestListenerT *l = new RequestListenerT();
  l->tabId = tabId;
  l->listener = listener;
  m_listeners.Add(l);
  return S_OK;
}

STDMETHODIMP CSalsitaApiServiceImpl::sendRequest(INT senderTabId, INT recipientTabId, VARIANT senderObject, VARIANT request, VARIANT requestCallback)
{
  CComVariant varSenderObject = senderObject;
  CComVariant varRequest = request;
  CComVariant varCallback = requestCallback;
  CAtlArray<CIDispatchHelper *> recipients;
  
  for (size_t i = 0; i < m_listeners.GetCount(); i ++)
  {
    RequestListenerT *r = m_listeners.GetAt(i);
    if (recipientTabId == -1 || r->tabId == recipientTabId)
    {
      recipients.Add(new CIDispatchHelper(r->listener)); // increases refcount
    }
  }
  
  for (size_t i = 0; i < recipients.GetCount(); i ++)
  {
    CIDispatchHelper * r = recipients.GetAt(i);
    CComVariant varArgs[3] = { varCallback, varSenderObject, varRequest };
    r->InvokeN((DISPID)0, varArgs, 3);
    delete r; // release the listener
  }

  return S_OK;
}