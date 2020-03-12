#pragma once
#include <Windows.h>

void killProcessByName(const wchar_t* filename);

void printError(const char* what);

BOOL setProxy(const wchar_t* server, const wchar_t* bypass);

BOOL resetProxy();