/****************************************************************************
 * MagpieApplication.cpp : Implementation of CMagpieApplication
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "MagpieApplication.h"
#include "DispExIdManager.h"

/*============================================================================
 * class CMagpieApplication
 */

//----------------------------------------------------------------------------
//  CTOR
CMagpieApplication::CMagpieApplication() :
  m_Console(*this),
  m_ScriptEngine(*this),
  m_ConsolePtr(NULL)
{
}

//----------------------------------------------------------------------------
//  FinalConstruct
HRESULT CMagpieApplication::FinalConstruct()
{
  // prepare the aggregated pointer
  m_Console.QueryInterface(IID_IUnknown, (void**)&m_ConsolePtr);
  return S_OK;
}

//----------------------------------------------------------------------------
//  FinalRelease
void CMagpieApplication::FinalRelease()
{
  Shutdown();
  if (m_ConsolePtr)
  {
    m_ConsolePtr->Release();
    m_ConsolePtr = NULL;
  }
}

//----------------------------------------------------------------------------
//  ResolveModuleID
//  @TODO: The handling of file extensions might have to be changed in
//         future.
HRESULT CMagpieApplication::ResolveModuleID(
  CMagpieModule * pSrcModule,
  LPCOLESTR       lpszModuleID,
  CString       & sAbsoluteModuleID)
{
  if (!lpszModuleID || !_tcslen(lpszModuleID))
  {
    // empty id? no way!
    return E_FAIL;
  }

  // if ID is already absolute...
  if (lpszModuleID[0] != _T('.'))
  {
    // ...return it as it is...
    sAbsoluteModuleID = lpszModuleID;
    // ...but strip off .js file extension if exists.
    if (sAbsoluteModuleID.Right(3) == _T(".js"))
    {
      sAbsoluteModuleID.ReleaseBufferSetLength(
        sAbsoluteModuleID.GetLength() - 3);
    }
    return S_OK;
  }

  HRESULT hr = E_FAIL;
  CString sSrcModuleID;
  // if no source module is given id will be
  // relative to root (means sSrcModuleID will be empty)
  if (pSrcModule)
  {
    pSrcModule->GetID(sSrcModuleID);
  }

  CAtlList<CString> srcIdList, idList;

  SplitString(lpszModuleID, _T("/"), idList);
  SplitString(sSrcModuleID, _T("/"), srcIdList);
  if (!srcIdList.IsEmpty())
  {
    // Remove last part, it is the "filename" part of the srcID.
    // dstID should be relative to the "folder" of srcID
    srcIdList.RemoveTailNoReturn();
  }

  // Adjust the srcIdList according to the parts in idList.
  // Any './' will change nothing.
  // Any '../' will pop off the tail of srcIdList.
  // Anything else will be appended to srcIdList.
  POSITION pos = idList.GetHeadPosition();
  while(pos)
  {
    const CString & cur = idList.GetNext(pos);
    if (cur == _T("."))
    {
      // current folder, do nothing
      continue;
    }
    if (cur == _T(".."))
    {
      // one up, remove tail from srcIdList
      if (srcIdList.IsEmpty())
      {
        // Oops, already top level.
        // Return an error.
        return E_FAIL;
      }
      srcIdList.RemoveTailNoReturn();
    }
    else
    {
      // append
      srcIdList.AddTail(cur);
    }
  }

  // Now compose final id.
  // sAbsoluteModuleID does NOT get emptied before, allowing to pass in
  // a parent path.
  pos = srcIdList.GetHeadPosition();
  while(pos)
  {
    sAbsoluteModuleID.Append(idList.GetNext(pos));
    if (pos)
    {
      sAbsoluteModuleID.AppendChar(_T('/'));
    }
  }

  // strip off .js file extension if exists.
  if (sAbsoluteModuleID.Right(3) == _T(".js"))
  {
    sAbsoluteModuleID.ReleaseBufferSetLength(
      sAbsoluteModuleID.GetLength() - 3);
  }

  return S_OK;
}

//----------------------------------------------------------------------------
//  GetModule
HRESULT CMagpieApplication::GetModule(
  LPCOLESTR                   lpszModuleID,
  CMagpieModuleComObject  *&  pRet)
{
  CComPtr<CMagpieModuleComObject> module;
  if (!lpszModuleID)
  {
    // No module ID, get first loaded ("main") module:
    POSITION pos = m_Modules.GetStartPosition();
    if (!pos)
    {
      return E_FAIL;
    }
    module = m_Modules.GetValueAt(pos);
  }
  else if (!m_Modules.Lookup(lpszModuleID, module))
  {
    // requested module not found
    return E_FAIL;
  }
  pRet = module.Detach();
  return S_OK;
}

//----------------------------------------------------------------------------
//  LoadModule
HRESULT CMagpieApplication::LoadModule(
  CMagpieModule           *   pSrcModule,
  LPCOLESTR                   lpszModuleID,
  CMagpieModuleComObject  *&  pRet)
{
  CString sModuleID;
  IF_FAILED_RET(ResolveModuleID(
    pSrcModule, lpszModuleID, sModuleID));

  CComPtr<CMagpieModuleComObject> module;

  // is the module already loaded?
  HRESULT hr = E_FAIL;
  hr = GetModule(sModuleID, module.p);
  if (SUCCEEDED(hr))
  {
    // yes, return module
    pRet = module.Detach();
    return S_FALSE;  // means: already loaded
  }

  // create a real path from the module ID...
  CString sModulePath, sModuleIDPath(sModuleID);
  sModuleIDPath.Replace(_T('/'), _T('\\'));
  // ...assuming that the file type is always 'js'
  // @TODO: Might change in future.
  sModulePath = m_RootPath + sModuleIDPath + _T(".js");

  if (!PathFileExists(sModulePath))
  {
    return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
  }

  IF_FAILED_RET(CMagpieModule::CreateObject(
    *this, sModuleID, sModulePath, module.p));

  m_Modules[sModuleID] = module;
  pRet = module.Detach();
  return S_OK;
}

//----------------------------------------------------------------------------
//  RunModule
HRESULT CMagpieApplication::RunModule(
  CMagpieModule           *   pSrcModule,
  LPCOLESTR                   lpszModuleID,
  CMagpieModuleComObject  **  ppRet)
{
  // load the module
  CComPtr<CMagpieModuleComObject> module;
  IF_FAILED_RET(LoadModule(pSrcModule, lpszModuleID, module.p));

  if (ppRet)
  {
    module.CopyTo(ppRet);
  }

  // and run
  return module->Run();
}

//----------------------------------------------------------------------------
//  EnterModule
void CMagpieApplication::EnterModule(
  LPCOLESTR lpszModuleID)
{
  m_Console.EnterModule(lpszModuleID);
}

//----------------------------------------------------------------------------
//  ExitModule
void CMagpieApplication::ExitModule()
{
  m_Console.ExitModule();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// IMagpieApplication implementation

//----------------------------------------------------------------------------
//  Init
STDMETHODIMP CMagpieApplication::Init(
  const OLECHAR* extensionId,
  const OLECHAR* lpszFolderName,
  VARIANT extensionTabId)
{
  m_ExtensionId = extensionId;

  // basic preparation
  CString sRootPath = lpszFolderName;
  if (sRootPath.IsEmpty() || !PathIsDirectory(sRootPath))
  {
    return E_INVALIDARG;
  }
  if (sRootPath[sRootPath.GetLength()-1] != _T('\\'))
  {
    sRootPath += _T('\\');
  }

  // shutdown if running
  Shutdown();

  CString appId;
  if (extensionTabId.vt == VT_INT)
  {
    appId.Format(_T("%s - %i"), extensionId, extensionTabId.intVal);
  } else {
    appId = extensionId;
  }

  // init script engine
  HRESULT hr = m_ScriptEngine.Init(m_ExtensionId, appId);
  IF_FAILED_RET(hr);

  // prepare CommonJS objects

  // console
  IF_FAILED_RET(m_Console.Init());
  IF_FAILED_RET(m_ScriptEngine.AddNamedItem(
    L"console", &m_Console, SCRIPTITEM_ISPERSISTENT|SCRIPTITEM_ISVISIBLE));

  m_RootPath = sRootPath;
  return S_OK;
}

STDMETHODIMP CMagpieApplication::CreateSalsitaApi(INT tabId, LPUNKNOWN pSalsitaApi)
{
  return m_ScriptEngine.CreateSalsitaApi(tabId, pSalsitaApi);
}

//----------------------------------------------------------------------------
//  Run
STDMETHODIMP CMagpieApplication::Run(
  const OLECHAR* lpszModuleID)
{
  if (m_RootPath.IsEmpty())
  {
    return E_UNEXPECTED;
  }

  // add the main module
  return RunModule(NULL, lpszModuleID);
}

//----------------------------------------------------------------------------
//  Shutdown
STDMETHODIMP CMagpieApplication::Shutdown()
{
  // script engine first...
  m_ScriptEngine.Shutdown();
  // ...then modules...
  m_Modules.RemoveAll();
  // ...the console...
  m_Console.Shutdown();
  // ...the DISPID manager...
  CDispExIdManager::Get().Clear();
  // ...and the root path
  m_RootPath.Empty();
  return S_OK;
}

STDMETHODIMP CMagpieApplication::ScriptAddNamedItem(const OLECHAR *name, LPDISPATCH pDisp, ULONG dwFlags)
{
  return m_ScriptEngine.AddNamedItem(name, pDisp, dwFlags);
}
