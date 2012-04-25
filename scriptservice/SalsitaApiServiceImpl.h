#pragma once

#include "service.h"

class CSalsitaApiServiceImpl;
typedef CComObject<CSalsitaApiServiceImpl> CSalsitaApiServiceImplComObject;

class ATL_NO_VTABLE CSalsitaApiServiceImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CSalsitaApiServiceImpl>,
  public ISalsitaApiService
{
public:

  static HRESULT CreateObject(CSalsitaApiServiceImplComObject *& pRet);

  DECLARE_NOT_AGGREGATABLE(CSalsitaApiServiceImpl)

  BEGIN_COM_MAP(CSalsitaApiServiceImpl)
    COM_INTERFACE_ENTRY(ISalsitaApiService)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct();
  void FinalRelease();

  STDMETHOD(connectClient)(INT tabId);
  STDMETHOD(releaseClient)(INT tabId);
  STDMETHOD(addRequestListener)(INT tabId, LPDISPATCH listener);
  STDMETHOD(sendRequest)(INT senderTabId, INT recipientTabId, VARIANT senderObject, VARIANT request, VARIANT requestCallback);

private:

  typedef struct {
    INT tabId;
    CIDispatchHelper listener;
  } RequestListenerT;

  CAtlArray<RequestListenerT *> m_listeners; ///< all request listeners, array contains pointers because we want to manage copying and ref counting for better performance
};