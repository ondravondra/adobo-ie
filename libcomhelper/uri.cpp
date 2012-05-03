#include "stdafx.h"
#include "include\libcomhelper.h"

namespace LIB_COMHelper
{

  CResourceUri::CResourceUri(LPCTSTR lpsURI)
  {
    if (lpsURI)
      Parse(lpsURI);
  }

  CResourceUri::CResourceUri(const CResourceUri &src)
  {
    m_sProtocol = src.m_sProtocol;
    m_sServer = src.m_sServer;
    m_sPath = src.m_sPath;
  }

  CResourceUri &CResourceUri::operator = (LPCTSTR lpsURI)
  {
    if (lpsURI)
      Parse(lpsURI);
    return *this;
  }

  CResourceUri &CResourceUri::operator = (const CResourceUri &src)
  {
    m_sProtocol = src.m_sProtocol;
    m_sServer = src.m_sServer;
    m_sPath = src.m_sPath;
    return *this;
  }

  CString CResourceUri::Translate(CString &sModuleFileName)
  {
    CString sPath(m_sPath);
    sPath.Replace(_T('/'), _T('|'));
    CString sRet;
    sRet.Format(_T("res://%s/%s"), sModuleFileName, sPath);
    return S_OK;
  }

  BOOL CResourceUri::IsEqualProtDom(const CResourceUri &src)
  {
    return ((src.m_sProtocol == m_sProtocol) && (src.m_sServer == m_sServer));
  }

}// namespace LIB_COMHelper
