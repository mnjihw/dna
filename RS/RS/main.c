#define _CRT_SECURE_NO_WARNINGS
#include "common.h"

const char version[] = "1.10";

#define INTERNAL_IP "192.168.219.100"
#define EXTERNAL_IP "182.211.75.197"

unsigned int channel;
char nick[13], serverName[16];

server_t server[11];
SOCKET g_clntSock = INVALID_SOCKET;
char packet_data[4096];
int packet_size;

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#define CARD_WIDTH 45 //30
#define CARD_HEIGHT 60 //40
#define CARD_DISTANCE_W 56 //38
#define CARD_DISTANCE_H 67 //44

extern aes256_context ctx;
char username[128];

HINSTANCE g_hInst;
LPSTR lpszClass = "";
HWND g_hMP, g_hWnd, g_hList[4], g_hCheck[3], g_hTab, g_hStatic[3], g_hEdit[2], g_hButton[1], g_hTransWnd;
unsigned int startup, adapter_sel, breakloop, target_id, operating, promiscuous_mode, 해상도;
unsigned int SERVERINFO, USERINFO, SHOP_INFO, CHARACTER_APPEAR, CHATTING, CHATTING_WHISPER, CHATTING_EX, PINK_MSG;
unsigned char keycode, scancode;
unsigned int card[30], game_sel, _operating;
COLORREF color[15];

charinfo_t character_list[200];

void ListBox_Modify(HWND hwnd, int sel, char *str)
{
	SendMessage(hwnd, LB_DELETESTRING, sel, 0);
	SendMessage(hwnd, LB_INSERTSTRING, sel, (LPARAM)str);
}

void MouseClick(unsigned int x, unsigned int y, unsigned int k)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
	Sleep(10);
	for (unsigned int i = 0; i < k; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}

__declspec(naked) void nop1()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

void Clipboard(char *source)
{
	HGLOBAL clipbuffer;
	char * buffer;
	int srclen, ok;

	ok = OpenClipboard(NULL);
	if (!ok)
		return; 
	EmptyClipboard();
	srclen = strlen(source) + 1;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, srclen);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy_s(buffer, srclen, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

}




void server_init(server_t *s)
{
	strcpy(s[0].name, "스카니아");
	strcpy(s[1].name, "베라");
	strcpy(s[2].name, "루나");
	strcpy(s[3].name, "제니스");
	strcpy(s[4].name, "크로아");
	strcpy(s[5].name, "유니온");
	strcpy(s[6].name, "엘리시움");
	strcpy(s[7].name, "이노시스");
	strcpy(s[8].name, "레드");
	strcpy(s[9].name, "오로라");
	strcpy(s[10].name, "리부트");

	s[0].value = 0;
	s[1].value = 0x1;
	s[2].value = 0x3;
	s[3].value = 0x4;
	s[4].value = 0x5;
	s[5].value = 0xA;
	s[6].value = 0x10;
	s[7].value = 0x1D;
	s[8].value = 0x2B;
	s[9].value = 0x2C;
	s[10].value = 0x2D;
}


unsigned int isserver(unsigned char value)
{
	unsigned int i;

	for (i = 0; i < _countof(server); ++i)
	{
		if (server[i].value == value)
			return 1;
	}
	return 0;
}

unsigned int isnickname(char *str, unsigned int count)
{
	unsigned int i;

	for (i = 0; i < count; ++i)
	{
		if ((str[i] & 0x80) != 0x80 && !isalnum(str[i]))
			return 0;
	}
	return 1;

}

char *GetServerName(char value, char *str)
{
	unsigned int i;

	for (i = 0; i < _countof(server); ++i)
	{
		if (server[i].value == value)
		{
			strcpy(str, server[i].name);
			break;
		}
	}
	return str;
}


__declspec(naked) void nop2()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

void packet_handle(unsigned char *data, unsigned int size)
{
	unsigned int i, j, textSize, userId, selcount;
	char textData[128], buf[128];
	SYSTEMTIME st;
	NMHDR nmhdr;
	RECT rect;
	static unsigned int last_id, myid;
	static DWORD tick;

	if (*(unsigned short*)data == SERVERINFO)
	{
		if (*(unsigned char*)&data[2] > 19)
			return;
		else if (*(unsigned char*)&data[2] == 0)
			channel = 1;
		else if (*(unsigned char*)&data[2] == 1)
			channel = 20;
		else
			channel = *(unsigned char*)&data[2];
		if (channel == 20)
			wsprintf(buf, "현재 채널: %u세", channel);
		else
			wsprintf(buf, "현재 채널: %u채널", channel);
		ListBox_Modify(g_hList[0], 0, buf);
		memset(&character_list, 0, sizeof(character_list));
	
		
			
		for (i = 0; i < size - 7; ++i)
		{
			if (isserver(data[i]) && memcmp("\x00\x00\x00", &data[i + 1], 3) == 0 && data[i + 4] != 0 && isnickname(&data[i + 4], 4))
			{
				myid = *(unsigned int*)&data[i - 4];
				GetServerName(data[i], serverName);
				strcpy(nick, &data[i + 4]);
				SendData("N %s %s", serverName, nick);
				break;
			}
		}
				
		
	}
	else if (*(unsigned short*)data == USERINFO)
	{
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*)&data[2])
			{
				target_id = character_list[i].id;
				wsprintf(buf, "상대: %s", character_list[i].nick);
				ListBox_Modify(g_hList[0], 1, buf);
				break;
			}
		}
	}
	else if (*(unsigned short*)data == SHOP_INFO)
	{
		if (data[2] == 0x60)
		{
			j = data[4];
			switch (j)
			{
			case 0xC:
				game_sel = 0;
				break;
			case 0x14:
				game_sel = 1;
				break;
			case 0x1E:
				game_sel = 2;
				break;
			}
			for (i = 0; i < j; ++i)
				card[i] = *(unsigned int*)&data[i * 4 + 5];
			_operating = 1;
			InvalidateRect(g_hTransWnd, NULL, 0);
		}
	}
	else if (*(unsigned short*)data == CHARACTER_APPEAR)
	{
		if (character_list[_countof(character_list) - 1].id != 0)
			memset(&character_list, 0, sizeof(character_list));
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == 0)
			{
				character_list[i].id = *(unsigned int*)&data[2];
				j = *(unsigned short*)&data[7];
				memcpy(character_list[i].nick, &data[9], j);
				character_list[i].nick[j] = '\0';
				break;
			}
		}
	}
	else if (*(unsigned short*)data == CHARACTER_APPEAR + 1)
	{
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*)&data[2])
			{
				memset(&character_list[i], 0, sizeof(character_list[i]));
				break;
			}
		}
	}
	else if (*(unsigned short*)data == CHATTING)
	{
		userId = *(unsigned int*)&data[2];
		textSize = *(unsigned short*)&data[7];
		memcpy(textData, &data[9], textSize);
		textData[textSize] = '\0';
		//if (SendMessage(g_hCheck[1], BM_GETCHECK, BST_CHECKED, 0))
		if(operating && TabCtrl_GetCurSel(g_hTab) == 1)
		{
			GetWindowText(g_hEdit[0], buf, sizeof(buf));
			if (strcmp(buf, "") == 0 || strcmp(buf, " ") == 0 || buf[0] == '\0');
			else if (strstr(textData, buf))
			{
				GetLocalTime(&st);
				g_hMP = FindWindow("MapleStoryClass", NULL);
				if (g_hMP)
				while (GetForegroundWindow() != g_hMP)
					SetForegroundWindow(g_hMP);
				Sleep(200);
				keybd_event(keycode, scancode, 0, 0);
				keybd_event(keycode, scancode, KEYEVENTF_KEYUP, 0);
				wsprintf(buf, "%s %u시 %u분 %u초 심씀", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond);
				SendMessage(g_hList[2], LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList[2], LB_ADDSTRING, 0, (LPARAM)buf), 0);
				keybd_event(VK_UP, 0x72, 0, 0);
				Sleep(300);
				keybd_event(VK_UP, 0x72, KEYEVENTF_KEYUP, 0);
				Sleep(50);
				keybd_event(VK_DOWN, 0x80, 0, 0);
				Sleep(300);
				keybd_event(VK_DOWN, 0x80, KEYEVENTF_KEYUP, 0);
				Sleep(50);
				keybd_event(VK_LEFT, 0x75, 0, 0);
				Sleep(300);
				keybd_event(VK_LEFT, 0x75, KEYEVENTF_KEYUP, 0);
				Sleep(50);
				keybd_event(VK_RIGHT, 0x77, 0, 0);
				Sleep(300);
				keybd_event(VK_RIGHT, 0x77, KEYEVENTF_KEYUP, 0);
			}
		}
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == userId)
			{
				wsprintf(buf, "%s: %s", character_list[i].nick, textData);
				break;
			}
		}
		if (character_list[i].id != userId)
			wsprintf(buf, "0x%X: %s", userId, textData);
		 
		GetClientRect(g_hList[1], &rect);
		selcount = (int)(rect.bottom - rect.top) / SendMessage(g_hList[1], LB_GETITEMHEIGHT, 0, 0);
		
		if (SendMessage(g_hList[1], LB_GETTOPINDEX, 0, 0) + selcount == SendMessage(g_hList[1], LB_GETCOUNT, 0, 0))
		{
			SendMessage(g_hList[1], LB_ADDSTRING, 0, (LPARAM)buf);
			SendMessage(g_hList[1], WM_VSCROLL, SB_BOTTOM, 0);
		}
		else
			SendMessage(g_hList[1], LB_ADDSTRING, 0, (LPARAM)buf);
		if (!operating || TabCtrl_GetCurSel(g_hTab) != 3)
			return;
		if (promiscuous_mode == 1)
		{
			if (*(unsigned int*)&data[2] == myid)
				return;
			else if (*(unsigned int*)&data[2] == last_id || GetTickCount() - tick <= 1200)
				return;
		}
		else if (*(unsigned int*)&data[2] != target_id)
			return;
		i = *(unsigned short*)&data[7];

		memcpy(buf, &data[9], i);
		buf[i] = '\0';
		Clipboard(buf);
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
		Sleep(50);
		PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
		Sleep(50);
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		Sleep(100);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		
		tick = GetTickCount();
		last_id = *(unsigned int*)&data[2];
		Sleep(100);

	}
	else if (*(unsigned short*)data == CHATTING_EX)
	{
		i = *(unsigned short*)&data[3];
		memcpy(nick, &data[5], i);
		nick[i] = '\0';
		textSize = *(unsigned short*)&data[4 + i + 1];
		memcpy(textData, &data[4 + i + 1 + 2], textSize);
		textData[textSize] = '\0';
		switch (*(unsigned char*)&data[2])
		{
		case 0x2:
			wsprintf(buf, "(길드)%s: %s", nick, textData);
			break;
		case 0x3:
			wsprintf(buf, "(연합)%s: %s", nick, textData);
			break;
		}

		GetClientRect(g_hList[1], &rect);
		selcount = (int)(rect.bottom - rect.top) / SendMessage(g_hList[1], LB_GETITEMHEIGHT, 0, 0);

		if (SendMessage(g_hList[1], LB_GETTOPINDEX, 0, 0) + selcount == SendMessage(g_hList[1], LB_GETCOUNT, 0, 0))
		{
			SendMessage(g_hList[1], LB_ADDSTRING, 0, (LPARAM)buf);
			SendMessage(g_hList[1], WM_VSCROLL, SB_BOTTOM, 0);
		}
		else
			SendMessage(g_hList[1], LB_ADDSTRING, 0, (LPARAM)buf);
	}
	else if (*(unsigned short*)data == CHATTING_WHISPER)
	{
		if (*(unsigned char*)&data[2] == 0x12)
		{
			i = *(unsigned short*)&data[3];
			memcpy(nick, &data[5], i);
			nick[i] = '\0';
			textSize = *(unsigned short*)&data[4 + i + 1 + 2];
			memcpy(textData, &data[4 + i + 1 + 2 + 2], textSize);
			j = *(unsigned short*)&data[4 + i + 1];
			textData[textSize] = '\0';
			
			wsprintf(buf, "%s[%u%s채널 귓]: %s", nick, j == 0 ? 1 : j == 1 ? 20 : j, j == 1 ? "세": "", textData);
			
			
			
			GetClientRect(g_hList[1], &rect);
			selcount = (int)(rect.bottom - rect.top) / SendMessage(g_hList[1], LB_GETITEMHEIGHT, 0, 0);

			if (SendMessage(g_hList[1], LB_GETTOPINDEX, 0, 0) + selcount == SendMessage(g_hList[1], LB_GETCOUNT, 0, 0))
			{
				SendMessage(g_hList[1], LB_ADDSTRING, 0, (LPARAM)buf);
				SendMessage(g_hList[1], WM_VSCROLL, SB_BOTTOM, 0);
			}
			else
				SendMessage(g_hList[1], LB_ADDSTRING, 0, (LPARAM)buf);

		}
	}
	else if (*(unsigned short*)data == PINK_MSG)
	{
		if (*(unsigned char*)&data[2] == 0x0B)
		{
			if (strncmp(&data[5], "고용상점에서", strlen("고용상점에서")) == 0)
			{
				if (SendMessage(g_hCheck[0], BM_GETCHECK, 0, 0) == BST_CHECKED)
					PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
				memcpy(buf, &data[5], *(unsigned short*)&data[3]);
				buf[*(unsigned short*)&data[3]] = '\0';
				
				GetClientRect(g_hList[1], &rect);
				selcount = (int)(rect.bottom - rect.top) / SendMessage(g_hList[1], LB_GETITEMHEIGHT, 0, 0);

				if (SendMessage(g_hList[1], LB_GETTOPINDEX, 0, 0) + selcount == SendMessage(g_hList[1], LB_GETCOUNT, 0, 0))
				{
					SendMessage(g_hList[1], LB_ADDSTRING, 0, (LPARAM)buf);
					SendMessage(g_hList[1], WM_VSCROLL, SB_BOTTOM, 0);
				}
				else
					SendMessage(g_hList[1], LB_ADDSTRING, 0, (LPARAM)buf);

				TabCtrl_SetCurSel(g_hTab, 2);
				nmhdr.code = TCN_SELCHANGE;
				nmhdr.idFrom = IDC_TAB1;
				nmhdr.hwndFrom = g_hWnd;
				SendMessage(g_hWnd, WM_NOTIFY, 0, (LPARAM)&nmhdr);
			}
		}
	}

	
}

unsigned int packet_parse(session_t *s, unsigned char *data, unsigned int size)
{
	unsigned int i, j, k;
	unsigned char pkt[65536];

	if (s->auth == 0u)
	{
		if ((j = *(unsigned __int16 *)data) + 2u > size)
			return 0;
		i = j + 2u;
		j = *(unsigned __int16 *)&data[4];
		s->send_iv = *(unsigned __int32 *)&data[j + 6u];
		s->recv_iv = *(unsigned __int32 *)&data[j + 10u];
		s->ver = atoi32((data[j + 6u] = 0u, (char *)&data[6]));
		s->auth = ~0u;
	}
	else
		i = 0;

	k = s->recv_iv;

	for (; (size - i) >= (j = *(unsigned __int16 *)&data[i] ^ *(unsigned __int16 *)&data[i + 2u]) + 4u; i += j + 4u) {

		wzcrypt_decrypt(j, k, &data[i + 4u], pkt);
		packet_handle(pkt, j);
		k = wzcrypt_nextiv(k);
	}

	s->recv_iv = k;
	return i;
}

void packet_handler_sub(session_t *s, unsigned char *data, unsigned int size)
{
	unsigned int i;

	if (s->size)
	{ /* fragmented */
		if (size > sizeof(s->data) - s->size)
			__asm int 3;
		memcpy(&s->data[s->size], data, size);
		if ((i = packet_parse(s, s->data, s->size += size)) != 0)
		if ((s->size -= i) != 0)
			memcpy(s->data, &s->data[i], s->size);
	}
	else if ((i = packet_parse(s, data, size)) != size)
		memcpy(s->data, &data[i], s->size = size - i);
}

__declspec(naked) void nop3()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}
void packet_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
	unsigned int i, j;
	session_t *s;
	iphdr_t *ih;
	tcphdr_t *th;

	ih = (iphdr_t *)(pkt_data + 14u);
	th = (tcphdr_t *)(pkt_data + 14u + (ih->ihl << 2));

	if (th->fin != 0u || th->rst != 0u)
	{
		if ((s = session[th->dport]) != NULL)
		{
			session[th->dport] = NULL;
			HeapFree(GetProcessHeap(), 0u, s);
		}
	}
	else
	{
		if (th->syn != 0u)
		{
			if ((s = session[th->dport]) == NULL)
			{
				if ((s = HeapAlloc(GetProcessHeap(), 0u, sizeof(session_t))) == NULL)
					__asm int 3;
				session[th->dport] = s;
			}
			startup = 1;
			s->auth = 0u;
			s->size = 0u;
		}
		else if ((s = session[th->dport]) == NULL)
			return;
		if (i = ih->tot_len, (i = (((i & 0xFF) << 8) | (i >> 8)) - (j = (ih->ihl << 2) + (th->doff << 2))) != 0)
			packet_handler_sub(s, (char *)pkt_data + j + 14u, i);
	}
}




int FileNameCheck(HINSTANCE hInstance, char *fileName)
{
	int i;
	char myFileName[128];

	GetModuleFileName(hInstance, myFileName, sizeof(myFileName));
	for (i = strlen(myFileName); myFileName[i] != '\\'; --i);
	if (strcmp(&myFileName[i + 1], fileName))
	{
		rename(&myFileName[i + 1], fileName);
		return FALSE;
	}
	return TRUE;

}



LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
	TerminateProcess(GetCurrentProcess(), 0);
	return EXCEPTION_EXECUTE_HANDLER;
}

void vmemcpy(void *addr, const void *data, unsigned int size)
{
	DWORD protect;

	VIRTUALIZER_TIGER_BLACK_START
	{
		if (VirtualProtect(addr, size, PAGE_READWRITE, &protect))
		{
			memcpy(addr, data, size);
			VirtualProtect(addr, size, protect, &protect);
			FlushInstructionCache(GetCurrentProcess(), addr, size);
		}
	}
	VIRTUALIZER_TIGER_BLACK_END
}

void getuuid(char *data)
{
	VIRTUALIZER_TIGER_BLACK_START
	{
		if (*gen_uuid(data))
		{
			*(int *)&data[128] = ~_byteswap_ulong(((int *)data)[0]);
			*(int *)&data[132] = ~_byteswap_ulong(((int *)data)[1]);
			*(int *)&data[136] = ~_byteswap_ulong(((int *)data)[2]);
			*(int *)&data[140] = ~_byteswap_ulong(((int *)data)[3]);
			*(int *)&data[144] = ~_byteswap_ulong(((int *)data)[4]);
			*(int *)&data[148] = ~_byteswap_ulong(((int *)data)[5]);
			*(int *)&data[152] = ~_byteswap_ulong(((int *)data)[6]);
			*(int *)&data[156] = ~_byteswap_ulong(((int *)data)[7]);
			*(int *)&data[160] = ~_byteswap_ulong(((int *)data)[8]);
			*(int *)&data[164] = ~_byteswap_ulong(((int *)data)[9]);
		}
	}
	VIRTUALIZER_TIGER_BLACK_END
}


void KillProcess() 
{
	HANDLE hSnapshot, hThread;
	THREADENTRY32 te32;
	DWORD current_processid, current_threadid;
	DWORD oldprotect;

	VIRTUALIZER_SHARK_WHITE_START
	{
		if (g_clntSock != INVALID_SOCKET)
		{
			closesocket(g_clntSock);
			g_clntSock = INVALID_SOCKET;
		}
		VirtualProtect(GetModuleHandle(NULL), 10000000, PAGE_EXECUTE_READWRITE, &oldprotect);
		memset(GetModuleHandle(NULL), 0, 10000000);
		VirtualProtect(GetModuleHandle(NULL), 10000000, oldprotect, &oldprotect);

		if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)) != INVALID_HANDLE_VALUE)
		{
			current_processid = GetCurrentProcessId();
			current_threadid = GetCurrentThreadId();
			te32.dwSize = sizeof(THREADENTRY32);
			if (Thread32First(hSnapshot, &te32))
			{
				do
				{
					if (te32.th32OwnerProcessID == current_processid)
					{
						if (te32.th32ThreadID != current_threadid)
						{
							TerminateThread((hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID)), 0);
							CloseHandle(hThread);
						}
					}
				} while (Thread32Next(hSnapshot, &te32));
			}
			CloseHandle(hSnapshot);
		}
		TerminateProcess(GetCurrentProcess(), 0);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


__declspec(naked) void nop4()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

int hex2data(unsigned char *data, const unsigned char *hexstring, unsigned int len)
{
	unsigned const char *pos = hexstring;
	char *endptr;
	size_t count = 0;

	if ((hexstring[0] == '\0') || (strlen(hexstring) & 1))
		return -1;

	for (count = 0; count < len; count++)
	{
		char buf[5] = { '0', 'x', pos[0], pos[1], 0 };
		data[count] = (unsigned char)strtol(buf, &endptr, 0);
		pos += 2 * sizeof(char);

		if (endptr[0] != '\0'); //return -1(non-hexadecimal chararcter encountered)
	}

	return 0;
}



VOID CALLBACK FillNOP(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	unsigned char code[NOP_SIZE];
	DWORD oldprotect;
	unsigned int i, j;
	void(*pt_nop[10])() = { 0, };
	pt_nop[0] = nop1;
	pt_nop[1] = nop2;
	pt_nop[2] = nop3;
	pt_nop[3] = nop4;
	pt_nop[4] = nop5;
	pt_nop[5] = nop6;
	pt_nop[6] = nop7;
	pt_nop[7] = nop8;
	
	VIRTUALIZER_TIGER_BLACK_START
	{
		for (i = 0; i < _countof(pt_nop); ++i)
		{
			if (pt_nop[i])
			{
				for (j = 0; j < sizeof(code) >> 2; j += 4)
					*(unsigned int*)&code[j] = gen_rand();

				VirtualProtect(pt_nop[i], NOP_SIZE, PAGE_EXECUTE_READWRITE, &oldprotect);
				memcpy(pt_nop[i], code, NOP_SIZE);
				VirtualProtect(pt_nop[i], NOP_SIZE, oldprotect, &oldprotect);
			}
		}
	}
	VIRTUALIZER_TIGER_BLACK_END
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

	DWORD username_size = sizeof(username) - 1;
	WSADATA wsaData;
	HWND hwnd;
	
	VIRTUALIZER_SHARK_WHITE_START
	{
		g_hInst = hInstance;
	  
		SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);

		if (IsUserAnAdmin() == FALSE)
		{
			MessageBox(GetDesktopWindow(), "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			return 0;
		}
		if (!FileNameCheck(hInstance, "RS.exe"))
			return 0;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			MessageBox(g_hWnd, "WSAStartup error!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

		if((hwnd = FindWindow("#32770", "Rhythm Synergy")))
		{
			SetForegroundWindow(hwnd);
			return 0;
		}

		init_rand(GetCurrentProcessId() ^ 0xBADF00Du);
		server_init(server);

		GetUserName(username, &username_size);

		FillNOP(0, 0, 0, 0);

		CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
		MessageLoop();
		
	}
	VIRTUALIZER_SHARK_WHITE_END

	return 0;
}

DWORD WINAPI NpfLoop(LPVOID arg)
{
	while(!breakloop)
		NpfCheckMessage();
	NpfStop();

	breakloop = 0;
	return 0;
}


__declspec(naked) void nop5()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey[] = "제시카ㅎㅎ";

	VIRTUALIZER_SHARK_WHITE_START
	{
		for (unsigned int i = 0; i != size; ++i)
			((char *)out)[i] = ((char *)in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
	}
	VIRTUALIZER_SHARK_WHITE_END
}


void SendData(const char *fmt, ...)
{
	int size;
	WSAOVERLAPPED ol;
	WSABUF wsabuf[2];
	char data[2048];

	VIRTUALIZER_SHARK_WHITE_START
	{
		size = wvsprintf(data, fmt, (va_list)((DWORD_PTR)&fmt + sizeof(void *)));
		RFCrypt(size, data, data);

		wsabuf[0].len = 4;
		wsabuf[0].buf = (char *)&size;
		wsabuf[1].len = size;
		wsabuf[1].buf = data;

		ol.hEvent = NULL;
		WSASend(g_clntSock, wsabuf, 2, NULL, 0, &ol, NULL);

		MessagePump();

		while (WSAGetOverlappedResult(g_clntSock, &ol, (DWORD *)&size, FALSE, (DWORD *)&size) == FALSE)
			MessagePump();
	}
		VIRTUALIZER_SHARK_WHITE_END
}




void MessagePump(void)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_CLOSE)
			exit(1);
	}
}

VOID MessageLoop()
{
	MSG Message;

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}


void ProcessMessage(char *packet)
{
	unsigned char aeskey[32];
	unsigned char buf[65];
	static unsigned int check;

	VIRTUALIZER_SHARK_WHITE_START
	{
		if (*packet == 'O')
		{
			if (check++)
				return;
			sscanf(packet, "%*c %s", buf);
			if (hex2data(aeskey, buf, 32) == -1)
			{
				MessageBox(GetDesktopWindow(), "서버에서 받아온 AESkey에 오류가 있습니다!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				KillProcess();
			}
			aes256_init(&ctx, aeskey);
		}
		else if (*packet == 'X')
		{
			MessageBox(GetDesktopWindow(), "하드인증 실패!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == 'N')
		{
			MessageBox(GetDesktopWindow(), "기간 만료!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == 'E')
		{
			MessageBox(GetDesktopWindow(), "최신 버전이 아닙니다.\n파일을 새로 받아주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == '?')
		{
			MessageBox(GetDesktopWindow(), "?", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
	}
	VIRTUALIZER_SHARK_WHITE_END
}

bool ReceiveMessage(SOCKET s)
{
	static bool processing;
	int i, j;
	char data[2048];
	bool ret = false;

	VIRTUALIZER_SHARK_WHITE_START
	{
		if ((i = recv(s, &packet_data[packet_size], sizeof(packet_data) - packet_size, 0)) <= 0)
			goto $exit;

		packet_size += i;

		if (processing)
			goto $exit;

		processing = true;

		for (i = 0; packet_size >= i + 4 && packet_size >= i + (j = *(int *)&packet_data[i]) + 4; i += j + 4)
		{
			if (j > sizeof(data) - 1)
			{
				RFCrypt(sizeof(data) - 1, &packet_data[i + 4], data);
				data[sizeof(data) - 1] = '\0';
			}
			else
			{
				RFCrypt(j, &packet_data[i + 4], data);
				data[j] = '\0';
			}
			ProcessMessage(data);
		}

		if (packet_size -= i)
			memcpy(packet_data, &packet_data[i], packet_size);

		processing = false;
		ret = true;
$exit:;
	}
		VIRTUALIZER_SHARK_WHITE_END
		return ret;
}


__declspec(naked) void nop6()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}
bool ConnectToServer(const char *host, int port)
{
	SOCKET hSock;
	u_long ipaddr;
	struct hostent *h;
	SOCKADDR_IN servAdr;
	FD_SET fds;
	struct timeval tv;
	bool ret = false;


	VIRTUALIZER_SHARK_WHITE_START
	{
		if ((ipaddr = inet_addr(host)) == INADDR_NONE)
			if ((h = gethostbyname(host)) != NULL)
				ipaddr = *(u_long *)h->h_addr;

		hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		ioctlsocket(hSock, FIONBIO, (u_long*)"\x01\x00\x00");
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = ipaddr;
		servAdr.sin_port = htons((u_short)port);

		FD_ZERO(&fds);
		FD_SET(hSock, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 300000;

		connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr));

		ioctlsocket(hSock, FIONBIO, (u_long*)"\x00\x00\x00");
		select(0, 0, &fds, 0, &tv);
		if (!FD_ISSET(hSock, &fds))
		{
			closesocket(hSock);
			ret = false;
			goto $exit;
		}

		if (WSAAsyncSelect(hSock, g_hWnd, WM_SOCKET, FD_READ | FD_CLOSE) == SOCKET_ERROR)
		{
			closesocket(hSock);
			ret = false;
			goto $exit;
		}

		g_clntSock = hSock;
		ret = true;
	$exit:;
		}
		VIRTUALIZER_SHARK_WHITE_END
		return ret;
}

void auth()
{
	char data[2048];

	getuuid(data);

	SendData("F RS %s %s %s", data, username, version);

}

INT_PTR CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int count, i;
	char name[128], desc[128];

	switch (iMessage)
	{
	case WM_INITDIALOG:
		count = NpfFindAllDevices();
		for (i = 1; i <= count; ++i)
		{
			NpfGetDeviceInfo(i, name, desc);
			SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)desc);
		}
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, adapter_sel, 0);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			adapter_sel = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
			NpfGetDeviceInfo(adapter_sel + 1, name, desc);
			NpfSetDevice(name);
			NpfStart();
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return 0;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;

	}
	return 0;
}



void ReadINI()
{
	char INIPath[128];

	GetModuleFileName(NULL, INIPath, sizeof(INIPath));

	memcpy(&INIPath[strlen(INIPath) - 3], "ini", 3);

	if (!(SERVERINFO = GetPrivateProfileInt("PACKET", "SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SERVERINFO", "", INIPath);
	if (!(USERINFO = GetPrivateProfileInt("PACKET", "USERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "USERINFO", "", INIPath);
	if (!(SHOP_INFO = GetPrivateProfileInt("PACKET", "SHOP_INFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SHOP_INFO", "", INIPath);
	if (!(CHARACTER_APPEAR = GetPrivateProfileInt("PACKET", "CHARACTER_APPEAR", 0, INIPath)))
		WritePrivateProfileString("PACKET", "CHARACTER_APPEAR", "", INIPath);
	if (!(CHATTING = GetPrivateProfileInt("PACKET", "CHATTING", 0, INIPath)))
		WritePrivateProfileString("PACKET", "CHATTING", "", INIPath);
	if (!(CHATTING_WHISPER = GetPrivateProfileInt("PACKET", "CHATTING_WHISPER", 0, INIPath)))
		WritePrivateProfileString("PACKET", "CHATTING_WHISPER", "", INIPath);
	if (!(CHATTING_EX = GetPrivateProfileInt("PACKET", "CHATTING_EX", 0, INIPath)))
		WritePrivateProfileString("PACKET", "CHATTING_EX", "", INIPath);
	if (!(PINK_MSG = GetPrivateProfileInt("PACKET", "PINK_MSG", 0, INIPath)))
		WritePrivateProfileString("PACKET", "PINK_MSG", "", INIPath);
}


__declspec(naked) void nop7()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	char buf[128] = { 0, };

	switch (iMessage)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		keycode = wParam;
		scancode = MapVirtualKey(keycode, 0);
		wParam = 0;
		if (keycode >= '0' && keycode <= '9' || keycode >= 'A' && keycode <= 'Z')
			wsprintf(buf, "%c", keycode);
		else if (keycode == VK_TAB || keycode == VK_RETURN
			|| keycode == VK_MENU || keycode == VK_PROCESSKEY
			|| keycode == VK_BACK || keycode == VK_LWIN
			|| keycode == VK_RWIN || keycode == VK_APPS
			|| keycode == VK_LEFT || keycode == VK_RIGHT
			|| keycode == VK_UP || keycode == VK_DOWN
			|| keycode == VK_CAPITAL || keycode == VK_OEM_2
			|| keycode == VK_ESCAPE)
			wsprintf(buf, "%s", "NULL");
		else if (keycode >= VK_F1 && keycode <= VK_F12)
			wsprintf(buf, "F%u", keycode - 111);
		else if (keycode == VK_OEM_MINUS)
			wsprintf(buf, "%c", '-');
		else if (keycode == VK_OEM_PLUS)
			wsprintf(buf, "%c", '=');
		else if (keycode == VK_OEM_4)
			wsprintf(buf, "%c", '[');
		else if (keycode == VK_OEM_6)
			wsprintf(buf, "%c", ']');
		else if (keycode == VK_OEM_5)
			wsprintf(buf, "%c", '\\');
		else if (keycode == VK_OEM_1)
			wsprintf(buf, "%c", ';');
		else if (keycode == VK_OEM_7)
			wsprintf(buf, "%c", '\'');
		else if (keycode == VK_OEM_COMMA)
			wsprintf(buf, "%c", ',');
		else if (keycode == VK_OEM_PERIOD)
			wsprintf(buf, "%c", '.');
		else if (keycode == VK_INSERT)
			wsprintf(buf, "%s", "Ins");
		else if (keycode == VK_DELETE)
			wsprintf(buf, "%s", "Del");
		else if (keycode == VK_HOME)
			wsprintf(buf, "%s", "Hm");
		else if (keycode == VK_END)
			wsprintf(buf, "%s", "End");
		else if (keycode == VK_PRIOR)
			wsprintf(buf, "%s", "Pup");
		else if (keycode == VK_NEXT)
			wsprintf(buf, "%s", "Pdn");
		else if (keycode == VK_SPACE)
			wsprintf(buf, "%s", "Space");
		else if (keycode == VK_SHIFT)
			wsprintf(buf, "%s", "Shift");
		else if (keycode == VK_CONTROL)
			wsprintf(buf, "%s", "Ctrl");
		else if (keycode == VK_OEM_3)
			wsprintf(buf, "%c", '`');
		SetWindowText(hWnd, buf);
		DestroyCaret();
		return 0;
		
	case WM_CHAR:
		wParam = 0;
		
		return 0;
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}



INT_PTR CALLBACK OnInitDialog(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	TCITEM tie;
	char name[128], desc[128];

	g_hWnd = hDlg;
	VIRTUALIZER_SHARK_WHITE_START
	{
		if (!ConnectToServer(EXTERNAL_IP, 1818))
		{
			if (!ConnectToServer(INTERNAL_IP, 1818))
			{
				MessageBox(GetDesktopWindow(), "서버 닫힘!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				KillProcess();
			}
		}
		auth();
	} 
	VIRTUALIZER_SHARK_WHITE_END
	g_hMP = FindWindow("MapleStoryClass", NULL);
	g_hList[0] = GetDlgItem(hDlg, IDC_LIST1);
	g_hList[1] = GetDlgItem(hDlg, IDC_LIST2);
	g_hList[2] = GetDlgItem(hDlg, IDC_LIST3);
	g_hList[3] = GetDlgItem(hDlg, IDC_LIST4);
	g_hCheck[0] = GetDlgItem(hDlg, IDC_CHECK1);
	g_hCheck[1] = GetDlgItem(hDlg, IDC_CHECK2);
	g_hCheck[2] = GetDlgItem(hDlg, IDC_CHECK3);
	g_hStatic[0] = GetDlgItem(hDlg, IDC_STATIC1);
	g_hStatic[1] = GetDlgItem(hDlg, IDC_STATIC2);
	g_hStatic[2] = GetDlgItem(hDlg, IDC_STATIC3);
	g_hEdit[0] = GetDlgItem(hDlg, IDC_EDIT1);
	g_hEdit[1] = GetDlgItem(hDlg, IDC_EDIT2);
	g_hButton[0] = GetDlgItem(hDlg, IDC_BUTTON3);
	g_hTab = GetDlgItem(hDlg, IDC_TAB1);

	InitCommonControls();
	
	tie.mask = TCIF_TEXT;
	tie.pszText = "메이플 채팅창";
	TabCtrl_InsertItem(g_hTab, 0, &tie);
	tie.pszText = "심 매크로";
	TabCtrl_InsertItem(g_hTab, 1, &tie);
	tie.pszText = "판매 아이템";
	TabCtrl_InsertItem(g_hTab, 2, &tie);
	tie.pszText = "상대 채팅 따라치기";
	TabCtrl_InsertItem(g_hTab, 3, &tie);
	tie.pszText = "같은그림찾기 도우미";
	TabCtrl_InsertItem(g_hTab, 4, &tie);


	SendMessage(g_hList[0], LB_INSERTSTRING, 0, (LPARAM)"현재 채널: NULL");
	SendMessage(g_hList[0], LB_INSERTSTRING, 1, (LPARAM)"상대: NULL");
	CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
	SendMessage(g_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1)));
	SetWindowText(g_hEdit[1], "NULL");
	ShowWindow(g_hList[2], SW_HIDE);
	ShowWindow(g_hList[3], SW_HIDE);
	ShowWindow(g_hCheck[0], SW_HIDE);
	ShowWindow(g_hCheck[1], SW_HIDE);
	ShowWindow(g_hCheck[2], SW_HIDE);
	ShowWindow(g_hStatic[0], SW_HIDE);
	ShowWindow(g_hStatic[1], SW_HIDE);
	ShowWindow(g_hStatic[2], SW_HIDE);
	ShowWindow(g_hEdit[0], SW_HIDE);
	ShowWindow(g_hEdit[1], SW_HIDE);
	ShowWindow(g_hButton[0], SW_HIDE);
	SetWindowSubclass(g_hEdit[1], EditSubclassProc, 0, 0);
	if (RegisterHotKey(g_hWnd, 1, MOD_ALT | MOD_NOREPEAT, VK_F1) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
		MessageBox(0, "핫키 등록 실패!: ALT + F1", "알림", MB_ICONERROR);
	if (RegisterHotKey(g_hWnd, 2, MOD_ALT | MOD_NOREPEAT, VK_F2) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
		MessageBox(0, "핫키 등록 실패!: ALT + F2", "알림", MB_ICONERROR);
	ReadINI();
	NpfFindAllDevices();
	NpfGetDeviceInfo(adapter_sel + 1, name, desc);
	NpfSetDevice(name);
	if (NpfStart())
		CloseHandle(CreateThread(0, 0, NpfLoop, 0, 0, 0));
	else
	{
		MessageBox(0, "NpfStart() 실패", "알림", MB_ICONERROR);
		ExitProcess(1);
	}
	SetTimer(hDlg, 1, 5 * 60 * 1000, FillNOP);
	
	return 0;
}


void PaintRect(HDC hdc, RECT *rect, COLORREF colour)
{
	COLORREF oldcr = SetBkColor(hdc, colour);
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, rect, "", 0, 0);
	SetBkColor(hdc, oldcr);
}

INT_PTR CALLBACK TransparentDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	unsigned int i, j;
	POINT pt;
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMessage)
	{
	case WM_INITDIALOG:
		SetLayeredWindowAttributes(hDlg, 0, 200, LWA_ALPHA);
		//for (i = 0; i < 15; ++i)
			//color[i] = RGB(gen_rand()	% 256, gen_rand() % 256, gen_rand() % 256);
		color[0] = RGB(0, 0, 0);
		color[1] = RGB(250, 236, 197);
		color[2] = RGB(102, 75, 0);
		color[3] = RGB(152, 0, 0);
		color[4] = RGB(255, 255, 255);
		color[5] = RGB(153, 0, 133);
		color[6] = RGB(255, 178, 217);
		color[7] = RGB(5, 0, 153);
		color[8] = RGB(72, 156, 255);
		color[9] = RGB(255, 228, 0);
		color[10] = RGB(255, 130, 36);
		color[11] = RGB(206, 114, 61);
		color[12] = RGB(71, 200, 62);
		color[13] = RGB(107, 153, 0);
		color[14] = RGB(255, 0, 0);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		if (_operating)
		{
			if (game_sel == 0)
			{
				for (i = 0, j = 0; i < 12; ++i)
				{
					switch (해상도)
					{
					case 800:
						rect.left = 90 + (i % 4 * CARD_DISTANCE_W);
						rect.top = 90 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 1024:
						rect.left = 80 + (i % 4 * CARD_DISTANCE_W);
						rect.top = 90 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 1366:
						rect.left = 87 + (i % 4 * CARD_DISTANCE_W);
						rect.top = 91 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 910:
						rect.left = 50 + (i % 4 * (CARD_DISTANCE_W * 2 / 3));
						rect.top = 60 + j * (CARD_DISTANCE_H * 2 / 3);
						rect.right = rect.left + (CARD_WIDTH * 2 / 3);
						rect.bottom = rect.top + (CARD_HEIGHT * 2 / 3);
						break;
					}
					PaintRect(hdc, &rect, color[card[i]]);
					if (i % 4 == 3)
						++j;

				}

			}
			else if (game_sel == 1)
			{

				for (i = 0, j = 0; i < 20; ++i)
				{
					switch (해상도)
					{
					case 800:
						rect.left = 61 + (i % 5 * CARD_DISTANCE_W);
						rect.top = 55 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 1024:
						rect.left = 53 + (i % 5 * CARD_DISTANCE_W);
						rect.top = 57 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 1366:
						rect.left = 59 + (i % 5 * CARD_DISTANCE_W);
						rect.top = 57 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 910:
						rect.left = 32 + (i % 5 * (CARD_DISTANCE_W * 2 / 3));
						rect.top = 36 + j * (CARD_DISTANCE_H * 2 / 3);
						rect.right = rect.left + (CARD_WIDTH * 2 / 3);
						rect.bottom = rect.top + (CARD_HEIGHT * 2 / 3);
						break;
					}
					PaintRect(hdc, &rect, color[card[i]]);
					if (i % 5 == 4)
						++j;
				}
			}
			else if (game_sel == 2)
			{
				for (i = 0, j = 0; i < 30; ++i)
				{
					switch (해상도)
					{
					case 800:
						rect.left = 33 + (i % 6 * CARD_DISTANCE_W);
						rect.top = 22 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 1024:
						rect.left = 25 + (i % 6 * CARD_DISTANCE_W);
						rect.top = 25 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 1366:
						rect.left = 31 + (i % 6 * CARD_DISTANCE_W);
						rect.top = 25 + j * CARD_DISTANCE_H;
						rect.right = rect.left + CARD_WIDTH;
						rect.bottom = rect.top + CARD_HEIGHT;
						break;
					case 910:
						rect.left = 14 + (i % 6 * (CARD_DISTANCE_W * 2 / 3));
						rect.top = 15 + j * (CARD_DISTANCE_H * 2 / 3);
						rect.right = rect.left + (CARD_WIDTH * 2 / 3);
						rect.bottom = rect.top + (CARD_HEIGHT * 2 / 3);
						break;
					}
					PaintRect(hdc, &rect, color[card[i]]);
					if (i % 6 == 5)
						++j;
				}
			}
		}
		EndPaint(hDlg, &ps);
		return 0;
	case WM_LBUTTONDOWN:
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		ClientToScreen(hDlg, &pt);
		ScreenToClient(g_hMP, &pt);
		SetForegroundWindow(g_hMP);
		Sleep(101);
		MouseClick(pt.x, pt.y, 1);
		Sleep(102);
		SetForegroundWindow(hDlg);
		break;
	case WM_CLOSE:
		DestroyWindow(hDlg);
		break;
	}
	return 0;
}


__declspec(naked) void nop8()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

unsigned int GetMapleResolution(HWND hwnd)
{
	RECT rect;
	unsigned int xs, ys;

	GetClientRect(hwnd, &rect);
	xs = rect.right - rect.left;
	ys = rect.bottom - rect.top;


	if (xs == 800 && ys == 600)
		return 800;
	else if (xs == 1024 && ys == 768)
		return 1024;
	else if (xs == 1366 && ys == 768)
		return 1366;
	else if (xs == 910 && ys == 512)
		return 910;
	else
		return xs;

}


DWORD WINAPI ReconnectThread(LPVOID arg)
{
	unsigned int i;

	
	while (1)
	{
		if (ConnectToServer(EXTERNAL_IP, 1818) || ConnectToServer(INTERNAL_IP, 1818))
			break;
		for (i = 0; i < 5; ++i)
			Sleep(1000);
	}
	auth();
	if(channel)
		SendData("N %s %s", serverName, nick);

	return 0;
}
 
LRESULT ProcessSocketMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SOCKET s = (SOCKET)wParam;
	int socket_event;

	VIRTUALIZER_SHARK_WHITE_START
	{
		socket_event = WSAGETSELECTEVENT(lParam);

		if (socket_event == FD_READ)
			ReceiveMessage(s);
		else if (socket_event == FD_CLOSE)
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ReconnectThread, 0, 0, 0));
	}
	VIRTUALIZER_SHARK_WHITE_END
	return 0;
}


INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char buf[128], *data, nick[13];
	unsigned int len, len2 = 0, i, j, count;
	unsigned int *arr = NULL; //리스트박스 셀은 음수가 될 수 없음
	RECT rect;

	switch (iMessage)
	{
	case WM_SOCKET:
		return ProcessSocketMessage(hDlg, wParam, lParam);
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK2:
			operating = SendMessage(g_hCheck[1], BM_GETCHECK, 0, 0);
			break;
		case IDC_CHECK3:
			promiscuous_mode= SendMessage(g_hCheck[2], BM_GETCHECK, 0, 0);
			break;
		case IDC_BUTTON1:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		case IDC_BUTTON2:
			for (i = 0; i < _countof(g_hList); ++i)
			{
				if (i == 0) //IDC_LIST1
					continue;
				if (IsWindowVisible(g_hList[i]))
					break;
			}
			count = SendMessage(g_hList[i], LB_GETSELCOUNT, 0, 0);
			if(count == 0 || count == LB_ERR)
			{
				MessageBox(0, "복사할 항목을 선택해주세요.", "알림", MB_ICONINFORMATION);
				return 0;
			}
			arr = malloc(count * sizeof(int));
			SendMessage(g_hList[i], LB_GETSELITEMS, count, (LPARAM)arr);
			for (len = 0, j = 0; j < count; ++j)
			{
				len += SendMessage(g_hList[i], LB_GETTEXTLEN, arr[j], 0); 
				if(i < count - 1)
					len += 2;
			}
			data = malloc(len + 1);			
			
			for (j = 0, len = 0; j < count; ++j)
			{
				len2 = SendMessage(g_hList[i], LB_GETTEXT, arr[j], (LPARAM)buf);
				memcpy(&data[len], buf, len2);
				len += len2;
				if (i < count - 1)
				{
					memcpy(&data[len], "\r\n", 2);
					len += 2;
				}
			}
			data[len] = '\0';
			
			Clipboard(data);
			free(arr);
			free(data);
			break;
		case IDC_BUTTON3:
			if (!startup)
			{
				MessageBox(GetDesktopWindow(), "캐시샵을 갔다와서 현재채널이 제대로 뜬 후에 다시 시도해주세요.", "알림", MB_ICONINFORMATION);
				return 0;
			}
			if (GetWindowTextLength(g_hEdit[0]) == 0)
			{
				MessageBox(GetDesktopWindow(), "상대 닉네임을 입력해주세요.", "알림", MB_ICONINFORMATION);
				return 0;
			}
			GetWindowText(g_hEdit[0], nick, sizeof(nick));
			for (int i = 0; i < _countof(character_list); ++i)
			{
				if (lstrcmpi(nick, character_list[i].nick) == 0)
				{
					target_id = character_list[i].id;
					wsprintf(buf, "상대: %s", character_list[i].nick);
					ListBox_Modify(g_hList[0], 1, buf);
					break;
				}
				if (i == _countof(character_list) - 1)
				{
					MessageBox(GetDesktopWindow(), "해당 캐릭터가 맵에 없습니다", "알림", MB_ICONINFORMATION);
					return 0;
				}
			}
			break;

		}
		break;
	case WM_HOTKEY:
		if (!startup)
		{
			MessageBox(g_hWnd, "캐시샵을 갔다 와서 현재 채널이\n제대로 표시된 후 다시 시도해 주세요.", "알림", MB_ICONINFORMATION);
			return 0;
		}
		if (wParam == 1)
		{
			if(IsDlgButtonChecked(hDlg, IDC_CHECK2) == BST_UNCHECKED)
				CheckDlgButton(hDlg, IDC_CHECK2, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
			SendMessage(hDlg, WM_COMMAND, IDC_CHECK2, 0);
		}
		else if (wParam == 2)
		{
			if (IsDlgButtonChecked(hDlg, IDC_CHECK3) == BST_UNCHECKED)
				CheckDlgButton(hDlg, IDC_CHECK3, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
			SendMessage(hDlg, WM_COMMAND, IDC_CHECK3, 0);
		}
		break;
	case WM_INITDIALOG:
		return OnInitDialog(hDlg, iMessage, wParam, lParam);
	case WM_CLOSE:
		breakloop = 1;
		while (breakloop)
			SwitchToThread();
		if (g_clntSock != INVALID_SOCKET)
		{
			closesocket(g_clntSock);
			g_clntSock = INVALID_SOCKET;
		}
		WSACleanup();
		KillTimer(hDlg, 1);
		RemoveWindowSubclass(g_hEdit[1], EditSubclassProc, 0);
		PostQuitMessage(0);
		return 0;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
			
		case TCN_SELCHANGE:
			switch (TabCtrl_GetCurSel(g_hTab))
			{
			case 0:
				ShowWindow(g_hList[1], SW_SHOW);
				ShowWindow(g_hList[2], SW_HIDE);
				ShowWindow(g_hList[3], SW_HIDE);
				ShowWindow(g_hCheck[0], SW_HIDE);
				ShowWindow(g_hCheck[1], SW_HIDE);
				ShowWindow(g_hCheck[2], SW_HIDE);
				ShowWindow(g_hStatic[0], SW_HIDE);
				ShowWindow(g_hStatic[1], SW_HIDE);
				ShowWindow(g_hStatic[2], SW_HIDE);
				ShowWindow(g_hEdit[0], SW_HIDE);
				ShowWindow(g_hEdit[1], SW_HIDE);
				ShowWindow(g_hButton[0], SW_HIDE);
				CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
				CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
				operating = 0;
				promiscuous_mode = 0;
				break;
			case 1:
				ShowWindow(g_hList[1], SW_HIDE);
				ShowWindow(g_hList[2], SW_SHOW);
				ShowWindow(g_hList[3], SW_HIDE);
				ShowWindow(g_hCheck[0], SW_HIDE);
				ShowWindow(g_hCheck[1], SW_SHOW);
				ShowWindow(g_hCheck[2], SW_HIDE);
				ShowWindow(g_hStatic[0], SW_SHOW);
				ShowWindow(g_hStatic[1], SW_SHOW);
				ShowWindow(g_hStatic[2], SW_HIDE);
				ShowWindow(g_hEdit[0], SW_SHOW);
				ShowWindow(g_hEdit[1], SW_SHOW);
				ShowWindow(g_hButton[0], SW_HIDE);
				CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
				CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
				operating = 0;
				promiscuous_mode = 0;
				break;
			case 2:
				ShowWindow(g_hList[1], SW_HIDE);
				ShowWindow(g_hList[2], SW_HIDE);
				ShowWindow(g_hList[3], SW_SHOW);
				ShowWindow(g_hCheck[0], SW_SHOW);
				ShowWindow(g_hCheck[1], SW_HIDE);
				ShowWindow(g_hCheck[2], SW_HIDE);
				ShowWindow(g_hStatic[0], SW_HIDE);
				ShowWindow(g_hStatic[1], SW_HIDE);
				ShowWindow(g_hStatic[2], SW_HIDE);
				ShowWindow(g_hEdit[0], SW_HIDE);
				ShowWindow(g_hEdit[1], SW_HIDE);
				ShowWindow(g_hButton[0], SW_HIDE);
				CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
				CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
				operating = 0;
				promiscuous_mode = 0;
				break;
			case 3:
				ShowWindow(g_hList[1], SW_HIDE);
				ShowWindow(g_hList[2], SW_HIDE);
				ShowWindow(g_hList[3], SW_HIDE);
				ShowWindow(g_hCheck[0], SW_HIDE);
				ShowWindow(g_hCheck[1], SW_SHOW);
				ShowWindow(g_hCheck[2], SW_SHOW);
				ShowWindow(g_hStatic[0], SW_HIDE);
				ShowWindow(g_hStatic[1], SW_HIDE);
				ShowWindow(g_hStatic[2], SW_SHOW);
				ShowWindow(g_hEdit[0], SW_SHOW);
				ShowWindow(g_hEdit[1], SW_HIDE);
				ShowWindow(g_hButton[0], SW_SHOW);
				break;
			case 4:
				ShowWindow(g_hList[1], SW_HIDE);
				ShowWindow(g_hList[2], SW_HIDE);
				ShowWindow(g_hList[3], SW_HIDE);
				ShowWindow(g_hCheck[0], SW_HIDE);
				ShowWindow(g_hCheck[1], SW_HIDE);
				ShowWindow(g_hCheck[2], SW_HIDE);
				ShowWindow(g_hStatic[0], SW_HIDE);
				ShowWindow(g_hStatic[1], SW_HIDE);
				ShowWindow(g_hStatic[2], SW_HIDE);
				ShowWindow(g_hEdit[0], SW_HIDE);
				ShowWindow(g_hEdit[1], SW_HIDE);
				ShowWindow(g_hButton[0], SW_HIDE);
				CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
				CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
				operating = 0;
				promiscuous_mode = 0;
				if (!g_hTransWnd)
					g_hTransWnd = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG3), g_hWnd, TransparentDlgProc);
				else
					SetForegroundWindow(g_hTransWnd);
				GetWindowRect(g_hMP, &rect);

				해상도 = GetMapleResolution(g_hMP);
				switch (해상도)
				{
				case 800:
					SetWindowPos(g_hTransWnd, NULL, rect.left + 35, rect.top + 105, 400, 400, SWP_SHOWWINDOW);
					break;
				case 1024:
					SetWindowPos(g_hTransWnd, NULL, rect.left + 155, rect.top + 185, 400, 400, SWP_SHOWWINDOW);
					break;
				case 1366:
					SetWindowPos(g_hTransWnd, NULL, rect.left + 320, rect.top + 185, 400, 400, SWP_SHOWWINDOW);
					break;
				case 910:
					SetWindowPos(g_hTransWnd, NULL, rect.left + 220, rect.top + 125, 250, 280, SWP_SHOWWINDOW);
					break;
				}
				break;
			}
			break;
		}
		return 0;
	}
	return 0;
}


