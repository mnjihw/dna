#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "resource.h"
#include <stdio.h>
#include <WinSock2.h>
#include <mstcpip.h>
#include <stdbool.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Comctl32.lib")
#define WM_SOCKET (WM_USER + 1)


typedef struct
{
	char name[32];
	char time_remaning[32];
}result_t;


typedef struct
{
	char *exename;
	char *uuid;
	char *username;
	char *version;
	SYSTEMTIME st;
	result_t *result;
}param_t;

typedef struct
{
	SOCKET s;
	bool processing;
	char ip[16];
	char nick[13];
	char username[128];
	char name[32];
	char server[16];
	char exename[32];

	int packet_size;
	char packet_data[1024 * 16];
}Client;

typedef struct
{
	int iSubItem;
	bool ascending;

}sortinfo_t;

void REFRESH_USERLIST();
void dbg(const char *fmt, ...);
void ListView_GetSelectedItem(HWND hwnd, int *buf);
void SendData(Client *client, const char *fmt, ...);


