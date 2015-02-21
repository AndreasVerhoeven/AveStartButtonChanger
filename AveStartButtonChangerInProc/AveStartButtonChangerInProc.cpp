// To get ourselves loaded into explorer.exe to do start the PatcherCode,
// we registered as a COM library with a dummy object to be able to be registered
// under the shared task-scheduler, which are COM objects loaded by explorer on start:
// This makes sure we are always only loaded in-proc in explorer on the UI thread, as well
// as being reloaded when explorer.exe is relaunched as shell only.
//
// The code here below is the COM bootstrap code, which is modified to only work
// in explorer.exe (and verclsid.exe, a mandatory helper library of explorer) and
// will launch the PatcherCode when we are actually loaded into explorer. To make
// sure this DLL is not unloaded (and thus crash explorer when the patching code
// is also unloaded), we up the reference count of the DLL by LoadLibrary()ing ourselves
// and by always returning S_FALSE on DllCanUnloadNow().
//
// Explorer SharedTaskScheduler registry stuff (register a CLSID of an object in this lib):
//		[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\explorer\SharedTaskScheduler]
//		"{F791A188-699D-4FD4-955A-EB59E89B1907}"="Ave's 7StartButton Changer"
//
//
// author: ave <ave@vistastylebuilder.com>
// Copyright Andreas Verhoeven, 2009. All rights reserved.
// This code cannot be used in any form without explicit written approval of Andreas Verhoeven.

#include "stdafx.h"
#include "resource.h"
#include "AveStartButtonChangerInProc.h"
#include "dlldatax.h"

BOOL inline IsRunningInExplorer()
{
	WCHAR path[MAX_PATH] = {0};
	GetModuleFileName(NULL, path, MAX_PATH);
	return wcsstr(path, L"explorer.exe") != NULL || wcsstr(path, L"verclsid.exe") != NULL;
	//const WCHAR* fileName = PathFindFileName(path);
	//return _wcsicmp(fileName, L"explorer.exe") == 0 || _wcsicmp(fileName, L"verclsid.exe") == 0;
}

class CAveStartButtonChangerInProcModule : public CAtlDllModuleT< CAveStartButtonChangerInProcModule >
{
public :
	DECLARE_LIBID(LIBID_AveStartButtonChangerInProcLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_AVESTARTBUTTONCHANGERINPROC, "{508AE86B-33BB-4F20-A7F1-7AC86F5DD375}")
};

CAveStartButtonChangerInProcModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

extern void initHooks();
extern void uninitHooks();

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if(DLL_PROCESS_ATTACH == dwReason)
	{
		if(IsRunningInExplorer())
		{
			initHooks();
		}
	}
	else if(DLL_PROCESS_DETACH == dwReason)
	{
		if(IsRunningInExplorer())
		{
			uninitHooks();
		}
	}

#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

void UpDllRefCount()
{
	WCHAR path[MAX_PATH] = {0};
	GetModuleFileName(HINST_THISCOMPONENT, path, _countof(path));
	LoadLibrary(path);
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
	return S_FALSE;

#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (hr != S_OK)
        return hr;
#endif
    return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	if(!IsRunningInExplorer())
	{
		return E_FAIL;
	}

	UpDllRefCount();

	return E_FAIL;
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
}

