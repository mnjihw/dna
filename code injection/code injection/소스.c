#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ShlObj.h>
#include <Windows.h>


typedef struct INJECT
{
	char funcname[64];
	char dllname[64];
	char buf[128];
	
}INJECT;

DWORD WINAPI Injection_ThreadProc(INJECT *inject_thread)
{
	PROC pFunc;
	printf("시작\n");
	pFunc = GetProcAddress(GetModuleHandle(inject_thread->dllname), inject_thread->funcname);
	printf("%X %Xㅋㅋ\n", pFunc, MessageBoxA);
	pFunc(0, inject_thread->buf, 0, 0);
	
	return 0;
}

void afterFunc()
{

}

BOOL Injection_Thread(DWORD pid)
{
	HANDLE hProcess, hThread;
	LPVOID addr, data_addr;
	INJECT inject;
	
	
	DWORD size = (DWORD)afterFunc - (DWORD)Injection_ThreadProc;
	
	strcpy(inject.buf, "ㅎㅇㅋㅋ");
	strcpy(inject.dllname, "user32.dll");
	strcpy(inject.funcname, "MessageBoxA");
	
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

	data_addr = VirtualAllocEx(hProcess, NULL, sizeof(INJECT), MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(hProcess, data_addr, (LPCVOID)&inject, sizeof(INJECT), NULL);

	addr = VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	WriteProcessMemory(hProcess, addr, (LPCVOID)Injection_ThreadProc, size, NULL);
	
	printf("야야 %X %X\n", data_addr, addr);
	hThread = CreateRemoteThread(hProcess, 0, 0, addr, data_addr, CREATE_SUSPENDED, 0);
	printf("%X %d 흠ㅋ\n", hThread, GetLastError());
	ResumeThread(hThread);
	WaitForSingleObject(hThread, INFINITE);
	
	CloseHandle(hThread);
	CloseHandle(hProcess);
	printf("ㅎㅁ");

	return TRUE;
}

DWORD GET_PID()
{
	DWORD pid;
	HWND hwnd;

	while (!(hwnd = FindWindow(0, "하이")))
		Sleep(100);
	GetWindowThreadProcessId(hwnd, &pid);

	return pid; 

}

DWORD WINAPI func(LPVOID arg)
{
	DWORD pid;

	pid = GET_PID();
	printf("윈도우 발견\n");
	Injection_Thread(pid);
	return 0;
}
void main()
{
	HANDLE hThread;
	hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, 0, 0, 0);
	WaitForSingleObject(hThread, 100);
	
	while (1)
	{
		printf("흠...\n");
		Sleep(4000);
	}

	CloseHandle(hThread);


}