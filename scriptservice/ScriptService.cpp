#include "stdafx.h"
#include "ScriptService.h"

HRESULT CTestFactory::InitScriptServiceInstance()
{
  HRESULT hr;
  hr = m_ScriptServiceInstance->Init(L"myExtension", L"myDir");
  return hr;
}