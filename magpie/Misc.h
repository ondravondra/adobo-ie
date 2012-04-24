/****************************************************************************
 * Misc.h : Declaration of some helper functions
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/
#pragma once

namespace Misc
{
  // tokenizes a string str and returns a list of tokens
  size_t SplitString(const CString & str, LPCTSTR tokens, CAtlList<CString> & list);
  HRESULT LoadHtmlResource(const wchar_t *name, CStringW &contents);
};
