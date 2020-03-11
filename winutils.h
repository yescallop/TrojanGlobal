#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <WinInet.h>

void killProcessByName(const wchar_t* filename);

BOOL refreshOptions(INTERNET_PER_CONN_OPTION_LIST list);

BOOL setProxy(LPWSTR server, LPWSTR bypass);

BOOL resetProxy();