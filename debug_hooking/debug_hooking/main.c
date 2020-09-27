#include "common.h"

BYTE g_original_byte;
CREATE_PROCESS_DEBUG_INFO g_cpdi;
int (WINAPI *_MessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);


void OnCreateProcessDebugEvent(LPDEBUG_EVENT pde)
{

	memcpy(&g_cpdi, &pde->u.CreateProcessInfo, sizeof(CREATE_PROCESS_DEBUG_INFO));

	ReadProcessMemory(g_cpdi.hProcess, _MessageBoxA, &g_original_byte, sizeof(BYTE), NULL);
	WriteProcessMemory(g_cpdi.hProcess, _MessageBoxA, "\xCC", sizeof(BYTE), NULL);

}

BOOL OnExceptionDebugEvent(LPDEBUG_EVENT pde)
{
	CONTEXT ctx;
	PEXCEPTION_RECORD per = &pde->u.Exception.ExceptionRecord;
	char buf[128] = { 0, };
	DWORD addr, protect;

	if (per->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		if (per->ExceptionAddress == _MessageBoxA)
		{
			printf("�ɸ�");
			WriteProcessMemory(g_cpdi.hProcess, _MessageBoxA, &g_original_byte, sizeof(BYTE), NULL);

			ctx.ContextFlags = CONTEXT_FULL;
			GetThreadContext(g_cpdi.hThread, &ctx);

			//ReadProcessMemory(g_cpdi.hProcess, (LPVOID)(*(char*)ctx.Esp), buf, , NULL);
			ReadProcessMemory(g_cpdi.hProcess, (LPVOID)(ctx.Esp + 0x8), &addr, 4, NULL);
			ReadProcessMemory(g_cpdi.hProcess, addr, buf, 64, 0);
			
			//ReadProcessMemory(g_cpdi.hProcess, (LPVOID)addr, buf, sizeof(buf), NULL);

			printf("����: %s", buf);
			wsprintf(buf, "��ŷ�ʤ�");

			//VirtualProtectEx(g_cpdi.hProcess, (LPVOID)addr, 4, PAGE_READWRITE, &protect);
			WriteProcessMemory(g_cpdi.hProcess, (LPVOID)addr, "��ŷ�ʤ�", strlen("��ŷ�ʤ�") + 1, NULL);
			
			//VirtualProtectEx(g_cpdi.hProcess, (LPVOID)addr, 4, protect, &protect);

			printf("�� %d", GetLastError());
			DWORD a = MB_ABORTRETRYIGNORE;
			WriteProcessMemory(g_cpdi.hProcess, (LPVOID)(ctx.Esp + 0x10), &a, 4, 0);


			 
			ctx.Eip = (DWORD)_MessageBoxA;
			SetThreadContext(g_cpdi.hThread, &ctx);

			ContinueDebugEvent(pde->dwProcessId, pde->dwThreadId, DBG_CONTINUE);
			Sleep(0);
			WriteProcessMemory(g_cpdi.hProcess, _MessageBoxA, "\xCC", sizeof(BYTE), NULL);

			return TRUE;
		}
		
	}

	return FALSE;
}

void DebugLoop()
{
	DEBUG_EVENT de;
	

	while (WaitForDebugEvent(&de, INFINITE))
	{
		if (de.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT)
		{
			OnCreateProcessDebugEvent(&de);
		}
		else if (de.dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
		{
			if (OnExceptionDebugEvent(&de))
				continue;
		}
		else if (de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
		{
			break;
		}
		ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_CONTINUE);


	}
}

void DebugWindow(LPCSTR lpWinTitle)
{
	HWND hWnd;
	DWORD pid;
	

	if(!(hWnd = FindWindow(NULL, lpWinTitle)))
	{
		printf("������ ã�� ����!");
		return;
	}

	_MessageBoxA = (int (WINAPI *)(HWND, LPCSTR, LPCSTR, UINT))GetExportAddress(GetModule(user32_module), "MessageBoxA");

	GetWindowThreadProcessId(hWnd, &pid);
	printf("pid: %d �޹�: %p\n", pid, _MessageBoxA);

	DebugActiveProcess(pid);
	
	DebugLoop();
		
	
	
}

void main()
{
	DebugWindow("���� dll ������");
	
}