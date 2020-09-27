#include "common.h"

void dbg(const char *fmt, ...)
{
	char msg[1024];

	OutputDebugStringA((wvsprintf(msg, fmt, (va_list)((unsigned int)&fmt + sizeof(void *))), msg));
}

void SelectLVItem(HWND hwnd, int sel)
{
	ListView_SetItemState(hwnd, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	ListView_SetItemState(hwnd, sel, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	ListView_EnsureVisible(hwnd, sel, FALSE);
}

unsigned int _strcpy(void *dst, const void *src)
{
	unsigned int i;

	for (i = 0; ; ++i)
		if ((((unsigned char *)dst)[i] = ((unsigned char *)src)[i]) == 0)
			return i;
}

UINT CalHP(UINT base_stat, UINT indiv_value, UINT effort_value, UINT level)
{
	return ((base_stat << 1) + (effort_value >> 2) + indiv_value) * level / 100 + 10 + level;
}


UINT CalOther(UINT base_stat, UINT indiv_value, UINT effort_value, UINT level, double nature_effect)
{
	return (UINT)((((base_stat << 1) + (effort_value >> 2) + indiv_value) * level / 100 + 5) * nature_effect);
}

bool IsStatUV(const char *str)
{
	if (strchr(str, 'U') || strchr(str, 'V'))
		return true;
	else
		return false;
}

bool IsStatZ(const char *str)
{
	if (strchr(str, 'Z'))
		return true;
	else
		return false;
}

void Clipboard(const char *source)
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