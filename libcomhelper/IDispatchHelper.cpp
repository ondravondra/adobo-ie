#include "stdafx.h"
#include "libcomhelper.h"

namespace LIB_COMHelper
{

  ///////////////////////////////////////////////////////////
  // CIDispatchHelper
  HRESULT CIDispatchHelper::Call(LPOLESTR lpsName, DISPPARAMS* pParams, CComVariant* pvtRet)
  {
    ATLASSERT(p);
    DISPID did = 0;
    if (lpsName)
    {
      LPOLESTR lpNames[] = {lpsName};
      HRESULT hr = p->GetIDsOfNames(IID_NULL, lpNames, 1, LOCALE_USER_DEFAULT, &did);
      if (FAILED(hr))
        return hr;
    }

    DISPPARAMS params = {0};
    CComVariant vtResult;
    if (!pParams)
      pParams = &params;
    if (!pvtRet)
      pvtRet = &vtResult;
    return p->Invoke(did, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, pParams, pvtRet, NULL, NULL);
  }

#ifndef _NO_IWEBBROWSER
  CIDispatchHelper CIDispatchHelper::GetScriptDispatch(IWebBrowser2* pBrowser)
  {
    ATLASSERT(pBrowser);
    CComQIPtr<IDispatch> spRet;
    CComPtr<IDispatch> spDoc;
    HRESULT hr = pBrowser->get_Document(&spDoc);
    if (FAILED(hr))
      return spRet;

    CComQIPtr<IHTMLDocument2> spHTMLDoc(spDoc);
    if (!spHTMLDoc)
      return spRet;

    CComPtr<IHTMLWindow2> spWindow;
    hr = spHTMLDoc->get_parentWindow(&spWindow);
    if (FAILED(hr))
      return spRet;
    spRet = spWindow;
    return spRet;
  }
#endif


  HRESULT CIDispatchHelper::Call_exOnCloseWindow()
  {
    return Call(L"exOnCloseWindow");
  }

  HRESULT CIDispatchHelper::Call_exOnBeforeNavigate(LPDISPATCH pDisp, VARIANT *pURL)
  {
    // arguments in reversed order!
    CComVariant args[2] = {*pURL, pDisp};
    DISPPARAMS params = {args, NULL, 2, 0};
    return Call(L"exOnBeforeNavigate", &params);
  }

  HRESULT CIDispatchHelper::Call_exOnDocumentComplete(LPDISPATCH pDisp, VARIANT *pURL)
  {
    // arguments in reversed order!
    CComVariant args[2] = {*pURL, pDisp};
    DISPPARAMS params = {args, NULL, 2, 0};
    return Call(L"exOnDocumentComplete", &params);
  }

  HRESULT CIDispatchHelper::Call_exOnShowWindow()
  {
    return Call(L"exOnShowWindow");
  }

  HRESULT CIDispatchHelper::Call_exOnHideWindow()
  {
    return Call(L"exOnHideWindow");
  }

  BOOL CIDispatchHelper::Call_exOnDeactivateWindow()
  {
    // returns TRUE if callback returns true. FALSE otherwise.
    CComVariant vt;
    HRESULT hr = Call(L"exOnDeactivateWindow", NULL, &vt);
    if (FAILED(hr))
      return FALSE;
    vt.ChangeType(VT_BOOL);
    return (VARIANT_TRUE == vt.boolVal);
  }

  HRESULT CIDispatchHelper::CreateObject(LPOLESTR lpsName, LPDISPATCH* ppDisp, DISPPARAMS* pConstructorParams)
  {
    ATLASSERT(p);
    ATLASSERT(ppDisp);

    // get DISPID for lpsName
    DISPID did = 0;
    LPOLESTR lpNames[] = {lpsName};
    HRESULT hr = p->GetIDsOfNames(IID_NULL, lpNames, 1, LOCALE_USER_DEFAULT, &did);
    if (FAILED(hr))
      return hr;

    // invoke scriptdispatch with DISPATCH_PROPERTYGET for lpsName
    CComVariant vtRet;
    DISPPARAMS params = {0};
    CComVariant vtResult;
    hr = p->Invoke(did, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &vtResult, NULL, NULL);
    if (FAILED(hr))
      return hr;

    // check result: should be a VT_DISPATCH
    if ((VT_DISPATCH != vtResult.vt) || (NULL == vtResult.pdispVal))
      return DISP_E_TYPEMISMATCH;

    // get IDispatchEx on returned IDispatch
    CComQIPtr<IDispatchEx> prototype(vtResult.pdispVal);
    if (!prototype)
      return E_NOINTERFACE;

    // call InvokeEx with DISPID_VALUE and DISPATCH_CONSTRUCT to construct new object
    vtResult.Clear();
    if (!pConstructorParams)
      pConstructorParams = &params;
    hr = prototype->InvokeEx(DISPID_VALUE, LOCALE_USER_DEFAULT, DISPATCH_CONSTRUCT, pConstructorParams, &vtResult, NULL, NULL);
    if (FAILED(hr))
      return hr;

    // vtresult should contain the new object now.
    if ((VT_DISPATCH != vtResult.vt) || (NULL == vtResult.pdispVal))
      return DISP_E_TYPEMISMATCH;

    return vtResult.pdispVal->QueryInterface(IID_IDispatch, (void**)ppDisp);
  }


}// namespace LIB_COMHelper
