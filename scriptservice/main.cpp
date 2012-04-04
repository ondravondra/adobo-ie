
#include "stdafx.h"
#include "service.h"
#include "service_i.c"

class CScriptingSrvcModule : public CAtlExeModuleT<CScriptingSrvcModule>
{
};

CScriptingSrvcModule _AtlModule;

extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
  return _AtlModule.WinMain(nShowCmd);
}
