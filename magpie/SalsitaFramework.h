#pragma once

#include "resource.h"
#include "Magpie.h"

class CMagpieActiveScript;

class ATL_NO_VTABLE CSalsitaFramework :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<ISalsitaFramework, &IID_ISalsitaFramework, &LIBID_MagpieLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CSalsitaFramework(CMagpieActiveScript & magpieScript);

public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
  DECLARE_NOT_AGGREGATABLE(CSalsitaFramework)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CSalsitaFramework)
    // The main interface.
    COM_INTERFACE_ENTRY(ISalsitaFramework)
    COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()
public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
  void FinalRelease();

public:
  STDMETHOD(createXMLHTTPRequest)(IDispatch** ppVal);
  STDMETHOD(makeGlobalSymbol)(LPDISPATCH pVal, BSTR globalName);

private:
  CMagpieActiveScript & m_MagpieActiveScript;

  HRESULT AddCustomInternetSecurity(CComPtr<IXMLHttpRequest> pRequest); 
};