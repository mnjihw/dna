#define _CRT_SECURE_NO_WARNINGS
#define NUMBEROF_USED_FUNCTIONS 1
#include <stdio.h>
#include <Windows.h>

typedef struct
{
	FARPROC pFunc[2];
	char dll_name[NUMBEROF_USED_FUNCTIONS][128];
	char func_name[NUMBEROF_USED_FUNCTIONS][128];
	/***** ↓parameters of the functions ↓*****/
	char buf[128];
	/***** ↑                            ↑*****/
}THREAD_PARAM, *PTHREAD_PARAM;

DWORD WINAPI ThreadProc(LPVOID arg)
{
	PTHREAD_PARAM param = (PTHREAD_PARAM)arg;
	FARPROC func[NUMBEROF_USED_FUNCTIONS];

	
	func[0] = (FARPROC)param->pFunc[1](param->pFunc[0](param->dll_name[0]), param->func_name[0]);
	((int (WINAPI*)(HWND, LPCSTR, LPCSTR, UINT))func[0])(0, param->buf, 0, 0);

	return 0;
}

DWORD InjectCode(LPCSTR lpWinTitle)
{
	HWND hWnd;
	THREAD_PARAM param = { 0, };
	HANDLE hProcess, hThread;
	LPVOID code_buf, param_buf;
	DWORD code_buf_size, pid;

	if (!(hWnd = FindWindow(NULL, lpWinTitle)))
	{
		printf("윈도우 찾지 못함!");
		return FALSE;

	}
	GetWindowThreadProcessId(hWnd, &pid);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	code_buf_size = (DWORD)InjectCode - (DWORD)ThreadProc;
	code_buf = VirtualAllocEx(hProcess, NULL, code_buf_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, code_buf, ThreadProc, code_buf_size, NULL);
	param.pFunc[0] = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	param.pFunc[1] = GetProcAddress;
	strcpy(param.dll_name[0], "user32.dll");
	strcpy(param.func_name[0], "MessageBoxA");
	/***** ↓sets parameters of the functions ↓*****/
	strcpy(param.buf, "zzz");
	/***** ↑                                 ↑*****/

	param_buf = VirtualAllocEx(hProcess, NULL, sizeof(param), MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, param_buf, &param, sizeof(param), NULL);
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)code_buf, param_buf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, code_buf, 0, MEM_RELEASE);
	VirtualFreeEx(hProcess, param_buf, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	return TRUE;
}

void main()
{	
	InjectCode("리듬 dll 인젝터");
}