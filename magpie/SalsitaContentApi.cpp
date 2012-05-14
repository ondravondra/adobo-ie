#include "stdafx.h"
#include "SalsitaContentApi.h"

CSalsitaContentApi::CSalsitaContentApi() : m_PopupIndexSeq(0)
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

STDMETHODIMP CSalsitaContentApi::openPopupWindow(VARIANT url, INT left, INT top, INT width, INT height, VARIANT onReady, INT *popupId)
{
  ENSURE_RETVAL(popupId);

  INT id = ++ m_PopupIndexSeq;

  CComPtr<IDispatch> ready;
  if (onReady.vt == VT_DISPATCH)
  {
    ready = onReady.pdispVal;
  }

  const OLECHAR *urlVal = NULL;
  if (url.vt == VT_BSTR)
  {
    urlVal = url.bstrVal;
  }

  //TODO: coordinates
  CPopupBrowserWindow *wnd = new CPopupBrowserWindow(id, urlVal, ready.p);
  if (wnd->CreateEx() == NULL)
  {
    *popupId = 0;
    delete wnd;
    return E_FAIL;
  }

  m_Popups[id] = wnd;
  *popupId = id;
  return S_OK;
}
