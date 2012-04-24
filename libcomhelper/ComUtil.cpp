#include "stdafx.h"
#include "libcomhelper.h"

namespace LIB_COMHelper
{
  CComQIPtr<IHTMLWindow2> CComUtil::IWebBrowserToIHTMLWindow(CComPtr<IWebBrowser2> &spBrowser)
  {
    ATLASSERT(spBrowser != NULL);
    CComQIPtr<IHTMLWindow2> spWindow;

    // Get the document of the browser.
    CComQIPtr<IDispatch> spDisp;
    spBrowser->get_Document(&spDisp);

    if (spDisp)
    {
      // Get the window of the document.
      CComQIPtr<IHTMLDocument2> spDoc = spDisp;
      if (spDoc != NULL)
      {
        spDoc->get_parentWindow(&spWindow);
      }
    }

    if (NULL == spWindow)
    {
      spWindow = spBrowser;
    }

    return spWindow;
  }

  HRESULT CComUtil::ExtractWindowMember(CComPtr<IWebBrowser2> &spBrowser, BSTR memberName, CComPtr<IDispatch> &result)
  {
    CIDispatchHelper window = CComUtil::IWebBrowserToIHTMLWindow(spBrowser);
    return window.Get<CComPtr<IDispatch>, VT_DISPATCH, IDispatch *>(memberName, result);
  }
}