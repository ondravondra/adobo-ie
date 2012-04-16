/****************************************************************************
 * MagpieModuleExports.h : Declaration of the CMagpieModuleExports
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#pragma once
#include "Magpie.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// forwards
class CMagpieModuleExports;
typedef CComObject<CMagpieModuleExports>  CMagpieModuleExportsComObject;

/*============================================================================
 * class CMagpieModuleExports
 * Implements the export object for a module.
 */
class ATL_NO_VTABLE CMagpieModuleExports :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IDispatchEx, &IID_IDispatchEx, &LIBID_MagpieLib,
                      /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
  // -------------------------------------------------------------------------
  // COM standard stuff
  DECLARE_NO_REGISTRY();
  DECLARE_NOT_AGGREGATABLE(CMagpieModuleExports)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
  // -------------------------------------------------------------------------
  // COM interface map
  BEGIN_COM_MAP(CMagpieModuleExports)
	  COM_INTERFACE_ENTRY(IDispatch)
	  COM_INTERFACE_ENTRY(IDispatchEx)
  END_COM_MAP()

public:
  // -------------------------------------------------------------------------
  // static creator function
  static HRESULT CreateObject(CMagpieModuleExportsComObject *&pRet);

public:
  // -------------------------------------------------------------------------
  // COM standard methods
  HRESULT FinalConstruct();
	void FinalRelease();

public:
  // -------------------------------------------------------------------------
  // Public methods.

  // locks the object for writing. Used after the exporting module has run.
  void SetReadonly(BOOL bReadOnly = TRUE)
                  {m_bReadOnly = bReadOnly;}

public:
  // -------------------------------------------------------------------------
  // IDispatchEx methods
  STDMETHOD(GetDispID)(BSTR     bstrName,
                       DWORD    grfdex,
                       DISPID * pid);

  STDMETHOD(InvokeEx)(DISPID              id,
                      LCID                lcid,
                      WORD                wFlags,
                      DISPPARAMS        * pdp,
                      VARIANT           * pvarRes,
                      EXCEPINFO         * pei,
                      IServiceProvider  * pspCaller);

  STDMETHOD(DeleteMemberByName)(BSTR  bstrName,
                                DWORD grfdex);

  STDMETHOD(DeleteMemberByDispID)(DISPID id);

  STDMETHOD(GetMemberProperties)(DISPID   id,
                                 DWORD    grfdexFetch,
                                 DWORD  * pgrfdex);

  STDMETHOD(GetMemberName)(DISPID id,
                           BSTR * pbstrName);

  STDMETHOD(GetNextDispID)(DWORD    grfdex,
                           DISPID   id,
                           DISPID * pid);

  STDMETHOD(GetNameSpaceParent)(IUnknown ** ppunk);

protected:
  // -------------------------------------------------------------------------
  // ctor
  CMagpieModuleExports();

private:
  // -------------------------------------------------------------------------
  // Private methods.

  // Cleanup (remove all exported properties when shutting down)
  void Clear();

private:
  // -------------------------------------------------------------------------
  // Private defs.
  typedef CAtlMap<DISPID, CComVariant*>
                          CAtlDispIDMap;

private:
  // -------------------------------------------------------------------------
  // Private members.

  // can properties be added to this object?
  BOOL          m_bReadOnly;

  // our data (exported properties of the module, the module's API)
  CAtlDispIDMap m_Data;
};
