#include "stdafx.h"
#include "ScriptService.h"

void CScriptService::OnFinalRelease(BSTR bsID)
{
  m_Objects.RemoveKey(bsID);
}

void CScriptService::FinalRelease()
{
  CScriptServiceInstanceComObject* pObject;
  CString sID;
  POSITION pos = m_Objects.GetStartPosition();
  while(pos)
  {
    m_Objects.GetNextAssoc(pos, sID, pObject);
    pObject->UnInit();
  }
  m_Objects.RemoveAll();
}

STDMETHODIMP CScriptService::GetServiceFor(BSTR bsID, LPUNKNOWN* ppUnk)
{
  CScriptServiceInstanceComObject* pObject = NULL;
  if (!m_Objects.Lookup(bsID, pObject))
  {
    ATLTRACE(_T("ADD OBJECT %s\n"), bsID);
    HRESULT hr = CScriptServiceInstanceComObject::CreateInstance(&pObject);
    if (FAILED(hr))
    {
      return hr;
    }
    hr = pObject->Init(this, bsID);
    if (FAILED(hr))
    {
      delete pObject;
      return hr;
    }
    m_Objects[bsID] = pObject;
  }
  else
  {
    ATLTRACE(_T("FOUND OBJECT %s\n"), bsID);
  }
  return pObject->QueryInterface<IUnknown>(ppUnk);
}

