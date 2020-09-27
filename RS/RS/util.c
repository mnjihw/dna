#include "common.h"


void dbg(const char *fmt, ...)
{
	char msg[1024];

	OutputDebugStringA((wvsprintf(msg, fmt, (va_list)((unsigned int)&fmt + sizeof(void *))), msg));

}




unsigned int atoi32(const char *string)
{
	unsigned int value = 0;

	while (*string >= '0' && *string <= '9')
		value = value * 10u + (*string++ - '0');

	return value;
}