#include <Windows.h>

char code[5];

void HookJMP(void *addr, void *fn, void *code)
{
	DWORD protect;

	if (VirtualProtect(addr, 5, PAGE_READWRITE, &protect))
	{
		if (fn)
		{
			if (code)
				memcpy(code, addr, 5);
			*(unsigned char *)addr = 0xE9; /* JMP */
			*(unsigned int  *)((unsigned int)addr + 1u) = (unsigned int)fn - (unsigned int)addr - 5u;
		}
		else if (code)
			memcpy(addr, code, 5);
		VirtualProtect(addr, 5, protect, &protect);
		FlushInstructionCache(GetCurrentProcess(), addr, 5);
	}

}

int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	int ret;
	static int count;
	char buf[128];


	OutputDebugString("À½À½À½");
	HookJMP(GetProcAddress(GetModuleHandle("user32.dll"), "MessageBoxA"), NULL, code);
	
	wsprintf(buf, "¿Ó´õ»¶ %d", ++count);
	ret = MessageBoxA(hWnd, buf, lpCaption, uType);
	HookJMP(GetProcAddress(GetModuleHandle("user32.dll"), "MessageBoxA"), MyMessageBoxA, code);

	return ret;
}



BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{


	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		
		HookJMP(GetProcAddress(GetModuleHandle("user32.dll"), "MessageBoxA"), MyMessageBoxA, code);
		
		break;
	case DLL_PROCESS_DETACH:
		HookJMP(GetProcAddress(GetModuleHandle("user32.dll"), "MessageBoxA"), NULL, code);
		break;
	}
	return TRUE;
}