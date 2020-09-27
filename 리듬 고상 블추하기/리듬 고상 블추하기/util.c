#include "common.h"

void vdbg(const char *fmt, va_list arg)
{
	char msg[4096];
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), msg, wvsprintfA(msg, fmt, arg), (DWORD *) &msg[1024], NULL);
}

void dbg(const char *fmt, ...)
{
	vdbg(fmt, (va_list) (((unsigned int) &fmt) + sizeof(void*)));
}

void dbg2(const char *fmt, ...)
{
	char msg[1024];
	OutputDebugStringA((wvsprintf(msg, fmt, (va_list) ((unsigned int) &fmt + sizeof(void *))), msg));
}




void dump(const void *data, unsigned int size)
{
	static const char hex [] = "0123456789ABCDEF";
	unsigned int off, i;
	char *p, buffer[128];
	unsigned int fucked = 0;

	if (size & ~255)
		size = 256;

	for (off = 0; off < size; off += 16u) {
		p = buffer;
		i = off;
		do {
			*(p + 0) = hex[*((unsigned char *) data + i) >> 4];
			*(p + 1) = hex[*((unsigned char *) data + i) & 15];
			*(p + 2) = ' ';
			p += 3;
			if (++i >= size) {
				while (i < off + 16u) {
					*(unsigned *) p = '    ';
					p += 3;
					++i;
				}
				break;
			}
		} while (i < off + 16u);
		i = off;
		do {
			if (fucked)
				--fucked;
			if (*((unsigned char *) data + i) > 31 && *((unsigned char *) data + i) < 127)
				*(p++) = *((char *) data + i);
			else if ((*((unsigned char *) data + i) & 0x80) &&
				(((*((unsigned char *) data + i + 0) == 0xA4 && (*((unsigned char *) data + i + 1) >= 0xA1 && *((unsigned char *) data + i + 1) <= 0xFE))) ||
				((*((unsigned char *) data + i + 0) >= 0xB0u && *((unsigned char *) data + i + 0) <= 0xC8u) && (*((unsigned char *) data + i + 1) >= 0xA1u && *((unsigned char *) data + i + 1) <= 0xFEu)) ||
				((*((unsigned char *) data + i + 0) >= 0x81 && *((unsigned char *) data + i + 0) <= 0xA0) && ((*((unsigned char *) data + i + 1) >= 0x41 && *((unsigned char *) data + i + 1) <= 0x5A) || (*((unsigned char *) data + i + 1) >= 0x61 && *((unsigned char *) data + i + 1) <= 0x7A) || (*((unsigned char *) data + i + 1) >= 0x81 && *((unsigned char *) data + i + 1) <= 0xFE))) ||
				((*((unsigned char *) data + i + 0) >= 0xA1 && *((unsigned char *) data + i + 0) <= 0xC5) && ((*((unsigned char *) data + i + 1) >= 0x41 && *((unsigned char *) data + i + 1) <= 0x5A) || (*((unsigned char *) data + i + 1) >= 0x61 && *((unsigned char *) data + i + 1) <= 0x7A) || (*((unsigned char *) data + i + 1) >= 0x81 && *((unsigned char *) data + i + 1) <= 0xA0))) ||
				(*((unsigned char *) data + i + 0) == 0xC6 && (*((unsigned char *) data + i + 1) >= 0x41 && *((unsigned char *) data + i + 1) <= 0x52)))) {
				if ((i + 1) < off + 16u) {
					if (fucked) {
						*(p++) = '.';
					}
					else {
						*(p++) = *((char *) data + i);
						*(p++) = *((char *) data + ++i);
					}
				}
				else {
					*(p++) = '.';
					fucked = 2;
				}
			}
			else
				*(p++) = '.';
			if (++i >= size) {
				while (i < off + 16u) {
					*(p++) = ' ';
					++i;
				}
				break;
			}
		} while (i < off + 16u);
		*(p++) = '\n';
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, (DWORD) (p - buffer), (LPDWORD) p, NULL);
	}
}

unsigned int atoi32(const char *string)
{
	unsigned int value = 0;

	while (*string >= '0' && *string <= '9')
		value = value * 10u + (*string++ - '0');

	return value;
}