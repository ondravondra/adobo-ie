#pragma once

#include <string>

#define __WSTR(s) L ## s
#define _WSTR(s) __WSTR(s)

class CSalsitaExtensionHelper {
private:
  CSalsitaExtensionHelper() {}

public:
  /**
   * Makes sure that resourcesDir ends with a backslash.
   */
  static void ResourcesDirNormalize(std::wstring &resourcesDir);

  /**
   * Reads path to the directory containing web resources from registry.
   * Calls ResourcesDirNormalize on the result.
   * The value must be of REG_SZ type.
   */
  static bool ResourcesDirReadFromRegistry(HKEY hKey, const wchar_t *subKey, const wchar_t *valueName, std::wstring &result);

  /**
   * In the debugging version the resources directory is a path to the source tree specified by preprocessor constant passed from cmake.
   * This function canonicalizes it and calls ResourcesDirNormalize on the result.
   * Use _WSTR macro to convert preprocessor constant string to a widechar string to pass to this function.
   */
  static void ResourcesDirGetDebugPath(const wchar_t *preprocessorDefinedPath, std::wstring &result);

  /**
   * Converts relative url to file:/// url pointing to resources directory.
   */
  static void ResourcesDirMakeUrl(const wchar_t *resourcesDir, const wchar_t *relativeUrl, std::wstring &pageUrl);

  /**
   * Checks if document.documentElement is not null.
   * When the user clicks a link on google search page it sends him to a redirect page which doesn't contain documentElement.
   * Scripts relying on this property then crash.
   */
  static bool CheckDocumentElementAvailable(IWebBrowser2 *browser);
};
