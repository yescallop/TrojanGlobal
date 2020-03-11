#include "proxy.h"
#include "killprocess.h"
#include <stdio.h>

const LPWSTR SERVER = (LPWSTR)L"http://127.0.0.1:1084";
const LPWSTR BYPASS_LIST = (LPWSTR)L"localhost;127.*;10.*;172.16.*;172.17.*;172.18.*;172.19.*;172.20.*;172.21.*;172.22.*;172.23.*;172.24.*;172.25.*;172.26.*;172.27.*;172.28.*;172.29.*;172.30.*;172.31.*;172.32.*;192.168.*;<local>";

HANDLE handles[2] = {};
BOOL reseted = FALSE;

BOOL WINAPI ctrlHandler(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_CLOSE_EVENT || dwCtrlType == CTRL_C_EVENT) {
		if (!reseted) {
			resetProxy();
			reseted = TRUE;
		}
		return TRUE;
	}
	return FALSE;
}

int main() {
	HANDLE hJob = CreateJobObject(nullptr, nullptr);
	if (hJob == nullptr) {
		printf("CreateJobObject failed: error %d\n", GetLastError());
		return 1;
	}

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {};
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) {
		printf("SetInformationJobObject failed: error %d\n", GetLastError());
		return 1;
	}

	STARTUPINFO si = {};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {};
	// Start trojan
	if (!CreateProcess(
		L"trojan.exe", nullptr,
		nullptr, nullptr, FALSE,
		0, nullptr, nullptr,
		&si, &pi
	)) {
		printf("CreateProcess failed: error %d\n", GetLastError());
		return 1;
	}
	CloseHandle(pi.hThread);
	if (!AssignProcessToJobObject(hJob, pi.hProcess)) {
		printf("AssignProcessToJobObject failed: error %d\n", GetLastError());
		return 1;
	}
	handles[0] = pi.hProcess;

	// Kill Privoxy
	killProcessByName(L"tj_privoxy.exe");
	// Start Privoxy
	si.wShowWindow = FALSE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	if (!CreateProcess(
		L"tj_privoxy.exe", nullptr,
		nullptr, nullptr, FALSE,
		0, nullptr, nullptr,
		&si, &pi
	)) {
		printf("CreateProcess failed: error %d\n", GetLastError());
		return 1;
	}
	CloseHandle(pi.hThread);
	if (!AssignProcessToJobObject(hJob, pi.hProcess)) {
		printf("AssignProcessToJobObject failed: error %d\n", GetLastError());
		return 1;
	}
	handles[1] = pi.hProcess;

	// Set global proxy
	if (!setProxy(SERVER, BYPASS_LIST)) {
		printf("setProxy failed: error %d\n", GetLastError());
		return 1;
	}

	// Set console ctrl handler
	if (!SetConsoleCtrlHandler(ctrlHandler, TRUE)) {
		printf("SetConsoleCtrlHandler failed: error %d\n", GetLastError());
		return 1;
	}

	WaitForMultipleObjects(2, handles, FALSE, INFINITE);
	if (!reseted) {
		resetProxy();
		reseted = TRUE;
	}
	return 0;
}
