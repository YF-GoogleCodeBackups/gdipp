#include "stdafx.h"
#include <gdipp_common.h>

int APIENTRY wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    lpCmdLine,
	int       nCmdShow)
{
	BOOL b_ret;
	NTSTATUS eh_error;

	if (lpCmdLine == NULL || wcslen(lpCmdLine) == 0)
	{
		MessageBox(NULL, TEXT("Drag an exe file to me and I will load it with gdimm.dll."), TEXT("gdipp Loader"), MB_OK | MB_ICONINFORMATION);
		return EXIT_SUCCESS;
	}

	int argc;
	LPWSTR *argv = CommandLineToArgvW(lpCmdLine, &argc);
	assert(argv != NULL);

	WCHAR working_dir[MAX_PATH];
	wcsncpy_s(working_dir, argv[0], MAX_PATH);
	b_ret = PathRemoveFileSpecW(working_dir);
	assert(b_ret);

	STARTUPINFO si = {0};
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	
	b_ret = CreateProcessW(argv[0], lpCmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, working_dir, &si, &pi);
	if (!b_ret)
	{
		MessageBoxW(NULL, L"Unable to create the target process.", L"gdipp Loader", MB_OK | MB_ICONERROR);
		LocalFree(argv);
		return EXIT_FAILURE;
	}

	const inject_payload payload = {GDIPP_LOADER, NULL};

#ifdef _M_X64
	WCHAR gdimm_path_64[MAX_PATH];
	get_dir_file_path(NULL, L"gdimm_64.dll", gdimm_path_64);
	eh_error = RhInjectLibrary(pi.dwProcessId, pi.dwThreadId, EASYHOOK_INJECT_DEFAULT, NULL, gdimm_path_64, (PVOID) &payload, sizeof(inject_payload));
#else
	WCHAR gdimm_path_32[MAX_PATH];
	get_dir_file_path(NULL, L"gdimm_32.dll", gdimm_path_32);
	eh_error = RhInjectLibrary(pi.dwProcessId, pi.dwThreadId, EASYHOOK_INJECT_DEFAULT, gdimm_path_32, NULL, (PVOID) &payload, sizeof(inject_payload));
#endif // _M_X64

	if (eh_error != 0)
	{
		b_ret = TerminateProcess(pi.hProcess, 0);
		assert(b_ret);

		wstring error_msg = L"Unable to inject gdimm.dll to the new process";

		// STATUS_WOW_ASSERTION
		if (eh_error == (NTSTATUS) 0xC0009898L)
			error_msg += L" due to different bitness. Try the other gdipp Loader";

		error_msg += L".";

		MessageBoxW(NULL, error_msg.c_str(), L"gdipp Loader", MB_OK | MB_ICONERROR);
	}

	LocalFree(argv);

	return EXIT_SUCCESS;
}
