#include "common.h"
#define DUMPED_CODE_SIZE 4096

unsigned char dumped_code[DUMPED_CODE_SIZE];


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
						*(unsigned short *)&buffer[k] = *(unsigned short *) &((unsigned char *)data)[i + j];
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

		
		{
			OutputDebugStringA((*(unsigned int *)&buffer[k] = 0, buffer));
		}
			
	}
}



void main()
{
	unsigned int i, size, bytes;
	DWORD oldprotect;
	PROC func;
	t_disasm da;
	const char dll_name[] = "user32.dll";
	const char func_name[] = "MessageBoxA";

	if (!(func = GetProcAddress(GetModuleHandle(dll_name), func_name)))
	{
		printf("함수 찾지 못함!");
		return;
	}
	
	
	i = 0;

	while(1)
	{
		bytes = Disasm(&((BYTE*)func)[i], DUMPED_CODE_SIZE, (ulong)&((BYTE*)func)[i], &da, DISASM_FILE);

		if (da.error || (strstr(da.result, "RET") && !strstr(da.result, "RETF")))
			break;
		i += bytes;
	}
	size = i + 4;
	VirtualProtect(&dumped_code, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dumped_code, func, size);

	for (i = 0; i < size; ++i)
	{
		if (((BYTE*)func)[i] == 0xE8 || ((BYTE*)func)[i] == 0xE9)
			*(DWORD*)&dumped_code[i + 1] = &((BYTE*)func)[i] + *(DWORD*)&((BYTE*)func)[i + 1] - &dumped_code[i];
		// dump된 코드의 상대주소 = 원본 API 내부에서 상대주소로 날아가는 코드의 주소 + 해당 상대주소 - dump된 코드의 상대주소로 날아가는 코드의 주소
	}
	func = (PROC)dumped_code;

	for (;;)
	{
		func(0, "후킹 우회됨", 0, 0);
	}
	VirtualProtect(&dumped_code, size, oldprotect, &oldprotect);
}
