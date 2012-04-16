/****************************************************************************
 * MagpieModule.cpp : Implementation of CMagpieModule
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "MagpieModule.h"
#include "MagpieActiveScript.h"
#include "MagpieApplication.h"

/*============================================================================
 * class CMagpieModule
 */

//----------------------------------------------------------------------------
//  static CreateObject
HRESULT CMagpieModule::CreateObject(
  CMagpieApplication      &   application,
  LPCOLESTR                   lpsModuleID,
  LPCOLESTR                   lpsFileName,
  CMagpieModuleComObject  *&  pRet)
{
  CMagpieModuleComObject *newObject = pRet = NULL;
  IF_FAILED_RET(CMagpieModuleComObject::CreateInstance(&newObject));
  newObject->AddRef();

  HRESULT hr = E_FAIL;
  hr = newObject->Init(application, lpsModuleID, lpsFileName);
  if(FAILED(hr))
  {
    newObject->Release();
    return hr;
  }

  pRet = newObject;
  return S_OK;
}

//----------------------------------------------------------------------------
//  CTOR
CMagpieModule::CMagpieModule() :
  m_pApplication(NULL),
  m_bDidRun(FALSE)
{
}

//----------------------------------------------------------------------------
//  FinalConstruct
HRESULT CMagpieModule::FinalConstruct()
{
	return S_OK;
}

//----------------------------------------------------------------------------
//  FinalRelease
void CMagpieModule::FinalRelease()
{
  m_Exports.Release();
  m_Require.Release();
}

//----------------------------------------------------------------------------
//  Init
HRESULT CMagpieModule::Init(
  CMagpieApplication  & application,
  LPCOLESTR             lpsModuleID,
  LPCOLESTR             lpsFileName)
{
  m_pApplication = &application;
  m_sID = lpsModuleID;
  m_sFilename = lpsFileName;

  // create "require" object
  IF_FAILED_RET(CMagpieRequire::CreateObject(*this, m_Require.p));

  // create "exports" object
  IF_FAILED_RET(CMagpieModuleExports::CreateObject(m_Exports.p));

  return S_OK;
}

//----------------------------------------------------------------------------
//  RunModule
HRESULT CMagpieModule::RunModule(
  LPCOLESTR                   lpszModuleID,
  CMagpieModuleComObject  **  ppModule)
{
  ATLASSERT(m_pApplication);
  return m_pApplication->RunModule(this, lpszModuleID, ppModule);
}

//----------------------------------------------------------------------------
//  Run
HRESULT CMagpieModule::Run()
{
  if (m_bDidRun)
  {
    // we ran already.
    return S_FALSE;
  }
  ATLASSERT(m_pApplication);

  m_bDidRun = TRUE;
  IF_FAILED_RET(m_pApplication->GetScriptEngine().RunModule(this));

  // prevent changes to exports
  m_Exports->SetReadonly(TRUE);
  
  return S_OK;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// IMagpieModule implementation

//----------------------------------------------------------------------------
//  get_id
STDMETHODIMP CMagpieModule::get_id(
  VARIANT* pVal)
{
  ENSURE_RETVAL(pVal);
  pVal->vt = VT_BSTR;
  pVal->bstrVal = m_sID.AllocSysString();
  return S_OK;
}

//----------------------------------------------------------------------------
//  get_exports
STDMETHODIMP CMagpieModule::get_exports(
  VARIANT* pVal)
{
  ENSURE_RETVAL(pVal);

  IF_FAILED_RET(
    m_Exports->QueryInterface(IID_IDispatch, (void**)&pVal->pdispVal));
  pVal->vt = VT_DISPATCH;
  return S_OK;
}
