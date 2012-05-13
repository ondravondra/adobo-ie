#include "stdafx.h"
#include "SalsitaContentApi.h"
#include <shlguid.h>

CSalsitaContentApi::CSalsitaContentApi()
{
}

HRESULT CSalsitaContentApi::Create(IDispatch *& pRet, LPUNKNOWN pClientSite)
{
  CComObject<CSalsitaContentApi> *newObject = NULL;
  IF_FAILED_RET(CComObject<CSalsitaContentApi>::CreateInstance(&newObject));

  HRESULT hr = newObject->Init(pClientSite);
  if (FAILED(hr))
  {
    delete newObject;
    return hr;
  }

  newObject->QueryInterface<IDispatch>(&pRet);
  return S_OK;
}

HRESULT CSalsitaContentApi::FinalConstruct()
{
  return S_OK;
}

void CSalsitaContentApi::FinalRelease()
{
}

HRESULT CSalsitaContentApi::Init(LPUNKNOWN pClientSite)
{
  CComQIPtr<IServiceProvider> pServiceProvider = pClientSite;
  if (!pServiceProvider)
  {
    return E_INVALIDARG;
  }

  pServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (LPVOID*)&m_WebBrowser.p);
  if (!m_WebBrowser)
  {
    return E_INVALIDARG;
  }

  return S_OK;
}
