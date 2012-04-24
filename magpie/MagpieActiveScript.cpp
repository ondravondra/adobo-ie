/****************************************************************************
 * MagpieActiveScript.cpp : Implementation of CMagpieActiveScript
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "MagpieApplication.h"
#include "MagpieModule.h"

/*============================================================================
 * class CMagpieApplication
 */

const wchar_t *CMagpieActiveScript::m_SalsitaApiModuleId = L"__salsitaapi";

//----------------------------------------------------------------------------
//  CTOR
CMagpieActiveScript::CMagpieActiveScript(CMagpieApplication & application) :
  m_Application(application)
{
}

//----------------------------------------------------------------------------
//  Init
HRESULT CMagpieActiveScript::Init()
{
  IF_FAILED_RET(LoadScriptEngine(CLSID_JScript));
  IF_FAILED_RET(m_ScriptEngine->SetScriptState(SCRIPTSTATE_INITIALIZED));
  return S_OK;
}

//----------------------------------------------------------------------------
//  Shutdown
HRESULT CMagpieActiveScript::Shutdown()
{
  UnloadScriptEngine();
  m_NamedItems.RemoveAll();
  return S_OK;
}

HRESULT CMagpieActiveScript::CreateSalsitaApi(/*IDispatch *pDispSharedState*/)
{
  m_ScriptEngine->SetScriptState(SCRIPTSTATE_DISCONNECTED);

  // add namespace for module
  IF_FAILED_RET(m_ScriptEngine->AddNamedItem(m_SalsitaApiModuleId, SCRIPTITEM_CODEONLY));

  // dispatch for module's namespace
  CIDispatchHelper script;
  IF_FAILED_RET(m_ScriptEngine->GetScriptDispatch(m_SalsitaApiModuleId, &script));

  // create salsita object
  CIDispatchHelper scriptGlobal;
  IF_FAILED_RET(m_ScriptEngine->GetScriptDispatch(NULL, &scriptGlobal));
  CComPtr<IDispatch> pSalsitaOb;
  IF_FAILED_RET(scriptGlobal.CreateObject(L"Object", &pSalsitaOb));

  script.SetPropertyByRef(L"salsita", CComVariant(pSalsitaOb));

  CStringW salsitaScript;

  Misc::LoadHtmlResource(L"salsita.js", salsitaScript);

  m_Application.EnterModule(m_SalsitaApiModuleId);
  HRESULT hr = AddScript(salsitaScript.GetBuffer(), m_SalsitaApiModuleId);
  salsitaScript.ReleaseBuffer();
  if (SUCCEEDED(hr))
  {
    m_ScriptEngine->SetScriptState(SCRIPTSTATE_CONNECTED);
  }
  m_Application.ExitModule();

  return S_OK;
}

//----------------------------------------------------------------------------
//  RunModule
HRESULT CMagpieActiveScript::RunModule(
  CMagpieModule* pModule)
{
  CComPtr<IDispatch> pModuleRequireOb(pModule->GetRequire());
  if (!pModuleRequireOb)
  {
    return E_UNEXPECTED;
  }

  CString sFilename, sModuleID;
  pModule->GetID(sModuleID);
  pModule->GetFilename(sFilename);

  m_ScriptEngine->SetScriptState(SCRIPTSTATE_DISCONNECTED);

  // add namespace for module
  IF_FAILED_RET(m_ScriptEngine->AddNamedItem(sModuleID, SCRIPTITEM_CODEONLY));

  // dispatch for module's namespace
  CIDispatchHelper script;
  IF_FAILED_RET(m_ScriptEngine->GetScriptDispatch(sModuleID, &script));

  // create exports object
  CIDispatchHelper scriptGlobal;
  IF_FAILED_RET(m_ScriptEngine->GetScriptDispatch(NULL, &scriptGlobal));
  CComPtr<IDispatch> pModuleExportsOb;
  IF_FAILED_RET(scriptGlobal.CreateObject(L"Object", &pModuleExportsOb));

  // inject CommonJS objects
  script.SetPropertyByRef(L"require", CComVariant(pModuleRequireOb));
  script.SetPropertyByRef(L"module", CComVariant(pModule));
  script.SetPropertyByRef(L"exports", CComVariant(pModuleExportsOb));

  // now run the module
  m_Application.EnterModule(sModuleID);
  HRESULT hr = E_FAIL;
  hr = AddScriptFile(sFilename, sModuleID);
  if (SUCCEEDED(hr))
  {
    m_ScriptEngine->SetScriptState(SCRIPTSTATE_CONNECTED);
  }
  m_Application.ExitModule();

  return S_OK;
}

//----------------------------------------------------------------------------
//  GetScriptDispatch
HRESULT CMagpieActiveScript::GetScriptDispatch(
  LPCOLESTR     pstrName,
  IDispatch  ** ppDisp)
{
  return m_ScriptEngine->GetScriptDispatch(pstrName, ppDisp);
}

//----------------------------------------------------------------------------
//  AddNamedItem
HRESULT CMagpieActiveScript::AddNamedItem(
  LPCOLESTR   pstrName,
  IDispatch * pDisp,
  DWORD       dwFlags)
{
  IF_FAILED_RET(m_ScriptEngine->AddNamedItem(pstrName, dwFlags));
  m_NamedItems[pstrName] = pDisp;
  return S_OK;
}

//----------------------------------------------------------------------------
//  FinalConstruct
HRESULT CMagpieActiveScript::FinalConstruct()
{
  return S_OK;
}

//----------------------------------------------------------------------------
//  FinalRelease
void CMagpieActiveScript::FinalRelease()
{
  Shutdown();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// IActiveScriptSite implementation

//----------------------------------------------------------------------------
// IActiveScriptSite
STDMETHODIMP CMagpieActiveScript::GetItemInfo(
  LPCOLESTR     pstrName,
  DWORD         dwReturnMask,
  IUnknown  **  ppiunkItem,
  ITypeInfo **  ppti)
{
  ATLASSERT(!(dwReturnMask & SCRIPTINFO_ITYPEINFO));
  if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
  {
    CComPtr<IDispatch> disp;
    if (m_NamedItems.Lookup(pstrName, disp))
    {
      return disp.QueryInterface<IUnknown>(ppiunkItem);
    }
  }
  return TYPE_E_ELEMENTNOTFOUND;
}
