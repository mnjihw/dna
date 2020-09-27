#define _CRT_SECURE_NO_WARNINGS
#pragma comment(linker, "/OPT:NOWIN98")
#pragma intrinsic(memset, strcmp)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "VirtualizerSDK.h"

/*
res#24
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
<assemblyIdentity version="0.0.0.0"
processorArchitecture="x86"
name="movntq"
type="win32"/>
<description>movntq</description>
<dependency>
<dependentAssembly>
<assemblyIdentity type="win32"
name="Microsoft.Windows.Common-Controls"
version="6.0.0.0"
processorArchitecture="x86"
publicKeyToken="6595b64144ccf1df"
language="*"/>
</dependentAssembly>
</dependency>
<trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
<security>
<requestedPrivileges>
<requestedExecutionLevel
level="requireAdministrator"
uiAccess="false"/>
</requestedPrivileges>
</security>
</trustInfo>
</assembly>
*/

/*
res#24
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
<assemblyIdentity version="1.0.0.0"
processorArchitecture="X86"
name="AliasDatabaseServer"
type="win32" />
<description>AlaiasDatabaseServer manifest</description>
<trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
<security>
<requestedPrivileges>
<requestedExecutionLevel level="requireAdministrator" />
</requestedPrivileges>
</security>
</trustInfo>
</assembly>
*/

unsigned int _wcslen(const void *data)
{
	unsigned int i;

	for (i = 0;; ++i)
		if (((unsigned short *) data)[i] == 0)
			return i;
}

unsigned int _wcscpy(void *dst, const void *src)
{
	unsigned int i;

	for (i = 0;; ++i)
		if ((((unsigned short *) dst)[i] = ((unsigned short *) src)[i]) == 0)
			return i;
}

__declspec(naked) void* GetModule(unsigned int hash)
{
	__asm {
		push esi
			push edi
			VIRTUALIZER_START
		{
			mov edx, fs:30h			/* Put address of PEB in EDX */
			mov edx, [edx + 0Ch]		/* Get pointer to _PEB.Ldr */
			mov edx, [edx + 14h]		/* _PEB_LDR_DATA.InMemoryOrderModuleList.Flink */
		}
		VIRTUALIZER_END
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
	if (module) {
		IMAGE_EXPORT_DIRECTORY *e = (IMAGE_EXPORT_DIRECTORY *) ((DWORD_PTR) module + ((IMAGE_NT_HEADERS *) ((DWORD_PTR) module + ((IMAGE_DOS_HEADER *) module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		if (e != module)
			if (((DWORD_PTR) (name)) & ~65535) {
				DWORD i = e->NumberOfNames;
				while (i)
					if (strcmp((char *) ((DWORD_PTR) module + ((DWORD *) ((DWORD_PTR) module + e->AddressOfNames))[--i]), name) == 0)
						return (PROC) ((DWORD_PTR) module + ((DWORD *) ((DWORD_PTR) module + e->AddressOfFunctions))[((WORD *) ((DWORD_PTR) module + e->AddressOfNameOrdinals))[i]]);
			}
			else if ((DWORD_PTR) name >= e->Base && ((DWORD_PTR) name - e->Base) < e->NumberOfFunctions)
				return (PROC) ((DWORD_PTR) module + ((DWORD *) ((DWORD_PTR) module + e->AddressOfFunctions))[(DWORD_PTR) name - e->Base]);
	}
	return NULL;
}

PROC GetExportAddress(void *module, const char *name)
{
	PROC i;

	VIRTUALIZER_START
	{
		if ((i = GetExportAddressSub(module, name)) != NULL && *(unsigned int *) i == 'LDTN')
		i = GetExportAddressSub(GetModule(0x3CFA685Du/*NTDLL*/), &((char *) i)[6]);
	}
		VIRTUALIZER_END

		return i;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	unsigned int i, j;
	PROCESS_INFORMATION process_info;
	CONTEXT context;
	char data[3072 + 64];

	VIRTUALIZER_START
	{
		if ((i = (unsigned int) GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "GetSystemDirectoryW")(data, 512u - 24u)) != 0 && ((unsigned short *) data)[i - 1u] != '\\')
		((unsigned short *) data)[i++] = '\\';
		_wcscpy(&((unsigned short *) data)[i], L"ADVAPI32.DLL");
		i = (unsigned int) GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "LoadLibraryW")(data);
	}
		VIRTUALIZER_END

		if (i != 0) {
			VIRTUALIZER_START
			{
				i = 1;
				if (GetExportAddress(GetModule(0xC78A43F4u/*ADVAPI*/), "RegOpenKeyExW")(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wizet\\MapleStory", 0, KEY_READ, (HKEY *) &data[1024]) == 0) {
					*(DWORD *) &data[1028] = 512u - 32u;
					if (GetExportAddress(GetModule(0xC78A43F4u/*ADVAPI*/), "RegQueryValueExW")(*(HKEY *) &data[1024], L"ExecPath", NULL, NULL, &data[2], (DWORD *) &data[1028]) == 0 && (i = _wcslen(data)) && ((unsigned short *) data)[i - 1u] != '\\')
						((unsigned short *) data)[i++] = '\\';
					GetExportAddress(GetModule(0xC78A43F4u/*ADVAPI*/), "RegCloseKey")(*(HKEY *) &data[1024]);
				}
				*(unsigned short *) data = '\"';
				_wcscpy(&((unsigned short *) data)[i], L"MapleStory.exe\" GameLaunching");
				i = (unsigned int) GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "CreateProcessW")(NULL, data, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, (memset(&data[1024], 0, sizeof(STARTUPINFO)), (STARTUPINFO *) &data[1024]), &process_info);
			}
				VIRTUALIZER_END
				if (i != 0) {
					__asm {
						pushad
							lea edi, [data + 10h]
							mov esi, offset $FUNC_BEGIN
							mov ecx, offset $FUNC_END
							sub ecx, esi
							add ecx, 3
							and ecx, ~3
							shr ecx, 2
							rep movsd
							popad
							jmp $FUNC_END

							////////////////////////////////////////////////////////////////////////////////
							align 16
						$FUNC_BEGIN:
						jmp short $ENTRYPOINT

							align 16
						$KERNEL32 :
								  push esi
								  push edi
								  mov edx, fs : 30h			/* Put address of PEB in EDX */
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
									   cmp edi, 6A4ABC5Bh		/* Check EDI against checksum ("kernel32.dll" = 0x6A4ABC5B) */
									   mov eax, [edx + 10h]		/* _LDR_MODULE.DllBase */
									   mov edx, [edx]			/* _LDR_MODULE.Flink */
									   jnz short $NEXT_MODULE	/* Move onto the next module name if this one wasn't the dll */

								   $EXIT :
						pop edi
							pop esi
							retn

							////////////////////////////////////////////////////////////////////////////////
							align 16
						$ENTRYPOINT:
						pushad /* 32bytes [EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI] */
							mov ebp, [esp + 24h]
							sub dword ptr[esp + 20h], 0Ah
							mov edi, [esp + 20h]
							mov esi, ebp
							mov ecx, 4
							rep movsd

							lea edx, [ebp + 800h]
							push edx
							call $KERNEL32
							push eax
							call $GetExportAddress
							add esp, 8
							jz short $ERR_EXIT

							lea edx, [ebp + 400h]
							push edx
							call eax
							test eax, eax
							jz short $ERR_EXIT

							push 71Ah /* 1818 */
							push eax
							call $GetExportAddress
							add esp, 8
							test eax, eax
							jz short $ERR_EXIT

							push ebp
							call eax
							add esp, 4
							test eax, eax
							jnz short $ERR_EXIT
							popad
							ret 4

							align 16
						$ERR_EXIT:
						push 0
							lea eax, [ebp + 840h]
							push eax
							call $KERNEL32
							push eax
							call $GetExportAddress
							add esp, 8
							call eax
							popad
							ret 4

							////////////////////////////////////////////////////////////////////////////////
							align 16
						$GetExportAddress:
						push ebx
							push ebp
							mov ebp, [esp + 0Ch]
							push esi
							test ebp, ebp
							push edi
							jz short $OFFSET_BB
							mov eax, [ebp + 3Ch]
							mov edi, [eax + ebp + 78h]
							add edi, ebp
							cmp edi, ebp
							mov[esp + 14h], edi
							jz short $OFFSET_BB
							mov eax, [esp + 18h]
							test eax, 0FFFF0000h
							jz short $OFFSET_E2
							mov edx, [edi + 18h]
							test edx, edx
							jz short $OFFSET_BB
							mov ecx, [edi + 20h]
							lea edi, [ecx + edx * 4]
							add edi, ebp
							jmp short $OFFSET_7D

						$OFFSET_79 :
						mov eax, [esp + 18h]

						$OFFSET_7D :
								   mov ecx, [edi - 4]
								   sub edi, 4
								   mov esi, eax
								   mov eax, ebp
								   dec edx
								   add eax, ecx

							   $OFFSET_8A :
						mov bl, [eax]
							mov cl, bl
							cmp bl, [esi]
							jnz short $OFFSET_AE
							test cl, cl
							jz short $OFFSET_AA
							mov bl, [eax + 1]
							mov cl, bl
							cmp bl, [esi + 1]
							jnz short $OFFSET_AE
							add eax, 2
							add esi, 2
							test cl, cl
							jnz short $OFFSET_8A

						$OFFSET_AA :
						xor eax, eax
							jmp short $OFFSET_B3

						$OFFSET_AE :
						sbb eax, eax
							sbb eax, 0FFFFFFFFh

						$OFFSET_B3 :
								   test eax, eax
								   jz short $OFFSET_C2
								   test edx, edx
								   jnz short $OFFSET_79

							   $OFFSET_BB :
						pop edi
							pop esi
							pop ebp
							xor eax, eax
							pop ebx
							retn

						$OFFSET_C2 :
						mov eax, [esp + 14h]
							pop edi
							pop esi
							mov ecx, [eax + 24h]
							lea edx, [ecx + edx * 2]
							xor ecx, ecx
							mov cx, [edx + ebp]
							mov edx, [eax + 1Ch]
							lea eax, [edx + ecx * 4]
							mov eax, [eax + ebp]
							add eax, ebp
							pop ebp
							pop ebx
							retn

						$OFFSET_E2 :
						mov ecx, [edi + 10h]
							cmp eax, ecx
							jb short $OFFSET_BB
							sub eax, ecx
							mov ecx, [edi + 14h]
							cmp eax, ecx
							jnb short $OFFSET_BB
							mov ecx, [edi + 1Ch]
							pop edi
							pop esi
							lea edx, [ecx + eax * 4]
							mov eax, [edx + ebp]
							add eax, ebp
							pop ebp
							pop ebx
							retn

							////////////////////////////////////////////////////////////////////////////////
							align 16
						$FUNC_END:
					}

					VIRTUALIZER_START
					{
						for (i = (unsigned int) GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "GetModuleFileNameW")(NULL, &data[1024], 512u - 16u); i && ((unsigned short *) data)[i + 511u] != '\\'; --i);
						((unsigned short *) data)[i++ + 512u] = 'M';
						((unsigned short *) data)[i++ + 512u] = 'B';
						((unsigned short *) data)[i++ + 512u] = 'C';
						((unsigned short *) data)[i++ + 512u] = '.';
						((unsigned short *) data)[i++ + 512u] = 'D';
						((unsigned short *) data)[i++ + 512u] = 'L';
						((unsigned short *) data)[i++ + 512u] = 'L';
						((unsigned short *) data)[i++ + 512u] = 0;
						strcpy(&data[2048], "LoadLibraryW");
						strcpy(&data[2112], "ExitProcess");

						/*
						EAX register from the process with CREATE_SUSPENDED indicates to EntryPoint address
						EBX register from the process with CREATE_SUSPENDED indicates to PEB address
						*/
						if (context.ContextFlags = CONTEXT_INTEGER, GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "GetThreadContext")(process_info.hThread, &context) == 0 ||
							GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "ReadProcessMemory")(process_info.hProcess, (void *) context.Eax, data, 16u, (DWORD *) &data[3072]) == 0 ||
							(j = (unsigned int) GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "VirtualAllocEx")(process_info.hProcess, 0, 4096, MEM_COMMIT | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE)) == 0 ||
							GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "WriteProcessMemory")(process_info.hProcess, (void *) j, data, 3072, (DWORD *) &data[3072]) == 0 ||
							GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "VirtualProtectEx")(process_info.hProcess, (void *) context.Eax, 16, PAGE_EXECUTE_READWRITE, (DWORD *) data) == 0 ||
							(*(unsigned char *) &data[0] = 0x68, *(unsigned int *) &data[1] = j, *(unsigned char *) &data[5] = 0xE8, *(unsigned int *) &data[6] = (j + 16u) - ((unsigned int) context.Eax + 5u) - 5u, *(unsigned int *) &data[10] = 0x90909090, *(unsigned short *) &data[14] = 0x9090),
							GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "WriteProcessMemory")(process_info.hProcess, (void *) context.Eax, data, 16, (DWORD *) &data[16]) == 0 ||
							GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "FlushInstructionCache")(process_info.hProcess, (void *) context.Eax, 16) == 0 ||
							GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "ResumeThread")(process_info.hThread) != 1)
							GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "TerminateProcess")(process_info.hProcess, 0);

						GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "CloseHandle")(process_info.hProcess);
						GetExportAddress(GetModule(0x6A4ABC5Bu/*KERNEL32*/), "CloseHandle")(process_info.hThread);
					}
						VIRTUALIZER_END
				}
		}

	ExitProcess(0);
}