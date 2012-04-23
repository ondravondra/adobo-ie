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

STDMETHODIMP CScriptService::GetServiceFor(BSTR serviceIdentifier, BSTR resourcesDir, LPUNKNOWN* ppUnk)
{
  CScriptServiceInstanceComObject* pObject;
  if (!m_Objects.Lookup(serviceIdentifier, pObject))
  {
    ATLTRACE(_T("ADD OBJECT %s\n"), serviceIdentifier);
    HRESULT hr = CScriptServiceInstance::CreateObject(this, serviceIdentifier, resourcesDir, pObject);
    if (FAILED(hr))
    {
      return hr;
    }
    m_Objects[serviceIdentifier] = pObject;
  }
  else
  {
    ATLTRACE(_T("FOUND OBJECT %s\n"), serviceIdentifier);
  }
  return pObject->QueryInterface<IUnknown>(ppUnk);
}

