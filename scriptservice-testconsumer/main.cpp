#include "stdafx.h"

extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
  HRESULT hr;

  hr = CoInitialize(NULL);

  CComPtr<IScriptService> m_pScriptService;
  hr = m_pScriptService.CoCreateInstance(CLSID_ScriptService);
  
  CComPtr<IUnknown> pObject;
  hr = m_pScriptService->GetServiceFor(CComBSTR(_T("myid")), &pObject.p);
	if (FAILED(hr))
		return hr;
  CComPtr<IDispatch> pDispatch;
  hr = pObject.QueryInterface<IDispatch>(&pDispatch.p);
	if (FAILED(hr))
		return hr;
  return 0;
}
