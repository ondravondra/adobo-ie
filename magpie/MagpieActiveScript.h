/****************************************************************************
 * MagpieActiveScript.h : Declaration of the CMagpieActiveScript
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once

#include "Magpie.h"

// we want to be able to load scripts from files
#define _USE_SCRIPTS_FROM_FILES
#include "ActiveScriptT.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// forwards
class CMagpieApplication;
class CMagpieModule;

/*============================================================================
 * class CMagpieActiveScript
 * The magpie implementation for IActiveScriptSite based on CActiveScriptT.
 */
class ATL_NO_VTABLE CMagpieActiveScript :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CActiveScriptT<CMagpieActiveScript>
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CMagpieActiveScript(CMagpieApplication & application);

public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
  DECLARE_NOT_AGGREGATABLE(CMagpieActiveScript)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CMagpieActiveScript)
    COM_INTERFACE_ENTRY(IActiveScriptSite)
  END_COM_MAP()

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
  void FinalRelease();

public:
  // -------------------------------------------------------------------------
  // public methods

  // Init the script engine
  HRESULT Init();

  // Shutdown script engine and cleanup
  HRESULT Shutdown();

  HRESULT CreateSalsitaApi(/*IDispatch *pDispSharedState*/);

  // Run a module. Runs the module unconditionally, so this method should be
  //  used only by the module itself after checking if the module ran
  //  already.
  HRESULT RunModule(CMagpieModule* pModule);

  // Simply adds a named item to the script engine. Used to inject objects
  //  into the global namespace of the script engine.
  HRESULT AddNamedItem(LPCOLESTR    pstrName,
                       IDispatch  * pDisp,
                       DWORD        dwFlags);

  // Returns the script dispatch for a module
  HRESULT GetScriptDispatch(LPCOLESTR     pstrName,
                            IDispatch  ** ppDisp);

public:
  // -------------------------------------------------------------------------
  // IActiveScriptSite methods overriding CActiveScriptT.
  STDMETHOD(GetItemInfo)(LPCOLESTR      pstrName,
                         DWORD          dwReturnMask,
                         IUnknown   **  ppiunkItem,
                         ITypeInfo  **  ppti);

private:
  // -------------------------------------------------------------------------
  // Private members.

  // Reference to the application.
  CMagpieApplication  & m_Application;

  // Map of named items. Used in GetItemInfo.
  CAtlMap<CString, CComPtr<IDispatch> >
                        m_NamedItems;

  static const wchar_t * m_SalsitaApiModuleId;
};
