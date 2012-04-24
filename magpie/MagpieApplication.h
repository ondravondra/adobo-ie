/****************************************************************************
 * MagpieApplication.h : Declaration of the CMagpieApplication
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once
#include "resource.h"

#include "Magpie.h"

#include "atlextensions.h"
#include "MagpieModule.h"
#include "MagpieConsole.h"
#include "MagpieActiveScript.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

/*============================================================================
 * class CMagpieApplication
 * The main magpie application object. The only class that can be intantiated
 * directly by a client.
 * This is the startpoint for every magpie program.
 */
class ATL_NO_VTABLE CMagpieApplication :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<CMagpieApplication, &CLSID_MagpieApplication>,
  public IMagpieApplication
{
public:
  // -------------------------------------------------------------------------
  // ctor
  CMagpieApplication();

public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_REGISTRY_RESOURCEID(IDR_MAGPIEAPPLICATION)
  DECLARE_NOT_AGGREGATABLE(CMagpieApplication)
  DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CMagpieApplication)
    // The main interface.
    COM_INTERFACE_ENTRY(IMagpieApplication)
    // Since the console is hidden inside CMagpieApplication but we still want
    //  to allow advising to the console events offer the console's connection
    //  point as an aggregated interface.
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IConnectionPointContainer, m_ConsolePtr)
  END_COM_MAP()

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
  void FinalRelease();

public:
  // -------------------------------------------------------------------------
  // Methods used internally and by require to get / run a module.

  // GetModule is used to get a loaded module. Returns E_FAIL if the module
  //  is not loaded.
  // lpszModuleID can be NULL, in which case the main module of the program
  //  is returned.
  HRESULT GetModule(LPCOLESTR                 lpszModuleID,
                    CMagpieModuleComObject *& pRet);

  // RunModule is the high level function to run a module. It looks up the
  //  module, and, if not found, loads it. If the module can be found or
  //  loaded it gets run (if not done already).
  // pSrcModule identifies the module trying to load the foreign module.
  //  Can be NULL in case of running the main module (on startup).
  // pRet can be NULL if the caller is not interested in the retuned module.
  HRESULT RunModule(CMagpieModule           * pSrcModule,
                    LPCOLESTR                 lpszModuleID,
                    CMagpieModuleComObject ** ppRet = NULL);

  // EnterModule / ExitModule are called whenever the script execution
  //  enters or leaves a certain module. It is used to track in which module
  //  we currently are (e.g. for logging).
  //  Calls the according methods in m_Console.
  void EnterModule(LPCOLESTR lpszModuleID);
  void ExitModule();

public:
  // -------------------------------------------------------------------------
  // Getters.

  // Get a reference to the scripting engine.
  CMagpieActiveScript & GetScriptEngine()
                        {return m_ScriptEngine;}

public:
  // -------------------------------------------------------------------------
  // IMagpieApplication methods. See .idl for description.
  STDMETHOD(Init)(const OLECHAR* lpszFolderName);
  STDMETHOD(CreateSalsitaApi)();
  STDMETHOD(Run)(const OLECHAR* lpszModuleID);
  STDMETHOD(Shutdown)();
  STDMETHOD(ScriptAddNamedItem)(const OLECHAR *name, LPDISPATCH pDisp, ULONG dwFlags);

private:
  // -------------------------------------------------------------------------
  // Private methods.

  // Resolves a module ID relative to pSrcModule. pSrcModule can be NULL when
  //  resolving the main module.
  // Returns an absolute module ID.
  HRESULT ResolveModuleID(CMagpieModule * pSrcModule,
                          LPCOLESTR       lpszModuleID,
                          CString       & sAbsoluteModuleID);

  // Internal method for loading a module. pSrcModule can be NULL for the main
  //  module.
  // LoadModule first looks if the requested module is already loaded. If so
  //  it returns S_FALSE.
  //  S_OK is only returned when the module really got loaded.
  HRESULT LoadModule(CMagpieModule          *   pSrcModule,
                     LPCOLESTR                  lpszModuleID,
                     CMagpieModuleComObject *&  pRet);

private:
  // -------------------------------------------------------------------------
  // Private members.

  // The root path of the application. Set by IMagpieApplication::Init
  CString m_RootPath;

  // Script engine.
  CComObjectStackRefCtorArg<CMagpieActiveScript, CMagpieApplication>
          m_ScriptEngine;

  // Basic console.
  CComObjectStackRefCtorArg<CMagpieConsole, CMagpieApplication>
          m_Console;

  // A map of loaded modules. Key is the module ID.
  CAtlMap<CString, CComPtr<CMagpieModuleComObject> >
          m_Modules;

  // Helper, pointing to the basic console. Used for aggregating
  //  the console interface.
  // For aggregation a raw pointer is required, using a safepointer
  //  of any kind does not work.
  IUnknown*
          m_ConsolePtr;
};

OBJECT_ENTRY_AUTO(__uuidof(MagpieApplication), CMagpieApplication)
