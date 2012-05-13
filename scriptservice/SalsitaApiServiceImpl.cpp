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

void CSalsitaApiServiceImpl::RemoveListenersForTab(INT tabId, CSalsitaApiServiceImpl::EventListenerArrayT *listenerArray)
{
  EventListenerArrayT newListenerArray;
  for (size_t i = 0; i < listenerArray->GetCount(); i ++)
  {
    EventListenerT *l = listenerArray->GetAt(i);
    if (l->tabId != tabId)
    {
      newListenerArray.Add(l);
    } else {
      delete l;
    }
  }
  listenerArray->RemoveAll();
  listenerArray->InsertArrayAt(0, &newListenerArray);
}

STDMETHODIMP CSalsitaApiServiceImpl::releaseClient(INT tabId)
{
  POSITION p = m_eventListeners.GetStartPosition();
  while (p)
  {
    EventMapT::CPair *pair = m_eventListeners.GetNext(p);
    RemoveListenersForTab(tabId, pair->m_value);
  }
  return S_OK;
}

CSalsitaApiServiceImpl::EventListenerArrayT *CSalsitaApiServiceImpl::GetListenerArray(LPWSTR eventId)
{
  EventListenerArrayT *ptr;
  if (!m_eventListeners.Lookup(eventId, ptr))
  {
    ptr = new EventListenerArrayT();
    m_eventListeners[eventId] = ptr;
  }
  return ptr;
}

STDMETHODIMP CSalsitaApiServiceImpl::addEventListener(LPWSTR eventId, INT tabId, LPDISPATCH listener)
{
  EventListenerArrayT *listeners = GetListenerArray(eventId);
  EventListenerT *l = new EventListenerT();
  l->tabId = tabId;
  l->listener = listener;
  listeners->Add(l);
  return S_OK;
}

HRESULT CSalsitaApiServiceImpl::CallListeners(LPWSTR eventId, INT recipientTabId, VARIANT *pvarParams, int nParams)
{
  CAtlArray<CIDispatchHelper *> recipients;

  EventListenerArrayT *listeners = GetListenerArray(eventId);
  for (size_t i = 0; i < listeners->GetCount(); i ++)
  {
    EventListenerT *l = listeners->GetAt(i);
    if (recipientTabId == -1 || l->tabId == recipientTabId)
    {
      recipients.Add(new CIDispatchHelper(l->listener)); // increases refcount
    }
  }

  for (size_t i = 0; i < recipients.GetCount(); i ++)
  {
    CIDispatchHelper * r = recipients.GetAt(i);
    r->InvokeN((DISPID)0, pvarParams, nParams);
    delete r; // release the listener
  }

  return S_OK;
}

STDMETHODIMP CSalsitaApiServiceImpl::sendRequest(INT senderTabId, INT recipientTabId, VARIANT senderObject, VARIANT request, VARIANT requestCallback)
{
  CComVariant varSenderObject = senderObject;
  CComVariant varRequest = request;
  CComVariant varCallback = requestCallback;

  CComVariant varArgs[3] = { varCallback, varSenderObject, varRequest };
  return CallListeners(L"extension.onRequest", recipientTabId, varArgs, _countof(varArgs));
}

STDMETHODIMP CSalsitaApiServiceImpl::tabActivated(INT tabId)
{
  CComVariant varTabId(tabId, VT_INT);

  CComVariant varArgs[1] = { varTabId };
  return CallListeners(L"tabs.onActivated", -1, varArgs, _countof(varArgs));
}
