#include "common.h"

void dbg(const char *fmt, ...)
{
	char msg[1024];
	
	OutputDebugStringA((wvsprintf(msg, fmt, (va_list) ((unsigned int) &fmt + sizeof(void *))), msg));

}


__declspec(naked) void* thiscall1(void *f, void *a1, void *a2)
{
	__asm
	{
		push ecx
		push[esp + 4 + 0Ch]
			mov ecx, [esp + 8 + 8]
			mov eax, [esp + 8 + 4]
			call eax
			pop ecx

			ret
	}
}



__declspec(naked) void* thiscall2(void *f, void *a1, void *a2, void *a3)
{
	// 애는 SendShopPacket에서 쓰기 때문에 가상화를 하지 않음
	__asm
	{
		push ecx
		push[esp + 4 + 10h]
			push[esp + 8 + 0Ch]
			mov ecx, [esp + 0Ch + 8]
			mov eax, [esp + 0Ch + 4]
			call eax
			pop ecx
			ret
	}
}

void DisplayPacket(const void *data, unsigned int size, HWND hwnd, unsigned int idx)
{
	static const char hex [] = "0123456789ABCDEF";
	unsigned int off, i;
	char *p, buffer[128];
	char buf[65536];
	unsigned int fucked = 0, len = 0;

	memset(buf, 0, sizeof(buf));
	if (size & ~511) //255 256이었다
		size = 512;

	for (off = 0; off < size; off += 16u)
	{
		p = buffer;
		i = off;
		do
		{
			*(p + 0) = hex[*((unsigned char *) data + i) >> 4];
			*(p + 1) = hex[*((unsigned char *) data + i) & 15];
			*(p + 2) = ' ';
			p += 3;
			if (++i >= size)
			{
				while (i < off + 16u)
				{
					*(unsigned *) p = '    ';
					p += 3;
					++i;
				}
				break;
			}
		} while (i < off + 16u);
		i = off;


		memcpy(&buf[len], buffer, (DWORD) (p - buffer));
		len += (DWORD) (p - buffer);

	}
	for (i = strlen(buf) - 1; buf[i] == ' '; --i);
	buf[i + 1] = '\0';


	ListView_SetItemText(hwnd, idx, 2, buf);
	memset(buf, 0, sizeof(buf));
	
	for (off = 0, len = 0; off < size; off += 16u)
	{
		p = buffer;
		i = off;
		do
		{
			if (fucked)
				--fucked;
			if (*((unsigned char *) data + i) > 31 && *((unsigned char *) data + i) < 127)
				*(p++) = *((char *) data + i);
			else if ((*((unsigned char *) data + i) & 0x80) &&
				(((*((unsigned char *) data + i + 0) == 0xA4 && (*((unsigned char *) data + i + 1) >= 0xA1 && *((unsigned char *) data + i + 1) <= 0xFE))) ||
				((*((unsigned char *) data + i + 0) >= 0xB0u && *((unsigned char *) data + i + 0) <= 0xC8u) && (*((unsigned char *) data + i + 1) >= 0xA1u && *((unsigned char *) data + i + 1) <= 0xFEu)) ||
				((*((unsigned char *) data + i + 0) >= 0x81 && *((unsigned char *) data + i + 0) <= 0xA0) && ((*((unsigned char *) data + i + 1) >= 0x41 && *((unsigned char *) data + i + 1) <= 0x5A) || (*((unsigned char *) data + i + 1) >= 0x61 && *((unsigned char *) data + i + 1) <= 0x7A) || (*((unsigned char *) data + i + 1) >= 0x81 && *((unsigned char *) data + i + 1) <= 0xFE))) ||
				((*((unsigned char *) data + i + 0) >= 0xA1 && *((unsigned char *) data + i + 0) <= 0xC5) && ((*((unsigned char *) data + i + 1) >= 0x41 && *((unsigned char *) data + i + 1) <= 0x5A) || (*((unsigned char *) data + i + 1) >= 0x61 && *((unsigned char *) data + i + 1) <= 0x7A) || (*((unsigned char *) data + i + 1) >= 0x81 && *((unsigned char *) data + i + 1) <= 0xA0))) ||
				(*((unsigned char *) data + i + 0) == 0xC6 && (*((unsigned char *) data + i + 1) >= 0x41 && *((unsigned char *) data + i + 1) <= 0x52))))
			{
				if ((i + 1) < off + 16u)
				{
					if (fucked)
					{
						*(p++) = '.';
					}
					else
					{
						*(p++) = *((char *) data + i);
						*(p++) = *((char *) data + ++i);
					}
				}
				else
				{
					*(p++) = '.';
					fucked = 2;
				}
			}
			else
				*(p++) = '.';
			if (++i >= size)
			{
				while (i < off + 16u)
				{
					*(p++) = ' ';
					++i;
				}
				break;
			}
		} while (i < off + 16u);

		memcpy(&buf[len], buffer, (DWORD) (p - buffer));
		len += (DWORD) (p - buffer);
	}
	for (i = strlen(buf) - 1; buf[i] == ' '; --i);
	buf[i + 1] = '\0';

	ListView_SetItemText(hwnd, idx, 3, buf);
	if (ListView_GetTopIndex(hwnd) + ListView_GetCountPerPage(hwnd) + 1 == ListView_GetItemCount(hwnd))
		ListView_EnsureVisible(hwnd, idx, FALSE);
	
	
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
					*(unsigned int *) &buffer[k] = '    ';
					k += 3u;
				}
				break;
			}
			buffer[k + 0u] = "0123456789ABCDEF"[((unsigned char *) data)[i + j] >> 4];
			buffer[k + 1u] = "0123456789ABCDEF"[((unsigned char *) data)[i + j] & 15];
			buffer[k + 2u] = ' ';
			k += 3u;
		}

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size)
				break;
			if (z == 0) {
				if (((unsigned char *) data)[i + j] > 31u && ((unsigned char *) data)[i + j] < 127u)
					buffer[k++] = ((unsigned char *) data)[i + j];
				/* FIXME: i + j + 1u가 size보다 큰 경우, 운이 아주 나쁘면 memory access violation 발생 */
				else if (((((unsigned char *) data)[i + j] >= 0xB0u && ((unsigned char *) data)[i + j] <= 0xC8u) && (((unsigned char *) data)[i + j + 1u] >= 0xA1u && ((unsigned char *) data)[i + j + 1u] <= 0xFEu)) ||
					((((unsigned char *) data)[i + j] >= 0x81u && ((unsigned char *) data)[i + j] <= 0xA0u) && ((((unsigned char *) data)[i + j + 1u] >= 0x41u && ((unsigned char *) data)[i + j + 1u] <= 0x5Au) || (((unsigned char *) data)[i + j + 1u] >= 0x61u && ((unsigned char *) data)[i + j + 1u] <= 0x7Au) || (((unsigned char *) data)[i + j + 1u] >= 0x81u && ((unsigned char *) data)[i + j + 1u] <= 0xFEu))) ||
					((((unsigned char *) data)[i + j] >= 0xA1u && ((unsigned char *) data)[i + j] <= 0xC5u) && ((((unsigned char *) data)[i + j + 1u] >= 0x41u && ((unsigned char *) data)[i + j + 1u] <= 0x5Au) || (((unsigned char *) data)[i + j + 1u] >= 0x61u && ((unsigned char *) data)[i + j + 1u] <= 0x7Au) || (((unsigned char *) data)[i + j + 1u] >= 0x81u && ((unsigned char *) data)[i + j + 1u] <= 0xA0u))) ||
					(((unsigned char *) data)[i + j] == 0xC6u && (((unsigned char *) data)[i + j + 1u] >= 0x41u && ((unsigned char *) data)[i + j + 1u] <= 0x52u)) ||
					(((unsigned char *) data)[i + j] == 0xA4u && (((unsigned char *) data)[i + j + 1u] >= 0xA1u && ((unsigned char *) data)[i + j + 1u] <= 0xFEu))) {
					if (j != 15u) {
						*(unsigned short *) &buffer[k] = *(unsigned short *) &((unsigned char *) data)[i + j];
						k += 2u;
						j += 1u;
					}
					else {
						buffer[k++] = '.';
						z = 1;
					}
				}
				else
					buffer[k++] = '.';
			}
			else {
				buffer[k++] = '.';
				z = 0;
			}
		}

		VIRTUALIZER_TIGER_BLACK_START
		{
			OutputDebugStringA((*(unsigned int *) &buffer[k] = 0, buffer));
		}
		VIRTUALIZER_TIGER_BLACK_END
	}
}


PROC GetExportAddressSub(void *module, const char *name)
{
	IMAGE_EXPORT_DIRECTORY *e;

	if (module) {
		VIRTUALIZER_TIGER_BLACK_START
		{
			e = (IMAGE_EXPORT_DIRECTORY *) ((DWORD_PTR) module + ((IMAGE_NT_HEADERS *) ((DWORD_PTR) module + ((IMAGE_DOS_HEADER *) module)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		}
			VIRTUALIZER_TIGER_BLACK_END
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

	VIRTUALIZER_TIGER_BLACK_START
	{
		if ((i = GetExportAddressSub(module, name)) != NULL && *(unsigned int *) i == 'LDTN')
		i = GetExportAddressSub(GetModule(0x3CFA685Du/*NTDLL*/), &((char *) i)[6]);
	}
		VIRTUALIZER_TIGER_BLACK_END

		return i;
}


__declspec(naked) void* GetModule(unsigned int hash)
{
	__asm {
		push esi
			push edi
			VIRTUALIZER_TIGER_BLACK_START
		{
			mov edx, fs:30h			/* Put address of PEB in EDX */
			mov edx, [edx + 0Ch]		/* Get pointer to _PEB.Ldr */
			mov edx, [edx + 14h]		/* _PEB_LDR_DATA.InMemoryOrderModuleList.Flink */
		}
		VIRTUALIZER_TIGER_BLACK_END
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


void* _memscan(const void *image_data, unsigned int image_size, const void *pattern_data, unsigned int pattern_size)
{
	unsigned int i, j;

	if (image_size && pattern_size && pattern_size <= image_size)
		for (i = 0; i <= (image_size - pattern_size); ++i)
			for (j = pattern_size - 1u; ((unsigned char *) pattern_data)[j] == 0 || ((unsigned char *) pattern_data)[j] == ((unsigned char *) image_data)[i + j]; --j)
				if (j == 0)
					return (void *) ((DWORD_PTR) image_data + i);
	return 0;
}



void* _memscan_ptr(const void *image_data, unsigned int image_size, const void *pattern_data, unsigned int pattern_size)
{
	unsigned int i;

	if (image_size && pattern_size && pattern_size <= image_size)
		for (i = 0; i <= (image_size - pattern_size); i += 4u)
			if (memcmp((void *) ((DWORD_PTR) image_data + i), pattern_data, pattern_size) == 0)
				return (void *) ((DWORD_PTR) image_data + i);
	return 0;
}

void* memscan(void *image_base, const void *pattern_data, unsigned int pattern_size)
{
	return _memscan(image_base, 0x2000000, pattern_data, pattern_size);
}

void* memscan_ptr(void *image_base, const void *pattern_data, unsigned int pattern_size)
{
	return _memscan_ptr(image_base, 0x20000000, pattern_data, pattern_size);
}

#define tolower(c) (((c) >= 'A' && (c) <= 'Z') ? ((c) | 32) : (c))
#define toupper(c) (((c) >= 'a' && (c) <= 'z') ? ((c) & ~32) : (c))

char* kstrstr(const char *text, const char *pattern)
{
	int i, j, T[64];

	if (*pattern == '\0')
		return (char *) text;

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
			return (char *) text + i - j;
	}

	return 0;
}

char* kstristr(const char *text, const char *pattern)
{
	int i, j, T[64];

	if (*pattern == '\0')
		return (char *) text;

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
			return (char *) text + i - j;
	}

	return 0;
}


/* http://research.microsoft.com/en-us/um/redmond/projects/invisible/src/crt/memmove.c.htm */
void* _memmove(void *d, const void *s, unsigned int c)
{
	if (d <= s)
		return memcpy(d, s, c);

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

	for (i = 0;; ++i)
		if ((((unsigned short *) dst)[i] = ((unsigned short *) src)[i]) == 0)
			return i;
}


unsigned int atoi32(const char *string)
{
	unsigned int value = 0;

	while (*string >= '0' && *string <= '9')
		value = value * 10u + (*string++ - '0');

	return value;
}




LPDWORD GetPointer2ProcAddress(HMODULE hModule, LPCSTR lpProcName)
{

	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER) hModule;

	PIMAGE_NT_HEADERS pNTHeader = RVA2OFFSET(PIMAGE_NT_HEADERS, pDOSHeader, pDOSHeader->e_lfanew);

	PIMAGE_EXPORT_DIRECTORY pExportDir = RVA2OFFSET(PIMAGE_EXPORT_DIRECTORY, hModule,
		pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);


	for (DWORD i = 0; i < pExportDir->NumberOfFunctions; ++i)
	{
		LPDWORD pENT = RVA2OFFSET(LPDWORD, hModule, (LPDWORD) pExportDir->AddressOfNames + i);
		LPWORD pAONO = RVA2OFFSET(LPWORD, hModule, (LPWORD) pExportDir->AddressOfNameOrdinals + i);
		LPDWORD pAOF = RVA2OFFSET(LPDWORD, hModule, (LPDWORD) pExportDir->AddressOfFunctions + *pAONO);

		if (HIWORD(lpProcName))
		{
			if (strcmp(RVA2OFFSET(LPCSTR, hModule, *pENT), lpProcName) == 0)
				return pAOF;
			else if (*pAONO == (WORD) lpProcName)
				return pAOF;
		}
	}


	return NULL;
}


BOOL HookExportedProc(HMODULE hModule, LPCSTR lpProcName, LPVOID* ppOldProcAddress, LPVOID pNewProcAddress)
{
	DWORD dwProtect;
	LPDWORD pAddress = GetPointer2ProcAddress(hModule, lpProcName);
	if (pAddress == NULL)
	{
		MessageBox(GetDesktopWindow(), "GetPointer2ProcAddress() 실패", 0, 0);
		return FALSE;
	}

	*ppOldProcAddress = MAKEPTR(LPVOID*, hModule, *pAddress);


	VirtualProtect(pAddress, sizeof(DWORD), PAGE_READWRITE, &dwProtect);
	*pAddress = (DWORD) pNewProcAddress - (DWORD) hModule;
	VirtualProtect(pAddress, sizeof(DWORD), dwProtect, &dwProtect);

	return TRUE;
}