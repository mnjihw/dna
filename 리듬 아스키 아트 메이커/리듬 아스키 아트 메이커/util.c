#include "common.h"


void dbg(const char *fmt, ...)
{
	char msg[1024];

	OutputDebugString((wvsprintf(msg, fmt, (va_list)((unsigned int)&fmt + sizeof(void *))), msg));
}



void Clipboard(char *source)
{
	int ok = OpenClipboard(NULL);
	if (!ok)
		return;

	HGLOBAL clipbuffer;
	char * buffer;


	EmptyClipboard();
	int srclen = strlen(source) + 1;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, srclen);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy_s(buffer, srclen, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

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


		OutputDebugStringA((*(unsigned int *)&buffer[k] = 0, buffer));
	}
}
