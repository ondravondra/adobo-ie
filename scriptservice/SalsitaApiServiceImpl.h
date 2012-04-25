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
  STDMETHOD(addEventListener)(LPWSTR eventId, INT tabId, LPDISPATCH listener);
  STDMETHOD(sendRequest)(INT senderTabId, INT recipientTabId, VARIANT senderObject, VARIANT request, VARIANT requestCallback);

private:

  typedef struct {
    INT tabId;
    CIDispatchHelper listener;
  } EventListenerT;

  /**
   * Array of event listeners for particular event id. It contains pointers because we want to manage copying and ref counting for better performance
   */
  typedef CAtlArray<EventListenerT *> EventListenerArrayT;

  /**
   * Map of event listener arrays indexed by event id.
   * Again contains pointers for optimization (this time only of local copying, no ref counts).
   */
  typedef CAtlMap<CString, EventListenerArrayT *> EventMapT;
  EventMapT m_eventListeners;

  EventListenerArrayT *GetListenerArray(LPWSTR eventId);
  void RemoveListenersForTab(INT tabId, EventListenerArrayT *listenerArray);
};