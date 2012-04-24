/****************************************************************************
 * Misc.cpp : Implementation of some helper functions
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "StdAfx.h"
#include "Misc.h"

size_t Misc::SplitString(const CString & str, LPCTSTR tokens, CAtlList<CString> & list)
{
  list.RemoveAll();
  CAtlString token;
  int curPos = 0;

  token = str.Tokenize(tokens, curPos);
  while (-1 != curPos)
  {
    if (!token.IsEmpty())
    {
      list.AddTail(token);
    }
    token = str.Tokenize(tokens, curPos);
  }
  return list.GetCount();
}

HRESULT Misc::LoadHtmlResource(const wchar_t *name, CStringW &contents)
{
  HRESULT hrRet = E_FAIL;
  HRSRC hrc = FindResourceW(g_hDllInstance, name, RT_HTML);
  if (NULL != hrc)
  {
    HGLOBAL hGlobalResource = LoadResource(g_hDllInstance, hrc);
    if (hGlobalResource)
    {
      DWORD sz = SizeofResource(g_hDllInstance, hrc);
      LPCSTR lpData = (LPCSTR)LockResource(hGlobalResource);
      if (lpData)
      {
        int wsz = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpData, sz, NULL, 0);
        if (wsz && wsz != 0xFFFD)
        {
          LPWSTR buf = contents.GetBuffer(wsz);
          ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)lpData, sz, buf, wsz);
          buf[wsz] = 0;
          contents.ReleaseBuffer();
          hrRet = S_OK;
        } else {
          hrRet = E_FAIL;
        }
      }
      FreeResource(hGlobalResource);
    }
  }
  return hrRet;
}