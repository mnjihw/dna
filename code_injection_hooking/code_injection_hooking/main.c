#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
void* GetModule(unsigned int hash);
PROC GetExportAddress(void *module, const char *name);
#define kernel32_module 0x6A4ABC5Bu
#define user32_module 0x63C84283u


typedef struct
{
	PROC _myfunc;
	char func_name[4][64];
}THREAD_PARAM, *PTHREAD_PARAM;

#if 1
int WINAPI myfunc(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);


DWORD WINAPI hook_hotpatch(LPVOID arg)
{
	DWORD protect;
	PTHREAD_PARAM param = (PTHREAD_PARAM)arg;

	int (WINAPI *_MessageBoxA)(HWND, LPCSTR, LPCSTR, UINT) = ((int (WINAPI *)(HWND, LPCSTR, LPCSTR, UINT))GetExportAddress(GetModule(user32_module), param->func_name[0]));
	
	if (*(BYTE*)_MessageBoxA == 0xEB)
		return -1;
	
	BOOL (WINAPI *_VirtualProtect)(LPVOID, SIZE_T, DWORD, PDWORD) = ((BOOL (WINAPI *)(LPVOID, SIZE_T, DWORD, PDWORD))GetExportAddress(GetModule(kernel32_module), param->func_name[1]));
	BOOL (WINAPI *_FlushInstructionCache)(HANDLE, LPCVOID, SIZE_T) = ((BOOL (WINAPI *)(HANDLE, LPCVOID, SIZE_T))GetExportAddress(GetModule(kernel32_module), param->func_name[2]));
	HANDLE (WINAPI *_GetCurrentProcess)() = ((HANDLE (WINAPI *)())GetExportAddress(GetModule(kernel32_module), param->func_name[3]));
	
	if (_VirtualProtect((LPVOID)((DWORD)_MessageBoxA - 5), 7, PAGE_EXECUTE_READWRITE, &protect))
	{

		*(BYTE*)((DWORD)_MessageBoxA - 5) = 0xE9; // jmp
		*(DWORD*)((DWORD)_MessageBoxA - 4) = (DWORD)param->_myfunc - (DWORD)_MessageBoxA; //이부분이문제임 시발 주소 치엔으로 확인
		*(WORD*)_MessageBoxA = 0xF9EB; //short_jmp

		_VirtualProtect((LPVOID)((DWORD)_MessageBoxA - 5), 7, protect, &protect);
		_FlushInstructionCache(_GetCurrentProcess(), (LPCVOID)((DWORD)_MessageBoxA - 5), 7);
	}
	return 0;

}

int WINAPI myfunc(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	char func_name[128], buf[128];

	((int*)func_name)[0] = 'sseM';
	((int*)func_name)[1] = 'Bega';
	((int*)func_name)[2] = 'Axo';
	((int*)buf)[0] = 'zzz';


	return ((int (WINAPI *)(HWND, LPCSTR, LPCSTR, UINT))(((DWORD)GetExportAddress(GetModule(user32_module), func_name) + 2)))(hWnd, buf, lpCaption, uType);
}
#endif


__declspec(naked) void* GetModule(unsigned int hash)
{
	__asm {
		push esi
		push edi
		\
		mov edx, fs:30h			/* Put address of PEB in EDX */
		mov edx, [edx + 0Ch]		/* Get pointer to _PEB.Ldr */
		mov edx, [edx + 14h]		/* _PEB_LDR_DATA.InMemoryOrderModuleList.Flink */

		cld

		$NEXT_MODULE :
		xor eax, eax
			test edx, edx
			jz short $EXIT
			mov esi, [edx + 28h]		/* _LDR_MODULE.BaseDllName.Buffer (Unicode) */
			mov ecx, [edx + 24h]
			and ecx, 0FFFFh
			test esi, esi
			jz short $EXIT
			xor edi, edi

			$NEXT_CHAR :
		lodsb
			cmp al, 'a'				/* Check if character is uppercase */
			jl short $NOT_LOWERCASE
			sub al, 20h				/* uppercase character */

			$NOT_LOWERCASE :
		ror edi, 0Dh
			add edi, eax			/* Add uppercase'd character to EDI */
			loop short $NEXT_CHAR
			cmp edi, [esp + 0Ch]		/* Check EDI against checksum */
			mov eax, [edx + 10h]		/* _LDR_MODULE.DllBase */
			mov edx, [edx]			/* _LDR_MODULE.Flink */
			jnz short $NEXT_MODULE	/* Move onto the next module name if this one wasn't the dll */

			$EXIT :
		pop edi
			pop esi
			retn
	}
}


PROC GetExportAddressSub(void *module, const char *name)
{
	IMAGE_EXPORT_DIRECTORY *e;

	if (module)
	{
		e = (IMAGE_EXPORT_DIRECTORY *)((DWORD_PTR)module + ((IMAGE_NT_HEADERS *)((DWORD_PTR)module + ((IMAGE_DOS_HEADER *)module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

		if (e != module)
			if (((DWORD_PTR)(name)) & ~65535)
			{
				DWORD i = e->NumberOfNames;
				while (i)
					if (strcmp((char *)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfNames))[--i]), name) == 0)
						return (PROC)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfFunctions))[((WORD *)((DWORD_PTR)module + e->AddressOfNameOrdinals))[i]]);
			}
			else if ((DWORD_PTR)name >= e->Base && ((DWORD_PTR)name - e->Base) < e->NumberOfFunctions)
				return (PROC)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfFunctions))[(DWORD_PTR)name - e->Base]);
	}
	return NULL;
}

PROC GetExportAddress(void *module, const char *name)
{
	PROC i;

	i = GetExportAddressSub(module, name);

	return i;
}


DWORD WINAPI unhook_hotpatch(LPVOID arg)
{
	DWORD protect;
	PTHREAD_PARAM param = (PTHREAD_PARAM)arg;


	int (WINAPI *_MessageBoxA)(HWND, LPCSTR, LPCSTR, UINT) = ((int (WINAPI *)(HWND, LPCSTR, LPCSTR, UINT))GetExportAddress(GetModule(user32_module), param->func_name[0]));
	
	if (*(BYTE*)_MessageBoxA != 0xEB)
		return -1;
	
	
	
	BOOL(WINAPI *_VirtualProtect)(LPVOID, SIZE_T, DWORD, PDWORD) = ((BOOL(WINAPI *)(LPVOID, SIZE_T, DWORD, PDWORD))GetExportAddress(GetModule(kernel32_module), param->func_name[1]));
	BOOL(WINAPI *_FlushInstructionCache)(HANDLE, LPCVOID, SIZE_T) = ((BOOL(WINAPI *)(HANDLE, LPCVOID, SIZE_T))GetExportAddress(GetModule(kernel32_module), param->func_name[2]));
	HANDLE(WINAPI *_GetCurrentProcess)() = ((HANDLE(WINAPI *)())GetExportAddress(GetModule(kernel32_module), param->func_name[3]));

	if (_VirtualProtect((LPVOID)((DWORD)_MessageBoxA - 5), 7, PAGE_EXECUTE_READWRITE, &protect))
	{

		*(DWORD*)((DWORD)_MessageBoxA - 5) = 0x90909090; // nop
		*(BYTE*)((DWORD)_MessageBoxA - 1) = 0x90;
		*(WORD*)_MessageBoxA = 0xFF8B; //mov edi, edi

		_VirtualProtect((LPVOID)((DWORD)_MessageBoxA - 5), 7, protect, &protect);
		_FlushInstructionCache(_GetCurrentProcess(), (LPCVOID)((DWORD)_MessageBoxA - 5), 7);
	}

	return 0;
}

void InjectCode(LPCSTR lpWinTitle)
{
	THREAD_PARAM param = { 0, };
	HANDLE hProcess, hThread;
	HWND hWnd;	
	LPVOID param_buf, code_buf;
	DWORD size, pid;

	if (!(hWnd = FindWindow(NULL, lpWinTitle)))
	{
		printf("윈도우 찾기 실패!");
		return;
	}

	GetWindowThreadProcessId(hWnd, &pid);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);


	size = (DWORD)unhook_hotpatch - (DWORD)hook_hotpatch; 
	code_buf = VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, code_buf, hook_hotpatch, size, NULL);

	strcpy(param.func_name[0], "MessageBoxA");
	strcpy(param.func_name[1], "VirtualProtect");
	strcpy(param.func_name[2], "FlushInstructionCache");
	strcpy(param.func_name[3], "GetCurrentProcess");
	param._myfunc = (DWORD)code_buf + (DWORD)myfunc - (DWORD)hook_hotpatch;
	param_buf = VirtualAllocEx(hProcess, NULL, sizeof(param), MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, param_buf, &param, sizeof(param), NULL);



	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)code_buf, param_buf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, param_buf, 0, MEM_RELEASE);
	//code_buf는 해제하면 안 됨
	CloseHandle(hProcess);


	
}

void EjectCode(LPCSTR lpWinTitle)
{
	THREAD_PARAM param = { 0, };
	HANDLE hProcess, hThread;
	HWND hWnd;
	LPVOID param_buf, code_buf;
	DWORD size, pid;

	if (!(hWnd = FindWindow(NULL, lpWinTitle)))
	{
		printf("윈도우 찾기 실패!");
		return;
	}

	GetWindowThreadProcessId(hWnd, &pid);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	

	size = (DWORD)InjectCode - (DWORD)GetModule; 
	code_buf = VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, code_buf, GetModule, size, NULL);

	strcpy(param.func_name[0], "MessageBoxA");
	strcpy(param.func_name[1], "VirtualProtect");
	strcpy(param.func_name[2], "FlushInstructionCache");
	strcpy(param.func_name[3], "GetCurrentProcess");
	
	param_buf = VirtualAllocEx(hProcess, NULL, sizeof(param), MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, param_buf, &param, sizeof(param), NULL);
	
	
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((DWORD)code_buf + (DWORD)unhook_hotpatch - (DWORD)GetModule), param_buf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, code_buf, 0, MEM_RELEASE);
	VirtualFreeEx(hProcess, param_buf, 0, MEM_RELEASE);
	CloseHandle(hProcess);


}

void main()
{

	InjectCode("리듬 dll 인젝터");

	printf("인젝션 완료!\n");
	printf("이젝션하려면 엔터");
	getchar();

	EjectCode("리듬 dll 인젝터");
	printf("이젝션 완료!");
	getchar();
	
}