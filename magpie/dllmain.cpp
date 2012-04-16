/****************************************************************************
 * dllmain.cpp : Implementation of DllMain.
 * Copyright 2012 Salsita Software.
 * Author: Arne Seib <kontakt@seiberspace.de>
 ****************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "Magpie.h"
#include "dllmain.h"

CMagpieAtlDllModule _AtlModule;

// required by ActiveScriptT.h
CLSID CLSID_JScript = {0xf414c260,0x6ac0,0x11cf,{0xb6,0xd1,0x00,0xaa,0x00,0xbb,0xbb,0x58}};

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
