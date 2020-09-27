#include "common.h"

void dbg(const char *fmt, ...)
{
	char msg[1024];

	OutputDebugStringA((wvsprintf(msg, fmt, (va_list)((unsigned int)&fmt + sizeof(void *))), msg));
}
