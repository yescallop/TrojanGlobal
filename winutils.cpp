#include "winutils.h"
#include <TlHelp32.h>
#include <WinInet.h>
#include <stdio.h>

void killProcessByName(const wchar_t* filename) {
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE) return;
	PROCESSENTRY32 pEntry = {};
	pEntry.dwSize = sizeof(pEntry);
	if (!Process32First(hSnapShot, &pEntry)) return;
	do {
		if (wcscmp(pEntry.szExeFile, filename) == 0) {
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE,
				pEntry.th32ProcessID);
			if (hProcess != nullptr) {
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
	} while (Process32Next(hSnapShot, &pEntry));
	CloseHandle(hSnapShot);
}

void printError(const char* what) {
	DWORD errorId = GetLastError();
	LPWSTR lpBuffer = nullptr;
	if (errorId == 0 ||
		!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, errorId, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPWSTR)&lpBuffer, 0, nullptr)) {
		printf("%s failed: No message available", what);
	}
	else {
		printf("%s failed: %S", what, lpBuffer);
		LocalFree(lpBuffer);
	}
}

BOOL refreshOptions(INTERNET_PER_CONN_OPTION_LIST list) {
	return InternetSetOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, sizeof(list)) &&
		InternetSetOption(nullptr, INTERNET_OPTION_PROXY_SETTINGS_CHANGED, nullptr, 0) &&
		InternetSetOption(nullptr, INTERNET_OPTION_REFRESH, nullptr, 0);
}

BOOL setProxy(const wchar_t* server, const wchar_t* bypass) {
	INTERNET_PER_CONN_OPTION_LIST list = {};

	// Fill the list structure.
	list.dwSize = sizeof(list);

	// nullptr == LAN, otherwise connectoid name.
	list.pszConnection = nullptr;

	// Set three options.
	list.dwOptionCount = 3;
	INTERNET_PER_CONN_OPTION options[3] = {};
	list.pOptions = options;

	// Set flags.
	options[0].dwOption = INTERNET_PER_CONN_FLAGS;
	options[0].Value.dwValue = PROXY_TYPE_DIRECT |
		PROXY_TYPE_PROXY;

	// Set proxy name.
	options[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	options[1].Value.pszValue = const_cast<wchar_t*>(server);

	// Set proxy override.
	options[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	options[2].Value.pszValue = const_cast<wchar_t*>(bypass);

	// Set the options on the connection.
	return refreshOptions(list);
}

BOOL resetProxy() {
	INTERNET_PER_CONN_OPTION_LIST list = {};
	list.dwSize = sizeof(list);
	list.pszConnection = nullptr;
	list.dwOptionCount = 1;
	INTERNET_PER_CONN_OPTION option = {};
	list.pOptions = &option;

	option.dwOption = INTERNET_PER_CONN_FLAGS;
	option.Value.dwValue = PROXY_TYPE_DIRECT;

	return refreshOptions(list);
}