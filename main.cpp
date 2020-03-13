#include "winutils.h"
#include <stdio.h>

const wchar_t* SERVER = L"http://127.0.0.1:1084";
const wchar_t* BYPASS = L"localhost;127.*;10.*;172.16.*;172.17.*;172.18.*;172.19.*;172.20.*;172.21.*;172.22.*;172.23.*;172.24.*;172.25.*;172.26.*;172.27.*;172.28.*;172.29.*;172.30.*;172.31.*;172.32.*;192.168.*;<local>";

HANDLE handles[2] = {};
BOOL reseted = FALSE;

BOOL WINAPI ctrlHandler(DWORD dwCtrlType) {
	if (!reseted) {
		resetProxy();
		reseted = TRUE;
	}
	return TRUE;
}

int main() {
	HANDLE hJob = CreateJobObject(nullptr, nullptr);
	if (hJob == nullptr) {
		printError("CreateJobObject");
		return 1;
	}

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {};
	jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) {
		printError("SetInformationJobObject");
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
		printError("Starting trojan");
		return 1;
	}
	CloseHandle(pi.hThread);
	if (!AssignProcessToJobObject(hJob, pi.hProcess)) {
		printError("AssignProcessToJobObject");
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
		printError("Starting Privoxy");
		return 1;
	}
	CloseHandle(pi.hThread);
	if (!AssignProcessToJobObject(hJob, pi.hProcess)) {
		printError("AssignProcessToJobObject");
		return 1;
	}
	handles[1] = pi.hProcess;

	// Set global proxy
	if (!setProxy(SERVER, BYPASS)) {
		printError("setProxy");
		return 1;
	}

	// Set console ctrl handler
	if (!SetConsoleCtrlHandler(ctrlHandler, TRUE)) {
		printError("SetConsoleCtrlHandler");
		return 1;
	}

	WaitForMultipleObjects(2, handles, FALSE, INFINITE);
	if (!reseted) {
		resetProxy();
		reseted = TRUE;
	}
	return 0;
}