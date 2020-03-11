#pragma once
#include <Windows.h>
#include <WinInet.h>

BOOL refreshOptions(INTERNET_PER_CONN_OPTION_LIST list) {
	return InternetSetOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, sizeof(list)) &
		InternetSetOption(nullptr, INTERNET_OPTION_PROXY_SETTINGS_CHANGED, nullptr, 0) &
		InternetSetOption(nullptr, INTERNET_OPTION_REFRESH, nullptr, 0);
}

BOOL setProxy(LPWSTR server, LPWSTR bypass) {
	INTERNET_PER_CONN_OPTION_LIST list = {};

	// Fill the list structure.
	list.dwSize = sizeof(list);

	// NULL == LAN, otherwise connectoid name.
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
	options[1].Value.pszValue = server;

	// Set proxy override.
	options[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
	options[2].Value.pszValue = bypass;

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
