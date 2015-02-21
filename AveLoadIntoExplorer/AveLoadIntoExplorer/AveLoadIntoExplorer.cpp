
#include <windows.h>

BOOL LoadDllIntoProcess(HANDLE processHandle, const WCHAR* dllName)
{
	HMODULE kernel32 = GetModuleHandle( L"kernel32.dll" );

	void*   pLibRemote=NULL;   // The address (in the remote process) where 
							// szLibPath will be copied to;
	DWORD   hLibModule=0;   // Base address of loaded module (==HMODULE);

	SIZE_T writtenSize;
	SIZE_T len = (lstrlen(dllName)+1)*sizeof(WCHAR);
	pLibRemote = ::VirtualAllocEx(processHandle, NULL, len, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE );
	if(NULL == pLibRemote)
	{
		return FALSE;
	}

	BOOL bRes = ::WriteProcessMemory(processHandle, pLibRemote, (void*)dllName, len, &writtenSize );
	if(!bRes)
	{
		::VirtualFreeEx(processHandle, pLibRemote, 0, MEM_RELEASE);
		return FALSE;
	}

	//notice how the ThreadProc signature is the same as LoadLibrary, 
	//and how kernel32 is never relocated?
	HANDLE hThread = CreateRemoteThread( 
		processHandle,
		NULL, 
		0, 
		(LPTHREAD_START_ROUTINE)GetProcAddress( kernel32, "LoadLibraryW" ), 
		pLibRemote, 
		0, 
		0 );

	if(NULL == hThread)
	{
		::VirtualFreeEx(processHandle, pLibRemote, 0, MEM_RELEASE);
		return FALSE;
	}

	::WaitForSingleObject( hThread, INFINITE );

	// Get handle of the loaded module
	::GetExitCodeThread( hThread, &hLibModule );

	// Clean up
	::CloseHandle( hThread );
	::VirtualFreeEx(processHandle, pLibRemote, 0, MEM_RELEASE);

	return hLibModule != NULL;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if(argc > 1)
	{
		HWND tray = FindWindow(L"Shell_TrayWnd", NULL);
		if(NULL == tray)
			return EXIT_FAILURE;

		DWORD pid = 0;
		GetWindowThreadProcessId(tray, &pid);
		if(0 == pid)
			return EXIT_FAILURE;

		DWORD flags = PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD;
		HANDLE processHandle = OpenProcess(flags, FALSE, pid); 
		if(NULL == processHandle)
			return EXIT_FAILURE;

		BOOL dllLoadSucceeded = LoadDllIntoProcess(processHandle, argv[1]);
		CloseHandle(processHandle);

		return dllLoadSucceeded ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	return EXIT_FAILURE;
}