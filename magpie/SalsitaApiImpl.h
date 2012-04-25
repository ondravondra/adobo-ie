#pragma once

#include "resource.h"
#include "Magpie.h"

class CSalsitaApiImpl;
typedef CComObject<CSalsitaApiImpl> CSalsitaApiImplComObject;

class ATL_NO_VTABLE CSalsitaApiImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<ISalsitaApiInternal, &IID_ISalsitaApiInternal, &LIBID_MagpieLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CSalsitaApiImpl();
  static HRESULT CreateObject(CSalsitaApiImplComObject *& pRet, CString &extensionId, INT tabId, LPUNKNOWN pSalsitaApi);

public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
  DECLARE_NOT_AGGREGATABLE(CSalsitaApiImpl)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CSalsitaApiImpl)
    // The main interface.
    COM_INTERFACE_ENTRY(ISalsitaApiInternal)
    COM_INTERFACE_ENTRY(IDispatch)
  END_COM_MAP()

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
  void FinalRelease();

public:
  STDMETHOD(get_extensionId)(VARIANT* pVal);
  STDMETHOD(get_currentTabId)(INT* tabId);
  STDMETHOD(addRequestListener)(LPDISPATCH listener);
  STDMETHOD(performSendRequest)(INT tabId, VARIANT senderObject, VARIANT request, VARIANT requestCallback);

private:
  CString m_ExtensionId;
  INT m_TabId;
  CComQIPtr<ISalsitaApiService> m_ApiService;
};