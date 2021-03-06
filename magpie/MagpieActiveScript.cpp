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
  m_Application(application),
  m_replacementSourceContextCnt(0)
{
}

//----------------------------------------------------------------------------
//  Init
HRESULT CMagpieActiveScript::Init(CString &extensionId, CString &appId)
{
  m_ExtensionId = extensionId;

  IF_FAILED_RET(InitializeDebugInterface(appId));

  IF_FAILED_RET(LoadScriptEngine(CLSID_JScript));
  IF_FAILED_RET(m_ScriptEngine->SetScriptState(SCRIPTSTATE_INITIALIZED));

  // add salsitaFramework
  IF_FAILED_RET(CSalsitaFramework::CreateObject(m_SalsitaFramework.p, this));
  IF_FAILED_RET(AddNamedItem(L"salsitaFramework", m_SalsitaFramework, SCRIPTITEM_ISPERSISTENT|SCRIPTITEM_ISVISIBLE));
  return S_OK;
}

//----------------------------------------------------------------------------
//  Shutdown
HRESULT CMagpieActiveScript::Shutdown()
{
  UnloadScriptEngine();
  UninitializeDebugInterface();
  m_NamedItems.RemoveAll();
  if (m_SalsitaFramework)
  {
    m_SalsitaFramework->Shutdown();
  }
  return S_OK;
}

HRESULT CMagpieActiveScript::GetSalsitaObject(VARIANT * result)
{
  CIDispatchHelper script;
  IF_FAILED_RET(m_ScriptEngine->GetScriptDispatch(m_SalsitaApiModuleId, &script));
  return script.GetPropertyByName(L"salsita", result);
}

HRESULT CMagpieActiveScript::CreateSalsitaApi(INT tabId, LPUNKNOWN pSalsitaApi, VARIANT pContentApi)
{
  m_debugContextIdentifier.Format(_T("Tab id = %i"), tabId);

  IF_FAILED_RET(CSalsitaApiImpl::CreateObject(m_SalsitaApiImpl.p, tabId, pSalsitaApi));
  m_SalsitaApiImpl->m_ExtensionId = m_ExtensionId;
  m_SalsitaApiImpl->m_RootPath = m_Application.GetRootPath();

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

  script.SetPropertyByRef(L"_salsita_impl", CComVariant(m_SalsitaApiImpl));
  script.SetPropertyByRef(L"salsita", CComVariant(pSalsitaOb));

  if (pContentApi.vt == VT_DISPATCH)
  {
    m_SalsitaContentApi = pContentApi.pdispVal;
    script.SetPropertyByRef(L"_salsita_content_impl", CComVariant(m_SalsitaContentApi));
  }

  m_Application.EnterModule(m_SalsitaApiModuleId);
  HRESULT hr = LoadScriptResource(g_hDllInstance, L"salsita.js", m_SalsitaApiModuleId);
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

  CComVariant vtSalsita;
  HRESULT hr = GetSalsitaObject(&vtSalsita);
  if (SUCCEEDED(hr))
  {
    script.SetPropertyByRef(L"salsita", vtSalsita);
  }

  // now run the module
  m_Application.EnterModule(sModuleID);
  hr = LoadScriptFile(sFilename, sModuleID);
  if (SUCCEEDED(hr))
  {
    m_ScriptEngine->SetScriptState(SCRIPTSTATE_CONNECTED);
  }
  m_Application.ExitModule();

  return S_OK;
}

HRESULT CMagpieActiveScript::AddLoadedScript(LPCOLESTR lpszSource, LPCOLESTR lpszFileName, LPCOLESTR lpszModuleName)
{
  ATLASSERT(m_ScriptEngine);
  if(!m_ScriptEngine)
  {
    return E_UNEXPECTED;
  }

  DWORD_PTR sourceContext = 0;
  HRESULT hr = CActiveScriptDebugT::AddScriptFile(m_ScriptEngine, lpszFileName ? lpszFileName : lpszModuleName, lpszModuleName, lpszSource, sourceContext);
  IF_FAILED_RET(hr);
  if (hr == S_FALSE)
  {
    sourceContext = ++ m_replacementSourceContextCnt;
  }
  return CActiveScriptT::AddScript(lpszSource, lpszModuleName, sourceContext);
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
