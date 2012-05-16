#pragma once

struct SalsitaExtensionEventCallback {
  virtual void ReleaseContentScriptBeforeNavigation(IDispatch *pWebBrowser) = 0;
  virtual void ReloadScriptsOnNavigation(IDispatch *pWebBrowser) = 0;
  virtual void ProcessBrowserWindowStateChanged(ULONG dwFlags, ULONG dwValidFlagsMask) = 0;
};

/**
 * This class is used to catch webbrowser events in the salsita extension without having to declare sink map in the BHO class.
 * BHO class can declare and use its own event handlers.
 */
class CSalsitaExtensionEventHandler :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CSalsitaExtensionEventHandler>,
  public IDispEventImpl<1, CSalsitaExtensionEventHandler, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 0>
{
public:

  CSalsitaExtensionEventHandler();

  SalsitaExtensionEventCallback *m_extensionCallback;

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  BEGIN_SINK_MAP(CSalsitaExtensionEventHandler)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBrowserBeforeNavigate2)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnBrowserDocumentComplete)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_WINDOWSTATECHANGED, OnBrowserWindowStateChanged)
  END_SINK_MAP()

  STDMETHOD_(void, OnBrowserBeforeNavigate2)(LPDISPATCH pDisp, VARIANT *pURL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);
  STDMETHOD_(void, OnBrowserDocumentComplete)(LPDISPATCH pDisp, VARIANT *pURL);
  STDMETHOD_(void, OnBrowserWindowStateChanged)(ULONG dwFlags, ULONG dwValidFlagsMask);
};
