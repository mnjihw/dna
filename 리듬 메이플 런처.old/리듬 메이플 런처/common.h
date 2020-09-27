#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4028)
#include "resource.h"
#include <Windows.h>
#include <ShlObj.h>
#include <TlHelp32.h>
#include <MsHTML.h>

VOID MessageLoop();

long EmbedBrowserObject(HWND hwnd);
void UnEmbedBrowserObject(HWND hwnd);
long DisplayHTMLPage(HWND hwnd, LPTSTR webPageName);