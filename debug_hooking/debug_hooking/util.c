#include "common.h"

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

