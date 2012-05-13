#pragma once

#include "resource.h"
#include "Magpie.h"

class ATL_NO_VTABLE CSalsitaContentApi :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<ISalsitaContentApi, &IID_ISalsitaContentApi, &LIBID_MagpieLib, IDISP_MAGPIE_LIBVERSION>
{
public:
  CSalsitaContentApi();
  static HRESULT Create(IDispatch *& pRet, LPUNKNOWN pClientSite);

public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
  DECLARE_NOT_AGGREGATABLE(CSalsitaContentApi)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CSalsitaContentApi)
    // The main interface.
    COM_INTERFACE_ENTRY(ISalsitaContentApi)
    COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()
public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
  void FinalRelease();

public:
  STDMETHOD(openNewTab)(BSTR url, BOOL setActive);
  STDMETHOD(navigateTo)(BSTR url);
  STDMETHOD(openPopupWindow)(BSTR url, INT left, INT top, INT width, INT height, VARIANT onReady, INT *popupId);

private:
  HRESULT Init(LPUNKNOWN pClientSite);

  CComPtr<IWebBrowser2> m_WebBrowser;
};
