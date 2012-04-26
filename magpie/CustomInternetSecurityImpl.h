#pragma once

#include "resource.h"

class ATL_NO_VTABLE CCustomInternetSecurityImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<IDispatch, &IID_IDispatch, NULL, /*wMajor =*/ 1, /*wMinor =*/ 0>,
  public IServiceProviderImpl<CCustomInternetSecurityImpl>,
  public IInternetSecurityManager,
  public IOleCommandTarget,
  public IOleClientSite
{
public:
  CCustomInternetSecurityImpl()
  {
  }

  BEGIN_SERVICE_MAP(CCustomInternetSecurityImpl)
    SERVICE_ENTRY(__uuidof(IInternetSecurityManager))
  END_SERVICE_MAP()

  BEGIN_COM_MAP(CCustomInternetSecurityImpl)
    COM_INTERFACE_ENTRY(IInternetSecurityManager)
    COM_INTERFACE_ENTRY(IServiceProvider)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IOleCommandTarget)
    COM_INTERFACE_ENTRY(IOleClientSite)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
  }

public:
  STDMETHOD(SetSecuritySite)(
    /* [unique][in] */ __RPC__in_opt IInternetSecurityMgrSite *pSite);

  STDMETHOD(GetSecuritySite)(
    /* [out] */ __RPC__deref_out_opt IInternetSecurityMgrSite **ppSite);

  STDMETHOD(MapUrlToZone)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [out] */ __RPC__out DWORD *pdwZone,
    /* [in] */ DWORD dwFlags);

  STDMETHOD(GetSecurityId)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [size_is][out] */ __RPC__out_ecount_full(*pcbSecurityId) BYTE *pbSecurityId,
    /* [out][in] */ __RPC__inout DWORD *pcbSecurityId,
    /* [in] */ DWORD_PTR dwReserved);

  STDMETHOD(ProcessUrlAction)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [in] */ DWORD dwAction,
    /* [size_is][out] */ __RPC__out_ecount_full(cbPolicy) BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ __RPC__in BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwReserved);

  STDMETHOD(QueryCustomPolicy)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [in] */ __RPC__in REFGUID guidKey,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcbPolicy) BYTE **ppPolicy,
    /* [out] */ __RPC__out DWORD *pcbPolicy,
    /* [in] */ __RPC__in BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwReserved);

  STDMETHOD(SetZoneMapping)(
    /* [in] */ DWORD dwZone,
    /* [in] */ __RPC__in LPCWSTR lpszPattern,
    /* [in] */ DWORD dwFlags);

  STDMETHOD(GetZoneMappings)(
    /* [in] */ DWORD dwZone,
    /* [out] */ __RPC__deref_out_opt IEnumString **ppenumString,
    /* [in] */ DWORD dwFlags);

  // IOleCommandTarget
public:
  STDMETHOD (QueryStatus)(
    /* [unique][in] */ const GUID *pguidCmdGroup,
    /* [in] */ ULONG cCmds,
    /* [out][in][size_is] */ OLECMD prgCmds[  ],
    /* [unique][out][in] */ OLECMDTEXT *pCmdText);

  STDMETHOD(Exec)(
    /*[in]*/ const GUID *pguidCmdGroup,
    /*[in]*/ DWORD nCmdID,
    /*[in]*/ DWORD nCmdExecOpt,
    /*[in]*/ VARIANTARG *pvaIn,
    /*[in,out]*/ VARIANTARG *pvaOut);

  //IOleClientSite
  STDMETHODIMP SaveObject()
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP GetMoniker(
    /* [in] */ DWORD dwAssign,
    /* [in] */ DWORD dwWhichMoniker,
    /* [out] */ IMoniker **ppmk)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP GetContainer(/* [out] */ IOleContainer **ppContainer)
  {
    return E_NOTIMPL;
  }

  STDMETHODIMP ShowObject()
  {
    return S_OK;
  }

  STDMETHODIMP OnShowWindow(/* [in] */ BOOL fShow)
  {
    return S_OK;
  }

  STDMETHODIMP RequestNewObjectLayout()
  {
    return E_NOTIMPL;
  }
};

// CEnableMixedContent
// Warnings about mixed content on the page are not shown.
// This class handles nothing more than visual event.
class ATL_NO_VTABLE CEnableMixedContent :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IInternetSecurityManager
{
public:
  CEnableMixedContent()
  {
  }

  BEGIN_COM_MAP(CEnableMixedContent)
    COM_INTERFACE_ENTRY(IInternetSecurityManager)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
  }

public:
  STDMETHOD(SetSecuritySite)(
    /* [unique][in] */ __RPC__in_opt IInternetSecurityMgrSite *pSite);

  STDMETHOD(GetSecuritySite)(
    /* [out] */ __RPC__deref_out_opt IInternetSecurityMgrSite **ppSite);

  STDMETHOD(MapUrlToZone)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [out] */ __RPC__out DWORD *pdwZone,
    /* [in] */ DWORD dwFlags);

  STDMETHOD(GetSecurityId)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [size_is][out] */ __RPC__out_ecount_full(*pcbSecurityId) BYTE *pbSecurityId,
    /* [out][in] */ __RPC__inout DWORD *pcbSecurityId,
    /* [in] */ DWORD_PTR dwReserved);

  STDMETHOD(ProcessUrlAction)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [in] */ DWORD dwAction,
    /* [size_is][out] */ __RPC__out_ecount_full(cbPolicy) BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ __RPC__in BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwReserved);

  STDMETHOD(QueryCustomPolicy)(
    /* [in] */ __RPC__in LPCWSTR pwszUrl,
    /* [in] */ __RPC__in REFGUID guidKey,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcbPolicy) BYTE **ppPolicy,
    /* [out] */ __RPC__out DWORD *pcbPolicy,
    /* [in] */ __RPC__in BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwReserved);

  STDMETHOD(SetZoneMapping)(
    /* [in] */ DWORD dwZone,
    /* [in] */ __RPC__in LPCWSTR lpszPattern,
    /* [in] */ DWORD dwFlags);

  STDMETHOD(GetZoneMappings)(
    /* [in] */ DWORD dwZone,
    /* [out] */ __RPC__deref_out_opt IEnumString **ppenumString,
    /* [in] */ DWORD dwFlags);
};
