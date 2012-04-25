#pragma once

#include "service.h"
#include "resource.h"
#include "ScriptServiceFactory.h"

class ATL_NO_VTABLE CTestFactory :
  public CScriptServiceFactory<CTestFactory, &CLSID_TestFactory>
{
public:
  CTestFactory()
  {
  }

  DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTSERVICE)

  virtual HRESULT CreateScriptServiceInstance();
};

OBJECT_ENTRY_AUTO(__uuidof(TestFactory), CTestFactory)