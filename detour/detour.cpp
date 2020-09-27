#undef UNICODE
#pragma comment(lib, "detours.lib")

#include <cstdio>
#include <windows.h>
#include <detours.h>

const LPCWSTR szFakeKey = L"Software\\CustomPolicies\\Microsoft\\Office\\16.0\\Lync";

// Detoured function
LSTATUS (WINAPI* pRegOpenKeyExW)(
	HKEY    hKey,
	LPCWSTR lpSubKey,
	DWORD   ulOptions,
	REGSAM  samDesired,
	PHKEY   phkResult
) = RegOpenKeyExW;

// Map HKLM to HKCU for specific key
LSTATUS WINAPI MyRegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	if (hKey == HKEY_LOCAL_MACHINE &&
		lpSubKey && !wcscmp(lpSubKey, L"Software\\Policies\\Microsoft\\Office\\16.0\\Lync"))
	{
		hKey = HKEY_CURRENT_USER;
		lpSubKey = szFakeKey;
		OutputDebugString("RegOpenKeyExW changed!");
	}
	return pRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
};

// Create "EnableAppearOffline" in user-writable location
bool CreateCustomKey() {
	HKEY hKey;
	if (RegCreateKeyExW(
		HKEY_CURRENT_USER,
		szFakeKey,
		NULL,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_SET_VALUE,
		NULL,
		&hKey,
		NULL
	) != ERROR_SUCCESS)
	{
		return false;
	}

	DWORD val = 0x1;
	if (RegSetValueExW(
		hKey,
		L"EnableAppearOffline",
		NULL,
		REG_DWORD,
		(const BYTE*)&val,
		sizeof(val)
	) != ERROR_SUCCESS)
	{
		return false;
	}

	return RegCloseKey(hKey);
}

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		// Attach detour
		DisableThreadLibraryCalls(hDLL);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)pRegOpenKeyExW, MyRegOpenKeyExW);
		if (DetourTransactionCommit() == NO_ERROR)
			OutputDebugString("RegOpenKeyExW() detoured successfully\n");

		// Write custom key
		if (CreateCustomKey())
			return FALSE;

		break;
	case DLL_PROCESS_DETACH:
		// Detach detour
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)pRegOpenKeyExW, MyRegOpenKeyExW);
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
