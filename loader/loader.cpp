#undef UNICODE
#pragma comment(lib, "detours.lib")

#include <cstdio>
#include <windows.h>
#include <detours.h>

int main()
{
	// Process vars
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);

	// Retrieve path of lync.exe
	HKEY hKey;
	LSTATUS rv = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Lync.exe",
		NULL,
		KEY_READ,
		&hKey
	);
	if (rv != ERROR_SUCCESS)
		return -1;

	WCHAR LyncPath[MAX_PATH];
	DWORD cbSize = sizeof(LyncPath);
	RegQueryValueExW(
		hKey,
		NULL, // Default key
		NULL,
		NULL,
		(LPBYTE)LyncPath,
		&cbSize
	);

	RegCloseKey(hKey);

	// Start lync.exe with custom DLL
	char* DirPath = new char[MAX_PATH];
	char* DetourPath = new char[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, DirPath);
	sprintf_s(DetourPath, MAX_PATH, "%s\\detour.dll", DirPath);
	DetourCreateProcessWithDllExW(LyncPath,
								 NULL,
								 NULL,
								 NULL,
								 FALSE,
								 CREATE_DEFAULT_ERROR_MODE,
								 NULL,
								 NULL,
								 &si,
								 &pi,
								 DetourPath,
								 NULL);
	delete[] DirPath;
	delete[] DetourPath;

	return 0;
}