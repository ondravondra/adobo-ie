#include "stdafx.h"
#include "ScriptServiceInstance.h"
#include "ScriptService.h"
/*#include "..\..\libbhohelper\libbhohelper\libbhohelper.h"
using namespace LIB_BhoHelper;*/

// CScriptServiceInstance
HRESULT CScriptServiceInstance::Init(CScriptServiceCallback* pCallback, BSTR bsID)
{
	/*if(m_HiddenWindow.CreateEx() == NULL)
	{
		return E_FAIL;
	}
  m_pScriptServiceCallback = pCallback;
  m_bsID = bsID;*/
  return S_OK;
}

void CScriptServiceInstance::UnInit()
{
  /*m_Handlers.RemoveAll();
  if (m_HiddenWindow)
  {
    m_HiddenWindow.DestroyWindow();
  }
  m_pScriptServiceCallback = NULL;*/
}

HRESULT CScriptServiceInstance::FinalConstruct()
{
  //m_pScriptServiceCallback = NULL;
	return S_OK;
}

void CScriptServiceInstance::FinalRelease()
{
  /*m_Handlers.RemoveAll();
  if (m_HiddenWindow)
  {
    m_HiddenWindow.DestroyWindow();
  }
  m_CallbackInterfaces.Release();
  if (m_pScriptServiceCallback)
  {
    m_pScriptServiceCallback->OnFinalRelease(m_bsID);
  }*/
}

// IScriptServiceInstanceAdmin
STDMETHODIMP CScriptServiceInstance::SetCallback(LPUNKNOWN pUnk)
{
  return m_CallbackInterfaces.Set(pUnk);
}

STDMETHODIMP CScriptServiceInstance::ReleaseCallback()
{
  return m_CallbackInterfaces.Release();
}

STDMETHODIMP CScriptServiceInstance::addMsgHandler(LPDISPATCH handler, LONG* pvtRet)
{/*
  if (!pvtRet)
  {
    return E_POINTER;
  }
  void* key = (void*)handler;
  m_Handlers[key] = handler;

  *pvtRet = (long)key;*/
  return S_OK;
}

STDMETHODIMP CScriptServiceInstance::removeMsgHandler(LONG handlerID)
{/*
  void* key = (void*)(handlerID);
  m_Handlers.RemoveKey(key);*/
  return S_OK;
}

STDMETHODIMP CScriptServiceInstance::sendMsg(LONG handlerID, VARIANT data)
{/*
  void* key;
  CComPtr<IDispatch> value;
  DISPID disp_this = DISPID_THIS;
  HRESULT hr;
  POSITION currentPos, pos = m_Handlers.GetStartPosition();
  while(pos)
  {
    currentPos = pos;
    m_Handlers.GetNextAssoc(pos, key, value);
    CIDispatchHelper disp = value;
    VARIANTARG vt[3] = {CComVariant(disp), data, CComVariant(handlerID)};
    DISPPARAMS params = {vt, &disp_this, 3, 1};
    hr = disp.Call(L"onMessage", &params);
    if (FAILED(hr))
    {
      m_Handlers.RemoveAtPos(currentPos);
    }
  }
  */
  return S_OK;
}
