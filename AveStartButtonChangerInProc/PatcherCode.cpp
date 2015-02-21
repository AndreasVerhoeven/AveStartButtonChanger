// Under Windows 7, the start button is a hardcoded resource in explorer.exe loaded using the
// standard LoadImage() function. So, our idea is to patch LoadImage to intercept LoadImage() requests
// for the start button and to actually load and return a different image. 
// As a bonus, this code will will work for other resources in explorer.exe or other dlls in the same process
// as well.
//
// This is the code that actually patches explorer to use our custom LoadImageW function by doing
// a pretty default IAT patch on explorer.exe and comctl32.dll (used by lots of default components).
//
// Our custom LoadImage function, called ThunkedLoadImageW(), actually tries
// to load images from <currentThemeDir>/<module_with_resource>/<requested_resource>.png/.ico/.cur
//
// author: ave <ave@vistastylebuilder.com>
// Copyright Andreas Verhoeven, 2009. All rights reserved.
// This code cannot be used in any form without explicit written approval of Andreas Verhoeven.

#include "stdafx.h"


// ============== PER PROCESS VARIABLES =============
typedef HANDLE (__stdcall *LoadImageWProc)(HINSTANCE, LPCWSTR, UINT, int, int ,UINT);
static PVOID origProcApp = NULL;
static PVOID origProcComctl32 = NULL;
static PVOID origProcExplorerFrame = NULL;
static BOOL didInitGdiplus = FALSE;

static GdiplusStartupInput gdiplusStartupInput;
static ULONG_PTR pGdiToken = 0;

BOOL didHook = FALSE;

#pragma data_seg(".AVESHELLHOOK")
BOOL doCustomResourceLookup = TRUE;
#pragma data_seg()
#pragma comment(linker, "/section:.AVESHELLHOOK,rws")

void __stdcall SetPatcherEnabledStatus(BOOL enabled)
{
	doCustomResourceLookup = enabled;	
}

BOOL __stdcall GetPatcherEnabledStatus()
{
	return doCustomResourceLookup;
}

// ========= FORWARD DECLARES =========

void initHooks();
void uninitHooks();
static HANDLE __stdcall ThunkedLoadImageW(__in_opt HINSTANCE hInst, __in LPCWSTR name, __in UINT type, __in int cx, __in int cy, __in UINT fuLoad);
HRESULT PatchIat(__in HMODULE Module,__in LPCSTR ImportedModuleName,__in LPCSTR ImportedProcName,__in PVOID AlternateProc, __out_opt PVOID *OldProc);
void Reload7StartButton();
void ForceHookUpdate();


// ================== UTILITY STUFF =====================
const WCHAR* findFileName(const WCHAR* path)
{
	const WCHAR* ptr = path;
	int len = lstrlen(path);
	ptr += len;

	while(ptr > path)
	{
		ptr -= 1;
		if(*ptr == L'/' || *ptr  == L'\\')
			return ptr + 1;
	}

	return ptr;
}

// ===================== WIN7 STUFF =========================

void PostThemeChangedMessage(HWND hwnd)
{
	if(hwnd != NULL)
	{
		PostMessage(hwnd, WM_THEMECHANGED, 0, 0);
	}
}

BOOL CALLBACK EnumThreadWndProc(HWND hwnd,LPARAM lParam)
{
	PostThemeChangedMessage(hwnd);
	return TRUE;
}

void Reload7StartButton()
{
	// the start button and most other components reload their (hardcoded) graphics
	// on WM_THEMECHANGED, so we fake one...
	// We only do this when we are loaded into explorer, to avoid unnessary reloads.

	WCHAR processPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, processPath, _countof(processPath));
	const WCHAR* processFileName = findFileName(processPath);
	if(lstrcmpi(processFileName, L"explorer.exe") == 0)
	{
		HWND startWindow = FindWindow(L"Button", L"Start");
		if(startWindow != NULL)
		{
			PostMessage(startWindow, WM_THEMECHANGED, 0, 0);

			// update all other windows in this UI thread as well
			DWORD threadId = GetWindowThreadProcessId(startWindow, NULL);
			EnumThreadWindows(threadId, EnumThreadWndProc, 0L);
		}
	}
}


void ForceHookUpdate()
{
	// force explorer hooking if we haven't done so yet
	HWND startWindow = FindWindow(L"Button", L"Start");
	if(startWindow != NULL)
		SendMessage(startWindow, WM_NULL, 0, 0);
}

// ===================== PROCESS INIT CODE STUFF =====================
void initHooks()
{
	if(didHook)
		return;

	HRESULT hRes = S_OK;
	PatchIat(GetModuleHandle(NULL), "user32.dll", "LoadImageW", ThunkedLoadImageW, (PVOID*)&origProcApp);
	PatchIat(GetModuleHandle(L"comctl32.dll"), "user32.dll", "LoadImageW", ThunkedLoadImageW, (PVOID*)&origProcComctl32);
	PatchIat(GetModuleHandle(L"dui70.dll"), "user32.dll", "LoadImageW", ThunkedLoadImageW, (PVOID*)&origProcExplorerFrame);

	Reload7StartButton();

	didHook = TRUE;
}

void uninitHooks()
{
	if(!didHook)
		return;

	if(origProcApp != NULL)
		PatchIat(GetModuleHandle(NULL), "user32.dll", "LoadImageW", origProcApp, NULL);

	if(origProcComctl32)
		PatchIat(GetModuleHandle(L"comctl32.dll"), "user32.dll", "LoadImageW", origProcComctl32, NULL);

	if(origProcExplorerFrame)
		PatchIat(GetModuleHandle(L"dui70.dll"), "user32.dll", "LoadImageW", origProcExplorerFrame, NULL);

	Reload7StartButton();

	didHook = FALSE;
}

// ===================== GDI+ STUFF =====================
void initGdiplus()
{
	didInitGdiplus = TRUE;

	GdiplusStartup(&pGdiToken,&gdiplusStartupInput,NULL); 
}

void uninitGdiplus()
{
	GdiplusShutdown(pGdiToken);
	didInitGdiplus = FALSE;
}

void ensureGdiplus()
{
	if(!didInitGdiplus)
		initGdiplus();
}


// ===================== RESOURCE LOADING REDIRECTION STUFF =====================

#define PtrFromRva( base, rva ) ( ( ( PBYTE ) base ) + rva )

HRESULT PatchIat(
  __in HMODULE Module,
  __in LPCSTR ImportedModuleName,
  __in LPCSTR ImportedProcName,
  __in PVOID AlternateProc,
  __out_opt PVOID *OldProc
  )
{
  PIMAGE_DOS_HEADER DosHeader = ( PIMAGE_DOS_HEADER ) Module;
  PIMAGE_NT_HEADERS NtHeader;
  PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
  UINT Index;

  NtHeader = ( PIMAGE_NT_HEADERS )
    PtrFromRva( DosHeader, DosHeader->e_lfanew );
  if( IMAGE_NT_SIGNATURE != NtHeader->Signature )
  {
    return HRESULT_FROM_WIN32( ERROR_BAD_EXE_FORMAT );
  }

  ImportDescriptor = ( PIMAGE_IMPORT_DESCRIPTOR )
    PtrFromRva( DosHeader,
      NtHeader->OptionalHeader.DataDirectory
        [ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress );

  //
  // Iterate over import descriptors/DLLs.
  //
  for ( Index = 0;
        ImportDescriptor[ Index ].Characteristics != 0;
        Index++ )
  {
    PSTR dllName = ( PSTR )
      PtrFromRva( DosHeader, ImportDescriptor[ Index ].Name );

    if ( 0 == _strcmpi( dllName, ImportedModuleName ) )
    {
      //
      // This the DLL we are after.
      //
      PIMAGE_THUNK_DATA Thunk;
      PIMAGE_THUNK_DATA OrigThunk;

      if ( ! ImportDescriptor[ Index ].FirstThunk ||
         ! ImportDescriptor[ Index ].OriginalFirstThunk )
      {
        return E_INVALIDARG;
      }

      Thunk = ( PIMAGE_THUNK_DATA )
        PtrFromRva( DosHeader,
          ImportDescriptor[ Index ].FirstThunk );
      OrigThunk = ( PIMAGE_THUNK_DATA )
        PtrFromRva( DosHeader,
          ImportDescriptor[ Index ].OriginalFirstThunk );

      for ( ; OrigThunk->u1.Function != NULL;
              OrigThunk++, Thunk++ )
      {
        if ( OrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
        {
          //
          // Ordinal import - we can handle named imports
          // ony, so skip it.
          //
          continue;
        }

        PIMAGE_IMPORT_BY_NAME import = ( PIMAGE_IMPORT_BY_NAME )
          PtrFromRva( DosHeader, OrigThunk->u1.AddressOfData );

        if ( 0 == strcmp( ImportedProcName,
                              ( char* ) import->Name ) )
        {
          //
          // Proc found, patch it.
          //
          DWORD junk;
          MEMORY_BASIC_INFORMATION thunkMemInfo;

          //
          // Make page writable.
          //
          VirtualQuery(
            Thunk,
            &thunkMemInfo,
            sizeof( MEMORY_BASIC_INFORMATION ) );
          if ( ! VirtualProtect(
            thunkMemInfo.BaseAddress,
            thunkMemInfo.RegionSize,
            PAGE_EXECUTE_READWRITE,
            &thunkMemInfo.Protect ) )
          {
            return HRESULT_FROM_WIN32( GetLastError() );
          }

          //
          // Replace function pointers (non-atomically).
          //
          if ( OldProc )
          {
            *OldProc = ( PVOID ) ( DWORD_PTR )
                Thunk->u1.Function;
          }
#ifdef _WIN64
		  //InterlockedExchange64(&Thunk->u1.Function, AlternateProc);

          Thunk->u1.Function = ( ULONGLONG ) ( DWORD_PTR ) AlternateProc;
#else
		  //InterlockedExchangePointer(&Thunk->u1.Function, AlternateProc);

          Thunk->u1.Function = ( DWORD ) ( DWORD_PTR )  AlternateProc;
#endif
          //
          // Restore page protection.
          //
          if ( ! VirtualProtect(
            thunkMemInfo.BaseAddress,
            thunkMemInfo.RegionSize,
            thunkMemInfo.Protect,
            &junk ) )
          {
            return HRESULT_FROM_WIN32( GetLastError() );
          }

          return S_OK;
        }
      }

      //
      // Import not found.
      //
      return HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
    }
  }

  //
  // DLL not found.
  //
  return HRESULT_FROM_WIN32( ERROR_MOD_NOT_FOUND );
}

HBITMAP aveLoadBitmap(const WCHAR* filename)
{
	ensureGdiplus();

	Bitmap source(filename);
	HBITMAP hBmp = NULL;
	source.GetHBITMAP(NULL, &hBmp);
	return hBmp;
}


HANDLE __stdcall ThunkedLoadImageW(__in_opt HINSTANCE hInst, __in LPCWSTR name, __in UINT type, __in int cx, __in int cy, __in UINT fuLoad)
{
	if(NULL == hInst || !doCustomResourceLookup)
	{
		return LoadImageW(hInst, name, type, cx, cy, fuLoad);			
	}

	WCHAR handlePath[MAX_PATH] = {0};
	GetModuleFileName(hInst, handlePath, MAX_PATH);
	WCHAR* handleFileName = PathFindFileName(handlePath);

	// we build the search path as <module_with_requested_resource>/<resource_name/id>.png/.ico/.cur
	// We actually re-route requested bitmaps to pngs, just for ease of use (no need for pre-multiplied ARGB bmps).
	WCHAR searchPathSuffix[MAX_PATH*3] = {0};
	wcscpy_s(searchPathSuffix, _countof(searchPathSuffix), handleFileName);
	if(IS_INTRESOURCE(name))
	{
		WORD id = (WORD)(name);
		WCHAR buf[100] = {0};
		_itow_s(id, buf, _countof(buf), 10);
		PathAppend(searchPathSuffix, buf);
	}
	else
	{
		PathAppend(searchPathSuffix, name);
	}

	if(type == IMAGE_BITMAP)	wcscat_s(searchPathSuffix, _countof(searchPathSuffix), L".png");
	else if(type == IMAGE_ICON)	wcscat_s(searchPathSuffix, _countof(searchPathSuffix), L".ico");
	else if(type == IMAGE_CURSOR)wcscat_s(searchPathSuffix, _countof(searchPathSuffix), L".cur");

	// the search path is current_theme dir + search path
	WCHAR searchPath[MAX_PATH] = {0};
	GetCurrentThemeName(searchPath, _countof(searchPath), NULL, 0, NULL, 0);
	PathRemoveFileSpec(searchPath);
	PathAppend(searchPath, searchPathSuffix);

	if(!PathFileExists(searchPath))
	{
		return LoadImageW(hInst, name, type, cx, cy, fuLoad);
	}
	else
	{
		if(IMAGE_BITMAP == type)
			return aveLoadBitmap(searchPath);
		else
			return LoadImageW(NULL, searchPath, type, cx, cy, fuLoad | LR_LOADFROMFILE);
	}
}


