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
  closeAllPopupWindows();
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

  CComObject<CPopupBrowser> *wnd;
  IF_FAILED_RET(CComObject<CPopupBrowser>::CreateInstance(&wnd));
  wnd->AddRef();
  wnd->Init(id, ready.p, NULL /* TODO: implement */, dynamic_cast<PopupBrowserEventCallback *>(this));
  RECT rect;

  if (left == -1 || top == -1 || width == -1 || height == -1) // all or nothing, ATL is stupid
  {
    rect.left = rect.top = CW_USEDEFAULT, CW_USEDEFAULT;
    rect.right = rect.bottom = 0;
  } else {
    rect.left = left;
    rect.right = left + width;
    rect.top = top;
    rect.bottom = top + height;
  }

  HWND parent = NULL;
  LONG_PTR parentLongHwnd;
  if (SUCCEEDED(m_WebBrowser->get_HWND(&parentLongHwnd)))
  {
    parent = (HWND)parentLongHwnd;
  }

  if (wnd->Create(parent, &rect, urlVal ? urlVal : _T("about:blank"), WS_POPUP, 0) == NULL)
  {
    *popupId = 0;
    delete wnd;
    return E_FAIL;
  }

  wnd->ShowWindow(SW_SHOW);

  m_Popups[id] = wnd;
  *popupId = id;
  return S_OK;
}

void CSalsitaContentApi::DoClosePopupWindow(CPopupBrowser *browser)
{
  if (!browser)
  {
    return;
  }

  if (browser->IsWindow())
  {
    browser->DestroyWindow();
  }

  browser->Dispose();

  browser->Release();
}

STDMETHODIMP CSalsitaContentApi::closePopupWindow(INT popupId)
{
  m_PopupMapT::CPair *pair = m_Popups.Lookup(popupId);
  if (!pair || !pair->m_value)
  {
    return S_OK; ///< already closed
  }

  DoClosePopupWindow(pair->m_value);

  m_Popups.RemoveKey(popupId);
  return S_OK;
}

STDMETHODIMP CSalsitaContentApi::closeAllPopupWindows()
{
  POSITION p = m_Popups.GetStartPosition();
  while (p)
  {
    m_PopupMapT::CPair *pair = m_Popups.GetNext(p);
    DoClosePopupWindow(pair->m_value);
  }

  m_Popups.RemoveAll();

  return S_OK;
}

bool CSalsitaContentApi::GetActivatedWindowPopupId(HWND activatedWindow, INT &popupId)
{
  POSITION p = m_Popups.GetStartPosition();
  while (p)
  {
    m_PopupMapT::CPair *pair = m_Popups.GetNext(p);
    if (activatedWindow && (pair->m_value->m_hWnd == activatedWindow || IsChild(pair->m_value->m_hWnd, activatedWindow)))
    {
      popupId = pair->m_key;
      return true;
    }
  }

  return false;
}
