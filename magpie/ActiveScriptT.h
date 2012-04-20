/****************************************************************************
 * MagpieApplication.h : ActiveScriptT.h : Template class for script engine
 * implementation.
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/
// ActiveScriptT.h : Template class for script engine implementation

#pragma once

#include <activscp.h>

// must be declared somewhere in a .cpp file
extern CLSID CLSID_JScript;

/*============================================================================
 * template CActiveScriptT
 * Implements IActiveScriptSite.
 * Implementors can override methods of IActiveScriptSite they require to
 * handle.
 */
template<class Timpl> class CActiveScriptT : public IActiveScriptSite
{
public:
  // -------------------------------------------------------------------------
  // Initialization and cleanup methods.

  // Terminates script execution and unloads the script engine.
  void UnloadScriptEngine()
  {
    if (m_ScriptEngine)
    {
      EXCEPINFO inf = {};
      m_ScriptEngine->InterruptScriptThread(SCRIPTTHREADID_ALL, &inf, 0);
      if (inf.bstrSource)
        SysFreeString(inf.bstrSource);
      if (inf.bstrDescription)
        SysFreeString(inf.bstrDescription);
      if (inf.bstrHelpFile)
        SysFreeString(inf.bstrHelpFile);
    }
    m_ScriptGlobal.Release();
    m_ScriptEngineParser.Release();
    if (m_ScriptEngine)
    {
      m_ScriptEngine->Close();
    }
    m_ScriptEngine.Release();
  }

  // Loads and prepares the script engine.
  //  LoadScriptEngine will leave the engine in SCRIPTSTATE_INITIALIZED.
  //  So after adding scripts put enigne (back) to state SCRIPTSTATE_CONNECTED
  //  to run the scripts.
  HRESULT LoadScriptEngine(CLSID &clsid)
  {
    if (m_ScriptEngine)
    {
      return E_UNEXPECTED;
    }
    HRESULT hr = E_FAIL;
    ATLASSERT(NULL == m_ScriptGlobal);

    do
    {
      // create engine
      hr = ::CoCreateInstance(
        clsid, NULL, CLSCTX_ALL, IID_IActiveScript, (void **)&m_ScriptEngine);
      if(FAILED(hr)) break;

      // set this as script site
      hr = m_ScriptEngine->SetScriptSite(
        static_cast<IActiveScriptSite*>(this));
      if(FAILED(hr)) break;

      // get IActiveScriptParse-interface
      hr = m_ScriptEngine->QueryInterface(
        IID_IActiveScriptParse, (void**)&m_ScriptEngineParser);
      if(FAILED(hr)) break;

      // init IActiveScriptParse
      hr = m_ScriptEngineParser->InitNew();
      if(FAILED(hr)) break;

      // get the script dispatch for global namespace
      CComPtr<IDispatch> pDisp;
      hr = m_ScriptEngine->GetScriptDispatch(NULL, &pDisp);
      if(FAILED(hr)) break;

      m_ScriptGlobal = pDisp;
    } while(false);
    if (FAILED(hr))
    {
      // something failed, cleanup
      ATLASSERT(0);
      UnloadScriptEngine();
    }
    return hr;
  }

  // -------------------------------------------------------------------------
  // Adding scripts

  // Add script as raw string. This is the lowlevel method.
  //  Before adding a script text make sure engine is in state
  //  SCRIPTSTATE_DISCONNECTED or SCRIPTSTATE_INITIALIZED.
  // lpszSource is the script encoded as UTF-16.
  // lpszModuleName is the namespace for the module, in case it is NULL
  //  the script will get added to the global namespace.
  HRESULT AddScript(LPCOLESTR lpszSource,
                    LPCOLESTR lpszModuleName = NULL)
  {
    ATLASSERT(m_ScriptEngine && m_ScriptEngineParser);
    if(!m_ScriptEngine || !m_ScriptEngineParser)
    {
      return E_UNEXPECTED;
    }

    DWORD id = m_SourceContextSeq ++;
    m_scriptModuleNameMap.Add(id, lpszModuleName);

    // parse script text
    IF_FAILED_RET(m_ScriptEngineParser->ParseScriptText(
      lpszSource, lpszModuleName, 0, 0, id, 1, SCRIPTTEXT_ISPERSISTENT, 0, 0));
    return S_OK;
  }

  // Add script stored in a resource in RT_HTML-type resouces.
  // The script text has to be UTF-16, so nSrcCodePage is used for conversion.
  HRESULT AddScriptResource(HINSTANCE hInstance,
                            LPCWSTR   nResourceID,
                            LPCOLESTR lpszModuleName = NULL,
                            UINT      nSrcCodePage = CP_ACP)
  {
    ATLASSERT(m_ScriptEngine && m_ScriptEngineParser);
    if(!m_ScriptEngine || !m_ScriptEngineParser)
    {
      return E_UNEXPECTED;
    }

    ATLTRACE(_T("ADD SCRIPT %s\n"), nResourceID);

    HRSRC hRes = FindResource(
      hInstance, nResourceID, MAKEINTRESOURCE(RT_HTML));
    if (hRes)
    {
      DWORD dwSize = SizeofResource(hInstance, hRes);
      HGLOBAL hResLoad = LoadResource(hInstance, hRes);
      if (hResLoad)
      {
        LPVOID lpResLock = LockResource(hResLoad);
        if (lpResLock)
        {
          LPSTR lps = (LPSTR)new BYTE[dwSize + 1];
          if (!lps)
            return E_OUTOFMEMORY;
          memcpy(lps, lpResLock, dwSize);
          lps[dwSize] = 0;
          HRESULT hr = AddScript(
            CA2WEX<4096>(lps, nSrcCodePage), lpszModuleName);
          delete [] lps;
          return hr;
        }
      }
    }
    return HRESULT_FROM_WIN32(::GetLastError());
  }

#ifdef _USE_SCRIPTS_FROM_FILES
  // Reads a script text from a file. Requires atlfile.h.
  HRESULT AddScriptFile(LPCOLESTR lpszFileName,
                        LPCOLESTR lpszModuleName = NULL,
                        UINT      nSrcCodePage = CP_ACP)
  {
    ATLASSERT(m_ScriptEngine && m_ScriptEngineParser);
    if(!m_ScriptEngine || !m_ScriptEngineParser)
    {
      return E_UNEXPECTED;
    }

    CAtlFile f;

    IF_FAILED_RET(f.Create(
      lpszFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING));

    ULONGLONG nLen = 0;
    HRESULT hr = E_FAIL;

    IF_FAILED_RET(f.GetSize(nLen));
    // limit script size to 4GB
    if (nLen > 0x00000000ffffffff)
      return E_OUTOFMEMORY;

    DWORD dwLen = (DWORD)(nLen & 0x00000000ffffffff);
    CStringA sImpl;
    LPVOID lpv = sImpl.GetBuffer(dwLen);
    if (!lpv)
      return E_OUTOFMEMORY;
    hr = f.Read(lpv, dwLen);
    sImpl.ReleaseBuffer(dwLen);
    f.Close();
    IF_FAILED_RET(hr);

    return AddScript(CA2WEX<4096>(sImpl, nSrcCodePage), lpszModuleName);
  }
#endif

  // -------------------------------------------------------------------------
  // IActiveScriptSite implementation
  STDMETHOD(GetLCID)(LCID *plcid)
  {
    *plcid = LOCALE_USER_DEFAULT;
    return S_OK;
  }

  STDMETHOD(GetItemInfo)(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppiunkItem, ITypeInfo **ppti)
  {
    return TYPE_E_ELEMENTNOTFOUND;
  }

  STDMETHOD(GetDocVersionString)(BSTR *pbstrVersion)
  {
    return E_NOTIMPL;
  }

  STDMETHOD(OnScriptTerminate)(const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo)
  {
    return S_OK;
  }

  STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState)
  {
    ATLTRACE(_T("SCRIPT OnStateChange: %i\n"), ssScriptState);
    return S_OK;
  }

  STDMETHOD(OnScriptError)(IActiveScriptError *pscripterror)
  {
#ifdef _DEBUG
    CString sSrc;
    EXCEPINFO inf = {0};
    CString sErr, sDesc;

    pscripterror->GetExceptionInfo(&inf);
    if (inf.bstrDescription)
    {
      sDesc = inf.bstrDescription;
    }
    if (inf.bstrSource)
    {
      sErr = inf.bstrSource;
    }
    DWORD dwSourceContext;
    ULONG lLine = 0;
    long lChar = 0;
    pscripterror->GetSourcePosition(&dwSourceContext, &lLine, &lChar);
    CString cstrModuleName = m_scriptModuleNameMap.Lookup(dwSourceContext);
    LPCWSTR moduleName = cstrModuleName;

    CString sOut;
    sOut.Format(_T("================================================================================\nError 0x%08x\nin %s at line %i pos %i:\n%s\n%s\n================================================================================\n"),
      inf.scode, moduleName, lLine, lChar, sErr, sDesc);

    ATLTRACE(sOut);
#endif
    return S_OK;
  }

  STDMETHOD(OnEnterScript)()
  {
    return S_OK;
  }

  STDMETHOD(OnLeaveScript)()
  {
    return S_OK;
  }

protected:
  CActiveScriptT() : m_SourceContextSeq(0)
  {}

  // -------------------------------------------------------------------------
  // protected members

  // the script engine itself
  CComPtr<IActiveScript>			m_ScriptEngine;

  // script parser
  CComPtr<IActiveScriptParse>	m_ScriptEngineParser;

  // IDispatchEx for global script namespace
  CComQIPtr<IDispatchEx>		  m_ScriptGlobal;

  DWORD m_SourceContextSeq;
  CSimpleMap<DWORD, CString>  m_scriptModuleNameMap;
};
