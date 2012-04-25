#include "stdafx.h"
#include "ScriptService.h"

HRESULT CTestFactory::CreateScriptServiceInstance()
{
  HRESULT hr;

  hr = m_ScriptServiceInstance.CoCreateInstance(CLSID_ScriptServiceInstance);
  if (FAILED(hr))
  {
    return hr;
  }

  hr = m_ScriptServiceInstance->Init(L"myExtension", L"myDir");
  return hr;
}