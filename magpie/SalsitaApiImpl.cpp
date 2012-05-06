#include "stdafx.h"
#include "SalsitaApiImpl.h"

CSalsitaApiImpl::CSalsitaApiImpl() : m_TabId(0)
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

STDMETHODIMP CSalsitaApiImpl::get_extensionId(VARIANT* pVal)
{
  ENSURE_RETVAL(pVal);
  pVal->vt = VT_BSTR;
  pVal->bstrVal = m_ExtensionId.AllocSysString();
  return S_OK;
}

STDMETHODIMP CSalsitaApiImpl::get_currentTabId(INT* tabId)
{
  ENSURE_RETVAL(tabId);
  *tabId = m_TabId;
  return S_OK;
}

STDMETHODIMP CSalsitaApiImpl::addEventListener(BSTR eventId, LPDISPATCH listener)
{
  if (!listener)
  {
    return S_OK; // silently return because nobody can call null
  }
  return m_ApiService->addEventListener(eventId, m_TabId, listener);
}

STDMETHODIMP CSalsitaApiImpl::performSendRequest(INT tabId, VARIANT senderObject, VARIANT request, VARIANT requestCallback)
{
  return m_ApiService->sendRequest(m_TabId, tabId, senderObject, request, requestCallback);
}

STDMETHODIMP CSalsitaApiImpl::getResourceUrl(BSTR relativeUrl, VARIANT* pVal)
{
  ENSURE_RETVAL(pVal);
  if (!relativeUrl)
  {
    return E_FAIL;
  }

  bool rootEndsWithBs = (m_RootPath.Right(1).Compare(_T("\\")) == 0);
  const wchar_t *relUrl = (rootEndsWithBs && ((*relativeUrl) == '/')) ? (&relativeUrl[1]) : relativeUrl;

  CString strPath = m_RootPath + relUrl;
  strPath.Replace(L'/', L'\\');

  TCHAR canonicalized[MAX_PATH + 1];
  PathCanonicalize(canonicalized, strPath);

  CString result = _T("file:///");
  result.Append(strPath);

  pVal->vt = VT_BSTR;
  pVal->bstrVal = result.AllocSysString();
  return S_OK;
}