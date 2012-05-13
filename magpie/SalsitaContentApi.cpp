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
  if (SUCCEEDED(hr))
  {
    hr = newObject->QueryInterface<IDispatch>(&pRet);
  }

  if (FAILED(hr))
  {
    delete newObject;
    pRet = NULL;
    return hr;
  } else {
    return S_OK;
  }
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

STDMETHODIMP CSalsitaContentApi::openNewTab(BSTR url, BOOL setActive)
{
  if (!url)
  {
    return E_INVALIDARG;
  }

  // TODO: In Microsoft Internet Explorer 6 or later, you can navigate through code only within the same domain as the application hosting the WebBrowser control. Otherwise, this method and Navigate2 are disabled. 

  CComVariant vtUrl(url);
  CComVariant vtFlags(setActive ? navOpenInNewTab : navOpenInBackgroundTab);
  return m_WebBrowser->Navigate2(&vtUrl, &vtFlags, NULL, NULL, NULL);
}

STDMETHODIMP CSalsitaContentApi::navigateTo(BSTR url)
{
  CComVariant vtUrl(url);
  return m_WebBrowser->Navigate2(&vtUrl, NULL, NULL, NULL, NULL);
}