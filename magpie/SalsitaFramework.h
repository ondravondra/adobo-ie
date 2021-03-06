#pragma once

#include "resource.h"
#include "Magpie.h"

class CMagpieActiveScript;

class CSalsitaFramework;
typedef CComObject<CSalsitaFramework> CSalsitaFrameworkComObject;

class ATL_NO_VTABLE CSalsitaFramework :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<ISalsitaFramework, &IID_ISalsitaFramework, &LIBID_MagpieLib, IDISP_MAGPIE_LIBVERSION>
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CSalsitaFramework();
  static HRESULT CreateObject(CSalsitaFrameworkComObject *& pRet, CMagpieActiveScript *magpieActiveScript);

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

  void Shutdown();

public:
  STDMETHOD(createXMLHTTPRequest)(IDispatch** ppVal);
  STDMETHOD(makeGlobalSymbol)(LPDISPATCH pVal, BSTR globalName);

private:
  CMagpieActiveScript *m_MagpieActiveScript;

  HRESULT AddCustomInternetSecurity(CComPtr<IXMLHttpRequest> pRequest);
};