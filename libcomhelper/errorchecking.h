// some shortcut macros for error checking

#ifdef _DEBUG
//#define _DEBUG_BREAK
#endif

#ifdef _DEBUG_BREAK
#define ASSERT_ ATLASSERT
#else
#define ASSERT_
#endif

#define IF_FAILED_RET(_hr) \
  do \
  { \
  HRESULT _hr__ = _hr; \
  ASSERT_(SUCCEEDED(_hr__)); \
  if (FAILED(_hr__)) \
    { \
    ATLTRACE(_T("ASSERTION FAILED: 0x%08x in "), _hr); \
    ATLTRACE(__FILE__); \
    ATLTRACE(_T(" line %i\n"), __LINE__); \
    return _hr__; \
    } \
  } while(0);

#define IF_FAILED_RET2(_hr, _ret) \
  do \
  { \
  HRESULT _hr__ = _hr; \
  ASSERT_(SUCCEEDED(_hr__)); \
  if (FAILED(_hr__)) \
    { \
    ATLTRACE(_T("ASSERTION FAILED: 0x%08x in "), _hr); \
    ATLTRACE(__FILE__); \
    ATLTRACE(_T(" line %i\n"), __LINE__); \
    return _ret; \
    } \
  } while(0);

#define ENSURE_RETVAL(_val) \
  if (!_val) return E_POINTER;
