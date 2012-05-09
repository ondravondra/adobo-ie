#include "stdafx.h"
#include "CustomInternetSecurityImpl.h"

// TODO: clean up this code, some methods seem weird

STDMETHODIMP CCustomInternetSecurityImpl::SetSecuritySite(IInternetSecurityMgrSite *pSite)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CCustomInternetSecurityImpl::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CCustomInternetSecurityImpl::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags)
{
  *pdwZone = URLZONE_TRUSTED;
  return S_OK;
}

STDMETHODIMP CCustomInternetSecurityImpl::GetSecurityId(
  LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CCustomInternetSecurityImpl::ProcessUrlAction(
  LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
{
  if(pPolicy != NULL)
  {
    // request permissions for java-applet in a banner
    // more info - http://msdn.microsoft.com/library/ms537183.aspx // msdn - About URL Security Zones
     if (URLACTION_JAVA_PERMISSIONS == dwAction)
       *pPolicy = URLPOLICY_QUERY;
     else
      *pPolicy = URLPOLICY_ALLOW;

    return S_OK;
  }
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CCustomInternetSecurityImpl::QueryCustomPolicy(
  LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CCustomInternetSecurityImpl::SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CCustomInternetSecurityImpl::GetZoneMappings(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CCustomInternetSecurityImpl::QueryStatus(
  const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
{
  return E_NOTIMPL;
}

STDMETHODIMP CCustomInternetSecurityImpl::Exec(
  const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANT *pvaIn, VARIANT *pvaOut)
{
  HRESULT hr = S_OK;
  if(pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
  {
    switch(nCmdID)
    {
      case OLECMDID_SHOWSCRIPTERROR:
      {
         (*pvaOut).vt = VT_BOOL;
        // Continue running scripts on the page.
         (*pvaOut).boolVal = VARIANT_TRUE;
      }
    }
  }
  return E_NOTIMPL; // TODO: wtf
}

STDMETHODIMP CCustomInternetSecurityImpl::QueryInterface(REFIID riid, void** ppv)
{
  if(IsEqualGUID(IID_IInternetSecurityManager, riid)){
    *ppv=(IInternetSecurityManager*) this;
  }
  else if(IsEqualGUID(IID_IUnknown, riid)){
    *ppv=(IUnknown*)((IOleClientSite*) this);
  }
  else if(IsEqualGUID(IID_IOleClientSite, riid)){
    *ppv=(IOleClientSite*) this;
  }
  else{
    *ppv=NULL;
    return E_NOINTERFACE;
  }

  return S_OK;
}

STDMETHODIMP CEnableMixedContent::SetSecuritySite(IInternetSecurityMgrSite *pSite)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CEnableMixedContent::GetSecuritySite(IInternetSecurityMgrSite **ppSite)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CEnableMixedContent::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CEnableMixedContent::GetSecurityId(
  LPCWSTR pwszUrl, BYTE *pbSecurityId, DWORD *pcbSecurityId, DWORD_PTR dwReserved)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CEnableMixedContent::ProcessUrlAction(
  LPCWSTR pwszUrl, DWORD dwAction, BYTE *pPolicy, DWORD cbPolicy,
  BYTE *pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
{
  HRESULT hr = INET_E_DEFAULT_ACTION;
  switch (dwAction)
  {
  case URLACTION_HTML_MIXED_CONTENT:
    if (pPolicy)
      *pPolicy = URLPOLICY_ALLOW;
    hr = S_OK;
  }
  return hr;
}

STDMETHODIMP CEnableMixedContent::QueryCustomPolicy(
  LPCWSTR pwszUrl, REFGUID guidKey, BYTE **ppPolicy, DWORD *pcbPolicy, BYTE *pContext, DWORD cbContext, DWORD dwReserved)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CEnableMixedContent::SetZoneMapping(DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags)
{
  return INET_E_DEFAULT_ACTION;
}

STDMETHODIMP CEnableMixedContent::GetZoneMappings(DWORD dwZone, IEnumString **ppenumString, DWORD dwFlags)
{
  return INET_E_DEFAULT_ACTION;
}
