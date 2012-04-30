#pragma once

#include <activscp.h>

/**
 * This class provides methods for loading scripts from various sources.
 * Loaded scripts are added to whatever script engine the implementor chooses using a virtual method.
 */
template<class Timpl> class CActiveScriptLoaderT 
{
protected:
  virtual HRESULT AddLoadedScript(LPCOLESTR lpszSource, LPCOLESTR lpszFileName, LPCOLESTR lpszModuleName) = 0;

public:
  // Add script stored in a resource in RT_HTML-type resouces.
  // The script text has to be UTF-16, so nSrcCodePage is used for conversion.
  HRESULT LoadScriptResource(HINSTANCE hInstance,
    LPCWSTR   nResourceID,
    LPCOLESTR lpszModuleName = NULL,
    UINT      nSrcCodePage = CP_ACP)
  {
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
          HRESULT hr = AddLoadedScript(
            CA2WEX<4096>(lps, nSrcCodePage), NULL, lpszModuleName);
          delete [] lps;
          return hr;
        }
      }
    }
    return HRESULT_FROM_WIN32(::GetLastError());
  }

  // Reads a script text from a file. Requires atlfile.h.
  HRESULT LoadScriptFile(LPCOLESTR lpszFileName,
    LPCOLESTR lpszModuleName = NULL,
    UINT      nSrcCodePage = CP_ACP)
  {
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

    return AddLoadedScript(CA2WEX<4096>(sImpl, nSrcCodePage), lpszFileName, lpszModuleName);
  }
};