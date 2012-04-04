#pragma once

#include "service.h"
#include "resource.h"
#include "ScriptServiceInstance.h"

// CScriptServiceCallback
struct CScriptServiceCallback
{
  virtual void OnFinalRelease(BSTR bsID) = 0;
};

class ATL_NO_VTABLE CScriptService :
  public CScriptServiceCallback,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptService, &CLSID_ScriptService>,
	public IScriptService
{
private:
  CAtlMap<CString, CScriptServiceInstanceComObject*> m_Objects;

public:
	CScriptService()
	{
	}

  DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTSERVICE)
  DECLARE_CLASSFACTORY_SINGLETON(CScriptService)

  DECLARE_NOT_AGGREGATABLE(CScriptService)

  BEGIN_COM_MAP(CScriptService)
	  COM_INTERFACE_ENTRY(IScriptService)
  END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

  virtual void OnFinalRelease(BSTR bsID);
	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease();

public:
  STDMETHOD(GetBackgroundScriptService)(BSTR bsID, BSTR initialUrl, LPUNKNOWN* ppUnk);
};

OBJECT_ENTRY_AUTO(__uuidof(ScriptService), CScriptService)