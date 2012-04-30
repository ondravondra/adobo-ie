#pragma once

#pragma comment(linker, "/include:_library_version_libcomhelper")
extern "C" {
  extern const wchar_t *library_version_libcomhelper;
}

namespace LIB_COMHelper
{

  ///////////////////////////////////////////////////////////
  // ATL function infos for WebBrowserEvents2
  extern _ATL_FUNC_INFO info_WebBrowserEvents2_WindowClosing;
  extern _ATL_FUNC_INFO info_WebBrowserEvents2_NewWindow2;
  extern _ATL_FUNC_INFO info_WebBrowserEvents2_BeforeNavigate2;
  extern _ATL_FUNC_INFO info_WebBrowserEvents2_DocumentComplete;
  extern _ATL_FUNC_INFO info_WebBrowserEvents2_WindowSetHeight;
  extern _ATL_FUNC_INFO info_WebBrowserEvents2_WindowStateChanged;

  ///////////////////////////////////////////////////////////
  // CIDispatchHelper
  // makes IDispatch handling easier
  class CIDispatchHelper : public CComQIPtr<IDispatch>
  {
  public:
    // Methods overwritten from CComQIPtr<IDispatch>
    CIDispatchHelper() throw() :
        CComQIPtr<IDispatch>() {}
    CIDispatchHelper(IDispatch* lp) throw() :
        CComQIPtr<IDispatch>(lp) {}
    CIDispatchHelper(const CComQIPtr<IDispatch>& p) throw() :
        CComQIPtr<IDispatch>(p) {}
    IDispatch* operator=(IDispatch* lp) throw()
        {return CComQIPtr<IDispatch>::operator =(lp);};

    // Get a property from IDispatch.
    // T is the type of the return value
    // VT is the resulting VARIANT type. Get(..) trys to do a conversion to this type.
    // TCast is an optional type to cast the result to before assigning to the return value.
    // This is required e.g. for BSTR
    template <class T, VARTYPE VT, class TCast> HRESULT Get(LPOLESTR lpsName, T &Ret)
    {
      ATLASSERT(p);
      DISPID did = 0;
      LPOLESTR lpNames[] = {lpsName};
      HRESULT hr = p->GetIDsOfNames(IID_NULL, lpNames, 1, LOCALE_USER_DEFAULT, &did);
      if (FAILED(hr))
        return hr;

      DISPPARAMS params = {0};
      CComVariant vtResult;
      hr = p->Invoke(did, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &vtResult, NULL, NULL);
      if (FAILED(hr))
        return hr;
      hr = vtResult.ChangeType(VT);
      if (FAILED(hr))
        return hr;
      Ret = (T)(TCast)(vtResult.llVal);
      return S_OK;
    }

    // Put a property
    HRESULT SetProperty(LPOLESTR lpsName, CComVariant vtValue);
    HRESULT SetPropertyByRef(LPOLESTR lpsName, CComVariant vtValue);

    // Wrapper used if T == TCast
    template <class T, VARTYPE VT> HRESULT Get(LPOLESTR lpsName, T &Ret)
      {return Get<T, VT, T>(lpsName, Ret);}

    // Call a method. If lpsName is NULL method with DISPID 0 is called (used for js function objects)
    HRESULT Call(LPOLESTR lpsName = NULL, DISPPARAMS* pParams = NULL, CComVariant* pvtRet = NULL);

    // create a JS object
    HRESULT CreateObject(LPOLESTR lpsName, LPDISPATCH* ppDisp, DISPPARAMS* pConstructorParams = NULL);

#ifndef _NO_IWEBBROWSER
    // Get the IDispatch for javascript from a WebBrowserControl
    static CIDispatchHelper GetScriptDispatch(IWebBrowser2* pBrowser);
#endif
    // some methods for WebBrowserEvents2
    HRESULT Call_exOnCloseWindow();
    HRESULT Call_exOnBeforeNavigate(LPDISPATCH pDisp, VARIANT *pURL);
    HRESULT Call_exOnDocumentComplete(LPDISPATCH pDisp, VARIANT *pURL);

    // and often used ones
    HRESULT Call_exOnShowWindow();
    HRESULT Call_exOnHideWindow();
    BOOL Call_exOnDeactivateWindow();
  };

  class CComUtil {
  public:
    static CComQIPtr<IHTMLWindow2> IWebBrowserToIHTMLWindow(CComPtr<IWebBrowser2> &spBrowser);
    static HRESULT ExtractWindowMember(CComPtr<IWebBrowser2> &spBrowser, LPCOLESTR memberName, CComPtr<IDispatch> &result);
  };

  ///////////////////////////////////////////////////////////
  // CUri
  // template class for URI implentations
  template<class T> class CUri
  {
  public:
    CString m_sProtocol;
    CString m_sServer;
    CString m_sPath;

    void Empty()
    {
      __if_exists (T::InternalEmpty)
      {
        T* pT = static_cast<T*>(this);
        pT->InternalEmpty();
      }
      m_sProtocol.Empty();
      m_sServer.Empty();
      m_sPath.Empty();
    }

    HRESULT Parse(LPCTSTR lpsURI)
    {
      Empty();
      __if_exists (T::InternalParse)
      {
        T* pT = static_cast<T*>(this);
        return pT->InternalParse(lpsURI);
      }

      // get protocol
      m_sProtocol = lpsURI;
      int i = m_sProtocol.Find(_T("://"));
      if (i < 0)
        return E_FAIL;

      m_sServer = m_sProtocol.Mid(i+3);       // get server
      m_sProtocol.ReleaseBufferSetLength(i);  // truncate protocol

      i = m_sServer.Find(_T("/"));
      if (i > 0)
      {
        m_sPath = m_sServer.Mid(i);           // get path
        m_sServer.ReleaseBufferSetLength(i);  // truncate server
      }

      return S_OK;
    }

    CString Get()
    {
      __if_exists (T::InternalGet)
      {
        T* pT = static_cast<T*>(this);
        return pT->InternalGet(lpsURI);
      }
      CString sRet;
      sRet.Format(_T("%s://%s%s"), m_sProtocol, m_sServer, m_sPath);
      return sRet;
    }
  };

  ///////////////////////////////////////////////////////////
  // CResourceUri
  class CResourceUri : public CUri<CResourceUri>
  {
  public:
    CResourceUri(LPCTSTR lpsURI = NULL);
    CResourceUri(const CResourceUri &src);
    CResourceUri &operator = (LPCTSTR lpsURI);
    CResourceUri &operator = (const CResourceUri &src);
    CString Translate(CString &sModuleFileName);
    BOOL IsEqualProtDom(const CResourceUri &src);
  };

}// namespace LIB_COMHelper
