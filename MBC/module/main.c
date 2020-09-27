#pragma comment(linker, "/OPT:NOWIN98")
#pragma comment(linker, "/EXPORT:_setup,@1818,NONAME")
#include "common.h"

unsigned int S_MINIROOM, S_MINIROOM_CREATE, S_MINIROOM_ENTER, S_MINIROOM_START2, S_MINIROOM_LEAVE, S_MINIROOM_ADDITEM, S_MINIROOM_START1, R_USER_MINIROOM_BALLON, R_EMPLOYEE_LEAVE, PACKET_DENY[10];

unsigned int secure_value_UUID, secure_value_S;
void *client_base, *module_base;
char client_path[1024], module_path[1024];
static PROC _GetStartupInfoA;
static void *TEB;

void setup_sub_ex(void)
{
	char data[2048];

	VIRTUALIZER1_START
	{
		strcpy(&data[strlen(strcpy(&data[128], module_path)) + 128u], "MBC.ini");
		S_MINIROOM = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "S_MINIROOM", 0, &data[128]);
		S_MINIROOM_CREATE = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "S_MINIROOM_CREATE", 0, &data[128]);
		S_MINIROOM_ENTER = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "S_MINIROOM_ENTER", 0, &data[128]);
		S_MINIROOM_START2 = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "S_MINIROOM_START2", 0, &data[128]);
		S_MINIROOM_LEAVE = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "S_MINIROOM_LEAVE", 0, &data[128]);
		S_MINIROOM_ADDITEM = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "S_MINIROOM_ADDITEM", 0, &data[128]);
		S_MINIROOM_START1 = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "S_MINIROOM_START1", 0, &data[128]);
		R_USER_MINIROOM_BALLON = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "R_USER_MINIROOM_BALLON", 0, &data[128]);
		R_EMPLOYEE_LEAVE = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "R_EMPLOYEE_LEAVE", 0, &data[128]);
		PACKET_DENY[0] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY0", 0xFFFF, &data[128]);
		PACKET_DENY[1] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY1", 0xFFFF, &data[128]);
		PACKET_DENY[2] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY2", 0xFFFF, &data[128]);
		PACKET_DENY[3] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY3", 0xFFFF, &data[128]);
		PACKET_DENY[4] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY4", 0xFFFF, &data[128]);
		PACKET_DENY[5] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY5", 0xFFFF, &data[128]);
		PACKET_DENY[6] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY6", 0xFFFF, &data[128]);
		PACKET_DENY[7] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY7", 0xFFFF, &data[128]);
		PACKET_DENY[8] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY8", 0xFFFF, &data[128]);
		PACKET_DENY[9] = SECURE_API(s_GetPrivateProfileIntA)("PACKET", "PACKET_DENY9", 0xFFFF, &data[128]);
	}
	VIRTUALIZER_END
}

LONG WINAPI MyInterlockedIncrement(LPLONG lpAddend)
{
	return (*lpAddend += 1u);
}

LONG WINAPI MyInterlockedDecrement(LPLONG lpAddend)
{
	return (*lpAddend -= 1u);
}

VOID WINAPI MySleep1(DWORD dwMilliseconds)
{
	SECURE_API(s_SleepEx)(dwMilliseconds, TEB == (void *)NtCurrentTeb());
}

PROC WINAPI MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	static volatile skip;
	unsigned int i;
	char data[64];

	/*
		KERNEL32 USER32 OLE32 GDI32 ADVAPI32 SHELL32 IMM32 WINMM WS2_32 MAPLESTORY DBGHELP
		ZtlTaskMemAllocImp
		ZtlTaskMemReallocImp
		ZtlTaskMemFreeImp
	*/

	if (skip == 0 && hModule == (HMODULE)0x400000u && HIWORD(lpProcName) && (~*(int *)&lpProcName[0] == ~'TltZ' && ~*(int *)&lpProcName[4] == ~'Mksa' && ~*(int *)&lpProcName[8] == ~'lAme' && ~*(int *)&lpProcName[12] == ~'Icol' && ~*(int *)&lpProcName[16] == ~'pm') && _InterlockedExchange(&skip, ~0) == 0) {
		VIRTUALIZER3_START
		{
			if ((secure_value_S ^ (init_rand(SECURE_API(s_GetCurrentProcessId)() ^ 0xBADF00Du), gen_rand() ^ gen_rand())) == 0x19920417u)
				if (SECURE_API(s_LoadLibraryA)((*(int *)&data[0] = 'MOCP', *(int *)&data[4] = 'LLD.', *(int *)&data[8] = 0, data)) &&
					SECURE_API(s_LoadLibraryA)((*(int *)&data[0] = 'MSER', *(int *)&data[4] = 'D.NA', *(int *)&data[8] = 'LL', data)) &&
					SECURE_API(s_LoadLibraryA)((*(int *)&data[0] = 'EMAN', *(int *)&data[4] = 'CAPS', *(int *)&data[8] = 'LD.E', *(int *)&data[12] = 'L', data))) {

					//MessageBox(NULL, "ㅋ", 0, 0);

					for (*(PROC *)data = GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), (((int *)data)[0] = 'etnI', ((int *)data)[1] = 'colr', ((int *)data)[2] = 'Idek', ((int *)data)[3] = 'ercn', ((int *)data)[4] = 'tnem', ((int *)data)[5] = 0, data)); (i = (unsigned int)memscan_ptr(client_base, data, 4)) != 0; *(PROC *)i = (PROC)MyInterlockedIncrement);
					for (*(PROC *)data = GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), (((int *)data)[0] = 'etnI', ((int *)data)[1] = 'colr', ((int *)data)[2] = 'Ddek', ((int *)data)[3] = 'erce', ((int *)data)[4] = 'tnem', ((int *)data)[5] = 0, data)); (i = (unsigned int)memscan_ptr(client_base, data, 4)) != 0; *(PROC *)i = (PROC)MyInterlockedDecrement);
					TEB = (void *)NtCurrentTeb();
					HookJMP(SECURE_API(s_Sleep), MySleep1, NULL);
					setup_sub_ex();
					hshield_hook();
					client_hook();
					socket_hook();
				} else
					SECURE_API(s_ExitProcess)(0);
		}
		VIRTUALIZER_END
	}

	return (PROC)SECURE_API(s_GetProcAddress)(hModule, lpProcName);
}

void do_hook(void)
{
	unsigned int i;
	PROC fn;

	VIRTUALIZER3_START
	{
		if ((secure_value_S ^ (init_rand(SECURE_API(s_GetCurrentProcessId)() ^ 0xBADF00Du), gen_rand() ^ gen_rand())) == 0x19920417u)
			for (fn = SECURE_API(s_GetProcAddress); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)MyGetProcAddress);
	}
	VIRTUALIZER_END
}

__declspec(naked) MyGetStartupInfoA(LPSTARTUPINFOA lpStartupInfo)
{
	static volatile skip;

	/*
		cdecl (http://en.wikipedia.org/wiki/X86_calling_conventions)
		Registers EAX, ECX, and EDX are caller-saved, and the rest are callee-saved.
	*/

	__asm {
		cmp skip, 0
		jnz short $1
		VIRTUALIZER3_START
		{
			mov eax, client_base
			add eax, dword ptr [eax+3Ch] /* IMAGE_DOS_HEADER.e_lfanew */
			mov edx, dword ptr [eax+34h] /* IMAGE_NT_HEADERS.OptionalHeader.ImageBase */
			add eax, size IMAGE_NT_HEADERS
			add edx, dword ptr [eax+IMAGE_SIZEOF_SHORT_NAME+4] /* IMAGE_SECTION_HEADER.VirtualAddress */
			cmp edx, [esp]
			ja short $2
			add edx, dword ptr [eax+IMAGE_SIZEOF_SHORT_NAME] /* IMAGE_SECTION_HEADER.VirtualSize */
			cmp edx, [esp]
			jb short $2
			mov eax, 0FFFFFFFFh
			lock xchg dword ptr [skip], eax
			test eax, eax
			jnz short $2
			call do_hook
$2:
		}
		VIRTUALIZER_END
$1:
		jmp dword ptr [_GetStartupInfoA]
	}
}

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
	SECURE_API(s_TerminateProcess)(SECURE_API(s_GetCurrentProcess)(), 0);
	return EXCEPTION_EXECUTE_HANDLER;
}

void setup_sub(const void *data)
{
	// 내꺼
	// 37AC DFDB 3433 8EA8 F640 4783 EBDF BDFC 5E6F 3C71
	// BC55 DC94 B636 FEA9 DF5D D2F4 3619 99BF 4698 65E1
	// E60B 0C4E 6F93 FE09 A809 42AD D483 EF0E 3674 A2C3

	// 리듬
	// 6E08 1FEF 5763 74C0 7B61 1B7F FB0E DA22 355C 35C0
	// 9A5A E69C 7FEC 9E6E BB4B DD68 43C8 63C0 BDFB 23F4
	// A7F2 737B BCDD B490 2A64 FBE9 53EF 69CE 2EBB 05EB

	// 0DAA 8024 0A5A BDC9 E905 5F17 7DD2 CA76 2A4F 2F08

	VIRTUALIZER3_START
	{
		if (
			/*
			(((int *)data)[0] == ~'37AC' && ((int *)data)[1] == ~'DFDB' && ((int *)data)[2] == ~'3433' && ((int *)data)[3] == ~'8EA8' && ((int *)data)[4] == ~'F640' && ((int *)data)[5] == ~'4783' && ((int *)data)[6] == ~'EBDF' && ((int *)data)[7] == ~'BDFC' && ((int *)data)[8] == ~'5E6F' && ((int *)data)[9] == ~'3C71') ||
			*/
			
			(((int *)data)[0] == ~'0DAA' && ((int *)data)[1] == ~'8024' && ((int *)data)[2] == ~'0A5A' && ((int *)data)[3] == ~'BDC9' && ((int *)data)[4] == ~'E905' && ((int *)data)[5] == ~'5F17' && ((int *)data)[6] == ~'7DD2' && ((int *)data)[7] == ~'CA76' && ((int *)data)[8] == ~'2A4F' && ((int *)data)[9] == ~'2F08')

			)
			secure_value_UUID = crcsum32(0xBADF00Du, data, 40u);
	}
	VIRTUALIZER_END
}

int setup(void *arg)
{
	unsigned int i;
	char data[2048];

	VIRTUALIZER3_START
	{
		SECURE_API(s_SetUnhandledExceptionFilter)(MyUnhandledExceptionFilter);
		vmemcpy(SECURE_API(s_SetUnhandledExceptionFilter), "\xC2\x04\x00", 3);

		if (*gen_uuid(data)) {
			*(int *)&data[128] = ~_byteswap_ulong(((int *)data)[0]);
			*(int *)&data[132] = ~_byteswap_ulong(((int *)data)[1]);
			*(int *)&data[136] = ~_byteswap_ulong(((int *)data)[2]);
			*(int *)&data[140] = ~_byteswap_ulong(((int *)data)[3]);
			*(int *)&data[144] = ~_byteswap_ulong(((int *)data)[4]);
			*(int *)&data[148] = ~_byteswap_ulong(((int *)data)[5]);
			*(int *)&data[152] = ~_byteswap_ulong(((int *)data)[6]);
			*(int *)&data[156] = ~_byteswap_ulong(((int *)data)[7]);
			*(int *)&data[160] = ~_byteswap_ulong(((int *)data)[8]);
			*(int *)&data[164] = ~_byteswap_ulong(((int *)data)[9]);
			setup_sub(&data[128]);
			if (crcsum32(0xBADF00Du, &data[128], 40u) == secure_value_UUID) {			
				SECURE_API(s_timeBeginPeriod)(SECURE_API(s_timeGetDevcaps)(data, 8) ? 1 : *(unsigned int *)data); /* Gr2D 초기화 할 때, 자동으로 셋팅하던데.. */
				SECURE_API(s_SetThreadPriority)(SECURE_API(s_GetCurrentThread)(), THREAD_PRIORITY_HIGHEST);
				i = (init_rand(SECURE_API(s_GetCurrentProcessId)() ^ 0xBADF00Du), gen_rand() ^ gen_rand());
				secure_value_S = i ^ 0x19920417u;
				secure_value_A = i ^ 0x92041719u;
				secure_value_B = i ^ 0x04171992u;
				secure_value_C = i ^ 0x17199204u;
				_GetStartupInfoA = HookExportProc(GetModule(0x6A4ABC5Bu/*KERNEL32*/), (*(int *)&data[0] = 'SteG', *(int *)&data[4] = 'trat', *(int *)&data[8] = 'nIpu', *(int *)&data[12] = 'Aof', data), MyGetStartupInfoA);
				SECURE_API(s_PlaySoundA)(MAKEINTRESOURCE(IDR_WAVE2), module_base, SND_ASYNC | SND_RESOURCE);
			} else {		
				strcpy(&data[strlen(strcpy(&data[128], module_path)) + 128u], "MBC.ini");
				SECURE_API(s_WritePrivateProfileStringA)("AuthCode", "AuthCode", data, &data[128]);
				SECURE_API(s_PlaySoundA)(MAKEINTRESOURCE(IDR_WAVE1), module_base, SND_RESOURCE);
			}
		}
	}
	VIRTUALIZER_END

	return 0;
}

int __stdcall entrypointsub(void *module)
{
	unsigned int i;

	VIRTUALIZER2_START
	{
		if ((module_base = module) != NULL && ((client_base = GetModule(0xEC755B21u/*MapleStory*/)) != NULL || (client_base = GetModule(0xC57FF591u/*MapleStoryT*/)) != NULL) && secure_setup() == 0) {
			for (i = SECURE_API(s_GetModuleFileNameA)(module_base, module_path, sizeof(module_path)); i && module_path[i - 1u] != '\\'; --i);
			module_path[i] = '\0';
			for (i = SECURE_API(s_GetModuleFileNameA)(client_base, client_path, sizeof(client_path)); i && client_path[i - 1u] != '\\'; --i);
			client_path[i] = '\0';
			i = SECURE_API(s_SetCurrentDirectoryA)(client_path) && SECURE_API(s_DisableThreadLibraryCalls)(module_base);
		} else
			i = 0;
	}
	VIRTUALIZER_END

	return i;
}


__declspec(naked) __stdcall entrypoint(void *module, unsigned int mode, void *data)
{
	/* EntryPoint에서 코드 가상화를 걸면 바이러스로 오진함 */
	__asm {
		mov eax, 1
		cmp [esp+8], DLL_PROCESS_ATTACH
		jnz short $EXIT 
		push [esp+4]
		mov eax, offset entrypointsub - 4096u
		add eax, 4096u
		call eax
$EXIT:
		ret 0Ch
	}
}