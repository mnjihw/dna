#include "common.h"

void dbg(const char *fmt, ...)
{
	char msg[1024];

	VIRTUALIZER1_START
	{
		SECURE_API(s_OutputDebugStringA)((SECURE_API(s_wvsprintfA)(msg, fmt, (va_list) ((unsigned int) &fmt + sizeof(void *))), msg));
	}
	VIRTUALIZER_END
}

void dump(const void *data, unsigned int size)
{
	unsigned int i, j, k, z;
	char buffer[128];

	for (z = 0, i = 0; i < size; i += 16u) {
		k = 0;

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size) {
				for (; j < 16u; ++j) {
					*(unsigned int *)&buffer[k] = '    ';
					k += 3u;
				}
				break;
			}
			buffer[k + 0u] = "0123456789ABCDEF"[((unsigned char *)data)[i + j] >> 4];
			buffer[k + 1u] = "0123456789ABCDEF"[((unsigned char *)data)[i + j] & 15];
			buffer[k + 2u] = ' ';
			k += 3u;
		}

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size)
				break;
			if (z == 0) {
				if (((unsigned char *)data)[i + j] > 31u && ((unsigned char *)data)[i + j] < 127u)
					buffer[k++] = ((unsigned char *)data)[i + j];
				/* FIXME: i + j + 1u가 size보다 큰 경우, 운이 아주 나쁘면 memory access violation 발생 */
				else if (((((unsigned char *)data)[i + j] >= 0xB0u && ((unsigned char *)data)[i + j] <= 0xC8u) && (((unsigned char *)data)[i + j + 1u] >= 0xA1u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu)) ||
					((((unsigned char *)data)[i + j] >= 0x81u && ((unsigned char *)data)[i + j] <= 0xA0u) && ((((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x5Au) || (((unsigned char *)data)[i + j + 1u] >= 0x61u && ((unsigned char *)data)[i + j + 1u] <= 0x7Au) || (((unsigned char *)data)[i + j + 1u] >= 0x81u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu))) ||
					((((unsigned char *)data)[i + j] >= 0xA1u && ((unsigned char *)data)[i + j] <= 0xC5u) && ((((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x5Au) || (((unsigned char *)data)[i + j + 1u] >= 0x61u && ((unsigned char *)data)[i + j + 1u] <= 0x7Au) || (((unsigned char *)data)[i + j + 1u] >= 0x81u && ((unsigned char *)data)[i + j + 1u] <= 0xA0u))) ||
					(((unsigned char *)data)[i + j] == 0xC6u && (((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x52u)) ||
					(((unsigned char *)data)[i + j] == 0xA4u && (((unsigned char *)data)[i + j + 1u] >= 0xA1u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu))) {
					if (j != 15u) {
						*(unsigned short *)&buffer[k] = *(unsigned short *)&((unsigned char *)data)[i + j];
						k += 2u;
						j += 1u;
					} else {
						buffer[k++] = '.';
						z = 1;
					}
				} else
					buffer[k++] = '.';
			} else {
				buffer[k++] = '.';
				z = 0;
			}
		}

		VIRTUALIZER1_START
		{
			SECURE_API(s_OutputDebugStringA)((*(unsigned int *)&buffer[k] = 0, buffer));
		}
		VIRTUALIZER_END
	}
}

__declspec(naked) void* GetModule(unsigned int hash)
{
	__asm {
		push esi
		push edi
		VIRTUALIZER1_START
		{
			mov edx, fs:30h			/* Put address of PEB in EDX */
			mov edx, [edx+0Ch]		/* Get pointer to _PEB.Ldr */
			mov edx, [edx+14h]		/* _PEB_LDR_DATA.InMemoryOrderModuleList.Flink */
		}
		VIRTUALIZER_END
		cld

$NEXT_MODULE:
		xor eax, eax
		test edx, edx
		jz short $EXIT
		mov esi, [edx+28h]		/* _LDR_MODULE.BaseDllName.Buffer (Unicode) */
		mov ecx, [edx+24h]
		and ecx, 0FFFFh
		test esi, esi
		jz short $EXIT
		xor edi, edi

$NEXT_CHAR:
		lodsb
		cmp al, 'a'				/* Check if character is uppercase */
		jl short $NOT_LOWERCASE
		sub al, 20h				/* uppercase character */

$NOT_LOWERCASE:
		ror edi, 0Dh
		add edi, eax			/* Add uppercase'd character to EDI */
		loop short $NEXT_CHAR
		cmp edi, [esp+0Ch]		/* Check EDI against checksum */
		mov eax, [edx+10h]		/* _LDR_MODULE.DllBase */
		mov edx, [edx]			/* _LDR_MODULE.Flink */
		jnz short $NEXT_MODULE	/* Move onto the next module name if this one wasn't the dll */

$EXIT:
		pop edi
		pop esi
		retn
	}
}

__declspec(naked) void EnumModuleHash(void)
{
	static char msg[] = "%p [%S]";

	__asm {
		push esi
		push edi
		VIRTUALIZER1_START
		{
			mov edx, fs:30h			/* Put address of PEB in EDX */
			mov edx, [edx+0Ch]		/* Get pointer to _PEB.Ldr */
			mov edx, [edx+14h]		/* _PEB_LDR_DATA.InMemoryOrderModuleList.Flink */
		}
		VIRTUALIZER_END
		cld

$NEXT_MODULE:
		xor eax, eax
		test edx, edx
		jz short $EXIT
		mov esi, [edx+28h]		/* _LDR_MODULE.BaseDllName.Buffer (Unicode) */
		mov ecx, [edx+24h]
		and ecx, 0FFFFh
		test esi, esi
		jz short $EXIT
		xor edi, edi

$NEXT_CHAR:
		lodsb
		cmp al, 'a'				/* Check if character is uppercase */
		jl short $NOT_LOWERCASE
		sub al, 20h				/* uppercase character */

$NOT_LOWERCASE:
		ror edi, 0Dh
		add edi, eax			/* Add uppercase'd character to EDI */
		loop short $NEXT_CHAR

		push edx
		mov eax, [edx+28h]
		push eax
		push edi
		push offset msg
		call dbg
		add esp, 0Ch
		pop edx

		mov edx, [edx]			/* _LDR_MODULE.Flink */
		jmp short $NEXT_MODULE	/* Move onto the next module name if this one wasn't the dll */

$EXIT:
		pop edi
		pop esi
		retn
	}
}

void GetExportDescSub(void *module, void *fn)
{
	IMAGE_EXPORT_DIRECTORY *e = (IMAGE_EXPORT_DIRECTORY *)((DWORD_PTR)module + ((IMAGE_NT_HEADERS *)((DWORD_PTR)module + ((IMAGE_DOS_HEADER *)module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	
	if (e != module) {
		DWORD i;
		for (i = e->NumberOfNames; i;)
			if ((void *)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfFunctions))[((WORD *)((DWORD_PTR)module + e->AddressOfNameOrdinals))[--i]]) == fn)
				dbg("SYMBOL [%s]", (DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfNames))[i]);
		for (i = e->NumberOfFunctions; i;)
			if ((void *)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfFunctions))[--i]) == fn)
				dbg("ORDINAL [#%u]", e->Base + i);
	}
}

void GetExportDesc(void *addr)
{
	MEMORY_BASIC_INFORMATION mbi;
	char data[256];

	if (SECURE_API(s_VirtualQuery)(addr, &mbi, sizeof(mbi)) != 0) {
		if (SECURE_API(s_GetModuleFileNameA)(mbi.AllocationBase, data, sizeof(data))) {
			dbg("<!--- %s+0x%X (%p)", data, (unsigned int)addr - (unsigned int)mbi.AllocationBase, addr);
			GetExportDescSub(mbi.AllocationBase, addr);
			dbg("%s", "--->");
		} else
			dbg("%s() %u", "GetModuleFileNameA", SECURE_API(s_GetLastError)());
	} else
		dbg("%s() %u", "VirtualQuery", SECURE_API(s_GetLastError)());
}

PROC GetExportAddressSub(void *module, const char *name)
{
	IMAGE_EXPORT_DIRECTORY *e;

	if (module) {
		VIRTUALIZER1_START
		{
			e = (IMAGE_EXPORT_DIRECTORY *)((DWORD_PTR)module + ((IMAGE_NT_HEADERS *)((DWORD_PTR)module + ((IMAGE_DOS_HEADER *)module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		}
		VIRTUALIZER_END
		if (e != module)
			if (((DWORD_PTR)(name)) & ~65535) {
				DWORD i = e->NumberOfNames;
				while (i)
					if (strcmp((char *)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfNames))[--i]), name) == 0)
						return (PROC)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfFunctions))[((WORD *)((DWORD_PTR)module + e->AddressOfNameOrdinals))[i]]);
			} else if ((DWORD_PTR)name >= e->Base && ((DWORD_PTR)name - e->Base) < e->NumberOfFunctions)
				return (PROC)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfFunctions))[(DWORD_PTR)name - e->Base]);
	}
	return NULL;
}

PROC GetExportAddress(void *module, const char *name)
{
	PROC i;

	VIRTUALIZER1_START
	{
		if ((i = GetExportAddressSub(module, name)) != NULL && *(unsigned int *)i == 'LDTN')
			i = GetExportAddressSub(GetModule(0x3CFA685Du/*NTDLL*/), &((char *)i)[6]);
	}
	VIRTUALIZER_END

	return i;
}

DWORD* GetExportAddressBase(void *module, const char *name)
{
	IMAGE_EXPORT_DIRECTORY *e;

	if (module) {
		VIRTUALIZER1_START
		{
			e = (IMAGE_EXPORT_DIRECTORY *)((DWORD_PTR)module + ((IMAGE_NT_HEADERS *)((DWORD_PTR)module + ((IMAGE_DOS_HEADER *)module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		}
		VIRTUALIZER_END
		if (e != module)
			if (((DWORD_PTR)(name)) & ~65535) {
				DWORD i = e->NumberOfNames;
				while (i)
					if (strcmp((char *)((DWORD_PTR)module + ((DWORD *)((DWORD_PTR)module + e->AddressOfNames))[--i]), name) == 0)
						return &((DWORD *)((DWORD_PTR)module + e->AddressOfFunctions))[((WORD *)((DWORD_PTR)module + e->AddressOfNameOrdinals))[i]];
			} else if ((DWORD_PTR)name >= e->Base && ((DWORD_PTR)name - e->Base) < e->NumberOfFunctions)
				return &((DWORD *)((DWORD_PTR)module + e->AddressOfFunctions))[(DWORD_PTR)name - e->Base];
	}
	return NULL;
}

PROC HookExportProc(void *module, const char *name, void *proc)
{
	DWORD *base, original, protect;

	VIRTUALIZER1_START
	{
		base = GetExportAddressBase(module, name);
	}
	VIRTUALIZER_END

	if (base == NULL)
		return NULL;

	VIRTUALIZER1_START
	{
		SECURE_API(s_VirtualProtect)(base, 4, PAGE_READWRITE, &protect);
		original = *base;
		*base = (DWORD)((DWORD_PTR)proc - (DWORD_PTR)module);
		SECURE_API(s_VirtualProtect)(base, 4, protect, &protect);
	}
	VIRTUALIZER_END
	return (PROC)((DWORD_PTR)module + original);
}

PROC GetImportAddress(void *module, const char *name, const char *fn)
{
	IMAGE_IMPORT_DESCRIPTOR *import;

	if (module) {
		VIRTUALIZER1_START
		{
			import = (IMAGE_IMPORT_DESCRIPTOR *)((DWORD_PTR)module + ((IMAGE_NT_HEADERS *)((DWORD_PTR)module + ((IMAGE_DOS_HEADER *)module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		}
		VIRTUALIZER_END
		if (import != module)
			while (import->Name) {
				if (_stricmp((char *)((DWORD_PTR)module + import->Name), name) == 0) {
					if (import->OriginalFirstThunk) {
						IMAGE_THUNK_DATA *hint = (IMAGE_THUNK_DATA *)((DWORD_PTR)module + import->OriginalFirstThunk);
						IMAGE_THUNK_DATA *thunk = (IMAGE_THUNK_DATA *)((DWORD_PTR)module + import->FirstThunk);
						if (((DWORD_PTR)(fn)) & ~65535) {
							while (hint->u1.AddressOfData) {
								if (IMAGE_SNAP_BY_ORDINAL(hint->u1.Ordinal) == 0)
									if (strcmp((char *)((IMAGE_IMPORT_BY_NAME *)((DWORD_PTR)module + hint->u1.AddressOfData))->Name, fn) == 0)
										return (PROC)thunk->u1.Function;
								++thunk;
								++hint;
							}
						} else {
							while (hint->u1.AddressOfData) {
								if (IMAGE_SNAP_BY_ORDINAL(hint->u1.Ordinal) && IMAGE_ORDINAL(hint->u1.Ordinal) == (DWORD_PTR)fn)
									return (PROC)thunk->u1.Function;
								++thunk;
								++hint;
							}
						}
					}
					break;
				}
				++import;
			}
	}
	return NULL;
}

DWORD_PTR* GetImportAddressBase(void *module, const char *name, const char *fn)
{
	IMAGE_IMPORT_DESCRIPTOR *import;

	if (module) {
		VIRTUALIZER1_START
		{
			import = (IMAGE_IMPORT_DESCRIPTOR *)((DWORD_PTR)module + ((IMAGE_NT_HEADERS *)((DWORD_PTR)module + ((IMAGE_DOS_HEADER *)module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		}
		VIRTUALIZER_END
		if (import != module)
			while (import->Name) {
				if (_stricmp((char *)((DWORD_PTR)module + import->Name), name) == 0) {
					if (import->OriginalFirstThunk) {
						IMAGE_THUNK_DATA *hint = (IMAGE_THUNK_DATA *)((DWORD_PTR)module + import->OriginalFirstThunk);
						IMAGE_THUNK_DATA *thunk = (IMAGE_THUNK_DATA *)((DWORD_PTR)module + import->FirstThunk);
						if (((DWORD_PTR)(fn)) & ~65535) {
							while (hint->u1.AddressOfData) {
								if (IMAGE_SNAP_BY_ORDINAL(hint->u1.Ordinal) == 0)
									if (strcmp((char *)((IMAGE_IMPORT_BY_NAME *)((DWORD_PTR)module + hint->u1.AddressOfData))->Name, fn) == 0)
										return &thunk->u1.Function;
								++thunk;
								++hint;
							}
						} else {
							while (hint->u1.AddressOfData) {
								if (IMAGE_SNAP_BY_ORDINAL(hint->u1.Ordinal) && IMAGE_ORDINAL(hint->u1.Ordinal) == (DWORD_PTR)fn)
									return &thunk->u1.Function;
								++thunk;
								++hint;
							}
						}
					}
					break;
				}
				++import;
			}
	}
	return NULL;
}

PROC HookImportProc(void *module, const char *name, const char *fn, void *proc)
{
	DWORD_PTR *base, original;
	DWORD protect;

	VIRTUALIZER1_START
	{
		base = GetImportAddressBase(module, name, fn);
	}
	VIRTUALIZER_END

	if (base == NULL)
		return NULL;

	VIRTUALIZER1_START
	{
		SECURE_API(s_VirtualProtect)(base, 4, PAGE_READWRITE, &protect);
		original = *base;
		*base = (DWORD_PTR)proc;
		SECURE_API(s_VirtualProtect)(base, 4, protect, &protect);
	}
	VIRTUALIZER_END

	return (PROC)original;
}

void HookCALL(void *addr, void *fn, void *code)
{
	DWORD protect;

	VIRTUALIZER1_START
	{
		if (SECURE_API(s_VirtualProtect)(addr, 5, PAGE_READWRITE, &protect)) {
			if (fn) {
				if (code)
					memcpy(code, addr, 5);
				*(unsigned char *)addr = 0xE8; /* CALL */
				*(unsigned int  *)((unsigned int)addr + 1u) = (unsigned int)fn - (unsigned int)addr - 5u;
			} else if (code)
				memcpy(addr, code, 5);
			SECURE_API(s_VirtualProtect)(addr, 5, protect, &protect);
			SECURE_API(s_FlushInstructionCache)(SECURE_API(s_GetCurrentProcess)(), addr, 5);
		}
	}
	VIRTUALIZER_END
}

void HookJMP(void *addr, void *fn, void *code)
{
	DWORD protect;

	VIRTUALIZER1_START
	{
		if (SECURE_API(s_VirtualProtect)(addr, 5, PAGE_READWRITE, &protect)) {
			if (fn) {
				if (code)
					memcpy(code, addr, 5);
				*(unsigned char *)addr = 0xE9; /* JMP */
				*(unsigned int  *)((unsigned int)addr + 1u) = (unsigned int)fn - (unsigned int)addr - 5u;
			} else if (code)
				memcpy(addr, code, 5);
			SECURE_API(s_VirtualProtect)(addr, 5, protect, &protect);
			SECURE_API(s_FlushInstructionCache)(SECURE_API(s_GetCurrentProcess)(), addr, 5);
		}
	}
	VIRTUALIZER_END
}

void vmemcpy(void *addr, const void *data, unsigned int size)
{
	DWORD protect;

	VIRTUALIZER1_START
	{
		if (SECURE_API(s_VirtualProtect)(addr, size, PAGE_READWRITE, &protect)) {
			memcpy(addr, data, size);
			SECURE_API(s_VirtualProtect)(addr, size, protect, &protect);
			SECURE_API(s_FlushInstructionCache)(SECURE_API(s_GetCurrentProcess)(), addr, size);
		}
	}
	VIRTUALIZER_END
}

void* _memscan(const void *image_data, unsigned int image_size, const void *pattern_data, unsigned int pattern_size)
{
	unsigned int i, j;

	if (image_size && pattern_size && pattern_size <= image_size)
		for (i = 0; i <= (image_size - pattern_size); ++i)
			for (j = pattern_size - 1u; ((unsigned char *)pattern_data)[j] == 0 || ((unsigned char *)pattern_data)[j] == ((unsigned char *)image_data)[i + j]; --j)
				if (j == 0)
					return (void *)((DWORD_PTR)image_data + i);
	return 0;
}

void* _memscan_ptr(const void *image_data, unsigned int image_size, const void *pattern_data, unsigned int pattern_size)
{
	unsigned int i;

	if (image_size && pattern_size && pattern_size <= image_size)
		for (i = 0; i <= (image_size - pattern_size); i += 4u)
			if (memcmp((void *)((DWORD_PTR)image_data + i), pattern_data, pattern_size) == 0)
				return (void *)((DWORD_PTR)image_data + i);
	return 0;
}

void* memscan(void *image_base, const void *pattern_data, unsigned int pattern_size)
{

	return _memscan(image_base, 0x2000000, pattern_data, pattern_size);
}

void* memscan_ptr(void *image_base, const void *pattern_data, unsigned int pattern_size)
{
	return _memscan_ptr(image_base, 0x2000000, pattern_data, pattern_size);
}

#define tolower(c) (((c) >= 'A' && (c) <= 'Z') ? ((c) | 32) : (c))
#define toupper(c) (((c) >= 'a' && (c) <= 'z') ? ((c) & ~32) : (c))

char* kstrstr(const char *text, const char *pattern)
{
	int i, j, T[64];

	if (*pattern == '\0')
		return (char *)text;
	
	/*T = (int *)_alloca(sizeof(int) * (strlen(pattern) + 1u));*/
	*T = -1;
	j = -1;
	i = 0;

	do {
		if (i == 63)
			__asm int 3;
		while (j > -1 && pattern[i] != pattern[j])
			j = T[j];
		++i, ++j;
		T[i] = j;
	} while (pattern[i]);

	for (i = 0, j = 0; text[i];) {
		while (j >= 0 && text[i] != pattern[j])
			j = T[j];
		++i, ++j;
		if (pattern[j] == '\0')
			return (char *)text + i - j;
	}

	return 0;
}

char* kstristr(const char *text, const char *pattern)
{
	int i, j, T[64];

	if (*pattern == '\0')
		return (char *)text;
	
	/*T = (int *)_alloca(sizeof(int) * (strlen(pattern) + 1u));*/
	*T = -1;
	j = -1;
	i = 0;

	do {
		if (i == 63)
			__asm int 3;
		while (j > -1 && tolower(pattern[i]) != tolower(pattern[j]))
			j = T[j];
		++i, ++j;
		T[i] = j;
	} while (pattern[i]);

	for (i = 0, j = 0; text[i];) {
		while (j >= 0 && tolower(text[i]) != tolower(pattern[j]))
			j = T[j];
		++i, ++j;
		if (pattern[j] == '\0')
			return (char *)text + i - j;
	}

	return 0;
}

__declspec(naked) void* thiscall0(void *f, void *a1)
{
	__asm {
		VIRTUALIZER1_START
		{
			push ecx
			mov ecx, [esp+4+8]
			mov eax, [esp+4+4]
			call eax
			pop ecx
		}
		VIRTUALIZER_END
		ret
	}
}

__declspec(naked) void* thiscall1(void *f, void *a1, void *a2)
{
	__asm {
		VIRTUALIZER1_START
		{
			push ecx
			push [esp+4+0Ch]
			mov ecx, [esp+8+8]
			mov eax, [esp+8+4]
			call eax
			pop ecx
		}
		VIRTUALIZER_END
		ret
	}
}

__declspec(naked) void* thiscall2(void *f, void *a1, void *a2, void *a3)
{
	// 애는 SendShopPacket에서 쓰기 때문에 가상화를 하지 않음
	__asm {
		push ecx
		push [esp+4+10h]
		push [esp+8+0Ch]
		mov ecx, [esp+0Ch+8]
		mov eax, [esp+0Ch+4]
		call eax
		pop ecx
		ret
	}
}

__declspec(naked) void* thiscall3(void *f, void *a1, void *a2, void *a3, void *a4)
{
	__asm {
		VIRTUALIZER1_START
		{
			push ecx
			push [esp+4+14h]
			push [esp+8+10h]
			push [esp+0Ch+0Ch]
			mov ecx, [esp+10h+8]
			mov eax, [esp+10h+4]
			call eax
			pop ecx
		}
		VIRTUALIZER_END
		ret
	}
}

__declspec(naked) void* thiscall4(void *f, void *a1, void *a2, void *a3, void *a4, void *a5)
{
	__asm {
		VIRTUALIZER1_START
		{
			push ecx
			push [esp+4+18h]
			push [esp+8+14h]
			push [esp+0Ch+10h]
			push [esp+10h+0Ch]
			mov ecx, [esp+14h+8]
			mov eax, [esp+14h+4]
			call eax
			pop ecx
		}
		VIRTUALIZER_END
		ret
	}
}

__declspec(naked) void* thiscall5(void *f, void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
	__asm {
		VIRTUALIZER1_START
		{
			push ecx
			push [esp+4+1Ch]
			push [esp+8+18h]
			push [esp+0Ch+14h]
			push [esp+10h+10h]
			push [esp+14h+0Ch]
			mov ecx, [esp+18h+8]
			mov eax, [esp+18h+4]
			call eax
			pop ecx
		}
		VIRTUALIZER_END
		ret
	}
}

/* http://research.microsoft.com/en-us/um/redmond/projects/invisible/src/crt/memmove.c.htm */
void* _memmove(void *d, const void *s, unsigned int c)
{
    if (d <= s)
        return memcpy(d,s,c);

    if ((((unsigned int) s) | ((unsigned int) d) | c) & (sizeof(UINT) - 1)) {

        BYTE *pS = (BYTE *) s;
        BYTE *pD = (BYTE *) (((unsigned int) d) + c);
        BYTE *pE = (BYTE *) (((unsigned int) s) + c);

        while (pS != pE)
            *(--pD) = *(--pE);
    }
    else {

        UINT *pS = (UINT *) s;
        UINT *pD = (UINT *) (BYTE *) (((unsigned int) d) + c);
        UINT *pE = (UINT *) (BYTE *) (((unsigned int) s) + c);

        while (pS != pE)
            *(--pD) = *(--pE);

    }

    return d;
}

unsigned int _wcscpy(void *dst, const void *src)
{
	unsigned int i;

	for (i = 0; ; ++i)
		if ((((unsigned short *)dst)[i] = ((unsigned short *)src)[i]) == 0)
			return i;
}