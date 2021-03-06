#include "stdafx.h"
#include "SalsitaExtensionHelper.h"
#include <algorithm>

void CSalsitaExtensionHelper::ResourcesDirNormalize(std::wstring &resourcesDir)
{
  if (resourcesDir.size() && resourcesDir.at(resourcesDir.size()-1) != L'\\')
  {
    resourcesDir.append(L"\\");
  }
}

bool CSalsitaExtensionHelper::ResourcesDirReadFromRegistry(HKEY hKey, const wchar_t *subKey, const wchar_t *valueName, std::wstring &result)
{
  bool ret = false;
  HKEY hkConfig;

  result.erase();
  if (RegOpenKeyExW(hKey, subKey, 0, KEY_READ, &hkConfig) == ERROR_SUCCESS)
  {
    DWORD valueType, valueLen = 0;
    if (RegQueryValueExW(hkConfig, valueName, NULL, &valueType, NULL, &valueLen) == ERROR_SUCCESS && valueType == REG_SZ)
    {
      BYTE *buf = new BYTE[valueLen];
      if (RegQueryValueExW(hkConfig, valueName, NULL, &valueType, buf, &valueLen) == ERROR_SUCCESS)
      {
        result.assign((const wchar_t *)buf);
        ResourcesDirNormalize(result);
        ret = true;
      }
      delete buf;
    }

    RegCloseKey(hkConfig);
  }

  return ret;
}

void CSalsitaExtensionHelper::ResourcesDirGetDebugPath(const wchar_t *preprocessorDefinedPath, std::wstring &result)
{
  result.assign(preprocessorDefinedPath);
  std::replace(result.begin(), result.end(), L'/', L'\\');

  std::wstring canonicalized;
  canonicalized.resize(MAX_PATH+1);
  PathCanonicalizeW((LPWSTR)canonicalized.c_str(), result.c_str());
  result.assign(canonicalized.c_str());
  ResourcesDirNormalize(result);
}

void CSalsitaExtensionHelper::ResourcesDirMakeUrl(const wchar_t *resourcesDir, const wchar_t *relativeUrl, std::wstring &pageUrl)
{
  pageUrl.assign(L"file:///");
  pageUrl.append(resourcesDir);
  ResourcesDirNormalize(pageUrl);
  pageUrl.append(relativeUrl);
  std::replace(pageUrl.begin(), pageUrl.end(), L'\\', L'/');
}

bool CSalsitaExtensionHelper::CheckDocumentElementAvailable(IWebBrowser2 *browser)
{
  CComPtr<IDispatch> pdispDocument;
  if (FAILED(browser->get_Document(&pdispDocument.p)))
  {
    return false;
  }

  CComQIPtr<IHTMLDocument3> document = pdispDocument;
  CComPtr<IHTMLElement> documentElement;
  if (!document || (FAILED(document->get_documentElement(&documentElement.p))) || !documentElement)
  {
    return false;
  }

  return true;
}
