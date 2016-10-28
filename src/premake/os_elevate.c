#include <stdbool.h>
#include "premake.h"

#if PLATFORM_WINDOWS
#include <shellapi.h>
static int do_elevate(void)
{
	BOOL is_elevated = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			is_elevated = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	if (is_elevated) {
		return 1;
	}

	char szPath[MAX_PATH];
	if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath))) {
		char *params = GetCommandLine();
		while (*params != '\0') {
			if (*params == ' ') {
				params++;
				break;
			}
			params++;
		}

		SHELLEXECUTEINFO sei = { 0 };
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.lpVerb = "runas";
		sei.lpFile = szPath;
		sei.lpParameters = params;
		sei.nShow = SW_NORMAL;

		if (ShellExecuteEx(&sei)) {
			exit(0);
		}
	}
	return 0;
}
#else
static int do_elevate(void)
{
	return 0;
}
#endif

int os_elevate(lua_State* L)
{
	lua_pushboolean(L, do_elevate());
	return 1;
}
