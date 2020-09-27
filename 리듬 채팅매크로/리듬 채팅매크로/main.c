#include "common.h"

const char version[] = "1.23";

#define INTERNAL_IP "192.168.219.100"
#define EXTERNAL_IP "182.211.75.197"

unsigned int channel;
char nick[13], serverName[16];

server_t server[11];
SOCKET g_clntSock = INVALID_SOCKET;
char packet_data[4096];
int packet_size;

extern aes256_context ctx;
HWND g_hWnd, g_hSettingWnd, g_hMP, g_hEdit[12];

unsigned int adapter_sel, breakloop, operating, check[13], delay, 해상도;
char INIPath[MAX_PATH], username[128];

unsigned int SERVERINFO, SHOP_INFO;

void ListBox_Modify(HWND hwnd, int sel, char *str)
{
	SendMessage(hwnd, LB_DELETESTRING, sel, 0);
	SendMessage(hwnd, LB_INSERTSTRING, sel, (LPARAM)str);
}

void dbg(const char *fmt, ...)
{
	char msg[1024];
	OutputDebugStringA((wvsprintf(msg, fmt, (va_list) ((unsigned int) &fmt + sizeof(void *))), msg));
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

unsigned int atoi32(const char *string)
{
	unsigned int value = 0;

	while (*string >= '0' && *string <= '9')
		value = value * 10u + (*string++ - '0');

	return value;
}


__declspec(naked) void nop1()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
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
		return 0;

}


void ReadINI()
{
	char buf[256];

	GetModuleFileName(NULL, INIPath, sizeof(INIPath));

	memcpy(&INIPath[strlen(INIPath) - 3], "ini", 3);

	if (!(SERVERINFO = GetPrivateProfileInt("PACKET", "SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SERVERINFO", "", INIPath);
	if (!(SHOP_INFO = GetPrivateProfileInt("PACKET", "SHOP_INFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SHOP_INFO", "", INIPath);

	if(GetPrivateProfileString("SETTING", "EDIT1", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT1", "", INIPath);
	SetWindowText(g_hEdit[0], buf);
	if (GetPrivateProfileString("SETTING", "EDIT2", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT2", "", INIPath);
	SetWindowText(g_hEdit[1], buf);
	if (GetPrivateProfileString("SETTING", "EDIT3", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT3", "", INIPath);
	SetWindowText(g_hEdit[2], buf);
	if (GetPrivateProfileString("SETTING", "EDIT4", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT4", "", INIPath);
	SetWindowText(g_hEdit[3], buf);
	if (GetPrivateProfileString("SETTING", "EDIT5", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT5", "", INIPath);
	SetWindowText(g_hEdit[4], buf);
	if (GetPrivateProfileString("SETTING", "EDIT6", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT6", "", INIPath);
	SetWindowText(g_hEdit[5], buf);
	if (GetPrivateProfileString("SETTING", "EDIT7", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT7", "", INIPath);
	SetWindowText(g_hEdit[6], buf);
	if (GetPrivateProfileString("SETTING", "EDIT8", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT8",  "", INIPath);
	SetWindowText(g_hEdit[7], buf);
	if (GetPrivateProfileString("SETTING", "EDIT9", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT9", "", INIPath);
	SetWindowText(g_hEdit[8], buf);
	if (GetPrivateProfileString("SETTING", "EDIT10", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT10", "", INIPath);
	SetWindowText(g_hEdit[9], buf);
	if (GetPrivateProfileString("SETTING", "EDIT11", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT11", "", INIPath);
	SetWindowText(g_hEdit[10], buf);
	if (GetPrivateProfileString("SETTING", "EDIT12", "", buf, sizeof(buf), INIPath) == 0)
		WritePrivateProfileString("SETTING", "EDIT12", "", INIPath);
	SetWindowText(g_hEdit[11], buf);
	
	if (!(check[0] = GetPrivateProfileInt("SETTING", "CHECK1", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK1", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK1, check[0]);
	if (!(check[1] = GetPrivateProfileInt("SETTING", "CHECK2", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK2", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK2, check[1]);
	if (!(check[2] = GetPrivateProfileInt("SETTING", "CHECK3", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK3", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK3, check[2]);
	if (!(check[3] = GetPrivateProfileInt("SETTING", "CHECK4", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK4", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK4, check[3]);
	if (!(check[4] = GetPrivateProfileInt("SETTING", "CHECK5", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK5", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK5, check[4]);
	if (!(check[5] = GetPrivateProfileInt("SETTING", "CHECK6", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK6", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK6, check[5]);
	if (!(check[6] = GetPrivateProfileInt("SETTING", "CHECK7", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK7", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK7, check[6]);
	if (!(check[7] = GetPrivateProfileInt("SETTING", "CHECK8", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK8", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK8, check[7]);
	if (!(check[8] = GetPrivateProfileInt("SETTING", "CHECK9", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK9", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK9, check[8]);
	if (!(check[9] = GetPrivateProfileInt("SETTING", "CHECK10", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK10", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK10, check[9]);
	if (!(check[10] = GetPrivateProfileInt("SETTING", "CHECK11", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK11", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK11, check[10]);
	if (!(check[11] = GetPrivateProfileInt("SETTING", "CHECK12", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK12", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK12, check[11]);
	if (!(check[12] = GetPrivateProfileInt("SETTING", "CHECK13", 0, INIPath)))
		WritePrivateProfileString("SETTING", "CHECK13", "0", INIPath);
	CheckDlgButton(g_hWnd, IDC_CHECK13, check[12]);

	if (!(adapter_sel = GetPrivateProfileInt("SETTING", "adapter_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "adapter_sel", "0", INIPath);
	if ((delay = GetPrivateProfileInt("SETTING", "delay", 1500, INIPath)) == 1500)
		WritePrivateProfileString("SETTING", "delay", "1500", INIPath);
}

void WriteINI()
{
	char buf[256];

	GetWindowText(g_hEdit[0], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT1", buf, INIPath);
	GetWindowText(g_hEdit[1], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT2", buf, INIPath);
	GetWindowText(g_hEdit[2], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT3", buf, INIPath);
	GetWindowText(g_hEdit[3], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT4", buf, INIPath);
	GetWindowText(g_hEdit[4], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT5", buf, INIPath);
	GetWindowText(g_hEdit[5], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT6", buf, INIPath);
	GetWindowText(g_hEdit[6], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT7", buf, INIPath);
	GetWindowText(g_hEdit[7], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT8", buf, INIPath);
	GetWindowText(g_hEdit[8], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT9", buf, INIPath);
	GetWindowText(g_hEdit[9], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT10", buf, INIPath);
	GetWindowText(g_hEdit[10], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT11", buf, INIPath);
	GetWindowText(g_hEdit[11], buf, sizeof(buf));
	WritePrivateProfileString("SETTING", "EDIT12", buf, INIPath);


	wsprintf(buf, "%u", check[0]);
	WritePrivateProfileString("SETTING", "CHECK1", buf, INIPath);
	wsprintf(buf, "%u", check[1]);
	WritePrivateProfileString("SETTING", "CHECK2", buf, INIPath);
	wsprintf(buf, "%u", check[2]);
	WritePrivateProfileString("SETTING", "CHECK3", buf, INIPath);
	wsprintf(buf, "%u", check[3]);
	WritePrivateProfileString("SETTING", "CHECK4", buf, INIPath);
	wsprintf(buf, "%u", check[4]);
	WritePrivateProfileString("SETTING", "CHECK5", buf, INIPath);
	wsprintf(buf, "%u", check[5]);
	WritePrivateProfileString("SETTING", "CHECK6", buf, INIPath);
	wsprintf(buf, "%u", check[6]);
	WritePrivateProfileString("SETTING", "CHECK7", buf, INIPath);
	wsprintf(buf, "%u", check[7]);
	WritePrivateProfileString("SETTING", "CHECK8", buf, INIPath);
	wsprintf(buf, "%u", check[8]);
	WritePrivateProfileString("SETTING", "CHECK9", buf, INIPath);
	wsprintf(buf, "%u", check[9]);
	WritePrivateProfileString("SETTING", "CHECK10", buf, INIPath);
	wsprintf(buf, "%u", check[10]);
	WritePrivateProfileString("SETTING", "CHECK11", buf, INIPath);
	wsprintf(buf, "%u", check[11]);
	WritePrivateProfileString("SETTING", "CHECK12", buf, INIPath);
	wsprintf(buf, "%u", check[12]);
	WritePrivateProfileString("SETTING", "CHECK13", buf, INIPath);


	wsprintf(buf, "%u", adapter_sel);
	WritePrivateProfileString("SETTING", "adapter_sel", buf, INIPath);
	wsprintf(buf, "%u", delay);
	WritePrivateProfileString("SETTING", "delay", buf, INIPath);

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
	char buf[1024];
	unsigned int i;
	WINDOWINFO wi;

	if (*(unsigned short*) data == SERVERINFO)
	{
		if (data[2] > 19)
			return;
		else if (data[2] == 0)
			channel = 1;
		else if (data[2] == 1)
			channel = 20;
		else
			channel = data[2];
		if (channel == 20)
			wsprintf(buf, "현재 채널: %u세", channel);
		else
			wsprintf(buf, "현재 채널: %u채널", channel);
		ListBox_Modify(GetDlgItem(g_hWnd, IDC_LIST1), 0, buf);

		for (i = 0; i < size - 7; ++i)
		{
			if (isserver(data[i]) && memcmp("\x00\x00\x00", &data[i + 1], 3) == 0 && data[i + 4] != 0 && isnickname(&data[i + 4], 4))
			{
				GetServerName(data[i], serverName);
				strcpy(nick, &data[i + 4]);
				SendData("N %s %s", serverName, nick);
				break;
			}
		}
		
		
	}
	else if (*(unsigned short*) data == SHOP_INFO)
	{
		if (data[2] == 0x15 && operating && check[10])
		{
			wi.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(g_hMP, &wi);
			if (wi.dwStyle & WS_MINIMIZE)
			{
				PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
				Sleep(301);
			}
			SetForegroundWindow(g_hMP);
			Sleep(150);

			switch (해상도)
			{
			case 800:
				MouseClick(583, 501, 2);
				break;
			case 1024:
				MouseClick(583, 667, 2);
				break;
			case 1366:
				MouseClick(583, 671, 2);
				break;
			case 910:
				MouseClick(389, 446, 2);
				Sleep(50);
				MouseClick(388, 443, 2);
				break;
			}
			PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_ASYNC | SND_RESOURCE);
		}
		else if (data[2] == 0x14 && operating && check[10])
		{
			if (data[3] == 0x4)
			{
				operating = 0;
				if (check[11])
				{
					GetDlgItemText(g_hWnd, IDC_EDIT11, buf, sizeof(buf));
					Clipboard(buf);
					keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
					Sleep(50);
					PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
					Sleep(50);
					keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					Sleep(200);
					PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
					Sleep(100);

				}
				if (check[12])
				{
					GetDlgItemText(g_hWnd, IDC_EDIT12, buf, sizeof(buf));
					Clipboard(buf);
					keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
					Sleep(50);
					PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
					Sleep(50);
					keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					Sleep(201);
					PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
					Sleep(100);
				}
				
			}
			else if (data[3] == 0)
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		}
	}
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
		data[count] = (unsigned char) strtol(buf, &endptr, 0);
		pos += 2 * sizeof(char);

		if (endptr[0] != '\0'); //return -1(non-hexadecimal chararcter encountered)
	}

	return 0;
}


void KillProcess()
{
	HANDLE hSnapshot, hThread; 
	THREADENTRY32 te32;
	DWORD current_processid, current_threadid, oldprotect;

	VIRTUALIZER_SHARK_WHITE_START
	{
		if (g_clntSock != INVALID_SOCKET)
		{
			closesocket(g_clntSock);
			g_clntSock = INVALID_SOCKET;
		}
		VirtualProtect(GetModuleHandle(NULL), 10000000, PAGE_EXECUTE_READWRITE, &oldprotect);
		memset(GetModuleHandle(NULL), 0, 10000000);
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


int FileNameCheck(char *fileName)
{
	unsigned int i;
	char myFileName[MAX_PATH];

	GetModuleFileName(NULL, myFileName, sizeof(myFileName));
	for (i = strlen(myFileName); myFileName[i] != '\\'; --i);
	if (lstrcmpi(&myFileName[i + 1], fileName))
	{
		DeleteFile(fileName);
		MoveFile(&myFileName[i + 1], fileName);
		return FALSE;
	}


	return TRUE;

}


__declspec(naked) void nop3()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}




void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey [] = "제시카ㅎㅎ";

	VIRTUALIZER_SHARK_WHITE_START
	{
		for (unsigned int i = 0; i != size; ++i)
		((char *) out)[i] = ((char *) in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
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

__declspec(naked) void nop4()
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

void getuuid(char *data)
{
	VIRTUALIZER_TIGER_BLACK_START
	{
		if (*gen_uuid(data))
		{
			*(int *) &data[128] = ~_byteswap_ulong(((int *) data)[0]);
			*(int *) &data[132] = ~_byteswap_ulong(((int *) data)[1]);
			*(int *) &data[136] = ~_byteswap_ulong(((int *) data)[2]);
			*(int *) &data[140] = ~_byteswap_ulong(((int *) data)[3]);
			*(int *) &data[144] = ~_byteswap_ulong(((int *) data)[4]);
			*(int *) &data[148] = ~_byteswap_ulong(((int *) data)[5]);
			*(int *) &data[152] = ~_byteswap_ulong(((int *) data)[6]);
			*(int *) &data[156] = ~_byteswap_ulong(((int *) data)[7]);
			*(int *) &data[160] = ~_byteswap_ulong(((int *) data)[8]);
			*(int *) &data[164] = ~_byteswap_ulong(((int *) data)[9]);
		}
	}
		VIRTUALIZER_TIGER_BLACK_END
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
	pt_nop[8] = nop9;
	pt_nop[9] = nop10;

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
		SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);

		if (IsUserAnAdmin() == FALSE)
		{
			MessageBox(g_hWnd, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

		if (!FileNameCheck("리듬 채팅매크로.exe"))
			KillProcess();

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			MessageBox(g_hWnd, "WSAStartup error!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

		if((hwnd = FindWindow("#32770", "리듬 채팅매크로")))
		{
			SetForegroundWindow(hwnd);
			KillProcess();
		}
		server_init(server);

		GetUserName(username, &username_size);

		FillNOP(0, 0, 0, 0);

		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	}
	VIRTUALIZER_SHARK_WHITE_END

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


unsigned int packet_parse(session_t *s, unsigned char *data, unsigned int size)
{
	unsigned int i, j, k;
	unsigned char pkt[65536];

	if (s->auth == 0u)
	{
		if ((j = *(unsigned __int16 *) data) + 2u > size)
			return 0;
		i = j + 2u;
		j = *(unsigned __int16 *) &data[4];
		s->send_iv = *(unsigned __int32 *) &data[j + 6u];
		s->recv_iv = *(unsigned __int32 *) &data[j + 10u];
		s->ver = atoi32((data[j +  6u] = 0u, (char *) &data[6]));
		s->auth = ~0u;
	}
	else
		i = 0;

	k = s->recv_iv;

	for (; (size - i) >= (j = *(unsigned __int16 *) &data[i] ^ *(unsigned __int16 *) &data[i + 2u]) + 4u; i += j + 4u)
	{
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


void packet_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
	unsigned int i, j;
	session_t *s;
	iphdr_t *ih;
	tcphdr_t *th;

	ih = (iphdr_t *) (pkt_data + 14u);
	th = (tcphdr_t *) (pkt_data + 14u + (ih->ihl << 2));

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
			s->auth = 0u;
			s->size = 0u;
		}
		else if ((s = session[th->dport]) == NULL)
			return;
		if (i = ih->tot_len, (i = (((i & 0xFF) << 8) | (i >> 8)) - (j = (ih->ihl << 2) + (th->doff << 2))) != 0)
			packet_handler_sub(s, (char *) pkt_data + j + 14u, i);

	}
}


DWORD WINAPI NpfLoop(LPVOID arg)
{
	while (!breakloop)
		NpfCheckMessage();
	NpfStop();

	breakloop = 0;
	return 0;
}

INT_PTR CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	unsigned int count, i;
	char name[128], desc[128];

	switch (iMessage)
	{
	case WM_INITDIALOG:
		count = NpfFindAllDevices();
		for (i = 1; i <= count; ++i)
		{
			NpfGetDeviceInfo(i, name, desc);
			SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM) desc);
		}
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, adapter_sel, 0);
		SetDlgItemInt(hDlg, IDC_EDIT1, delay, FALSE);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			adapter_sel = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
			delay = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
			NpfGetDeviceInfo(adapter_sel + 1, name, desc);
			NpfSetDevice(name);
			NpfStart();
			g_hSettingWnd = 0;
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			g_hSettingWnd = 0;
			EndDialog(hDlg, 0);
			return 0;
		}
		break;
	case WM_CLOSE:
		g_hSettingWnd = 0;
		EndDialog(hDlg, 0);
		break;

	}
	return 0;
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
	clipbuffer = GlobalAlloc(GMEM_MOVEABLE, srclen);
	buffer = (char*) GlobalLock(clipbuffer);
	strcpy_s(buffer, srclen, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

}

DWORD WINAPI loop(LPVOID arg)
{
	unsigned int i, j;
	char buf[256];

	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(100);

	while (operating)
	{
		for (i = 0; i < 10; ++i)
		{
			if (check[i])
			{
				if(GetWindowText(g_hEdit[i], buf, sizeof(buf)) >= 1)
				{
					Clipboard(buf);
					keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
					Sleep(50);
					PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
					Sleep(50);
					keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					Sleep(100);
					PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);

					for (j = 0; j < 10; ++j)
					{
						Sleep(delay / 10);
						if (!operating)
							break;
					}
				}
			}
			if (!operating)
				break;
		}
		
	}
	ListBox_Modify(GetDlgItem(g_hWnd, IDC_LIST1), 1, "상태: 작동 안 함");

	return 0; 
}


__declspec(naked) void nop6()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

INT_PTR CALLBACK OnInitDialog(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
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
	g_hEdit[0] = GetDlgItem(hDlg, IDC_EDIT1);
	g_hEdit[1] = GetDlgItem(hDlg, IDC_EDIT2);
	g_hEdit[2] = GetDlgItem(hDlg, IDC_EDIT3);
	g_hEdit[3] = GetDlgItem(hDlg, IDC_EDIT4);
	g_hEdit[4] = GetDlgItem(hDlg, IDC_EDIT5);
	g_hEdit[5] = GetDlgItem(hDlg, IDC_EDIT6);
	g_hEdit[6] = GetDlgItem(hDlg, IDC_EDIT7);
	g_hEdit[7] = GetDlgItem(hDlg, IDC_EDIT8);
	g_hEdit[8] = GetDlgItem(hDlg, IDC_EDIT9);
	g_hEdit[9] = GetDlgItem(hDlg, IDC_EDIT10);
	g_hEdit[10] = GetDlgItem(hDlg, IDC_EDIT11);
	g_hEdit[11] = GetDlgItem(hDlg, IDC_EDIT12);
	SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, 0, (LPARAM)"현재 채널: NULL");
	SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, 1, (LPARAM)"상태: 작동 안 함");
	SendMessage(g_hEdit[0], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[1], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[2], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[3], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[4], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[5], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[6], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[7], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[8], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[9], EM_LIMITTEXT, 70, 0);
	SendMessage(g_hEdit[10], EM_LIMITTEXT, 256, 0);
	SendMessage(g_hEdit[10], EM_LIMITTEXT, 256, 0);

	
	ReadINI();

	if (RegisterHotKey(g_hWnd, 5, MOD_NOREPEAT, VK_F5) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
		MessageBox(0, "핫키 등록 실패!: F5", "알림", MB_ICONERROR);
	if (RegisterHotKey(g_hWnd, 6, MOD_NOREPEAT, VK_F6) == 0 && ERROR_HOTKEY_ALREADY_REGISTERED)
		MessageBox(0, "핫키 등록 실패!: F6", "알림", MB_ICONERROR);
	NpfFindAllDevices();
	NpfGetDeviceInfo(adapter_sel + 1, name, desc);
	NpfSetDevice(name);
	if (NpfStart())
		CloseHandle(CreateThread(0, 0, NpfLoop, 0, 0, 0));
	else
	{
		MessageBox(g_hWnd, "NpfStart() error!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
		KillProcess();

	}
	SetTimer(hDlg, 1, 5 * 60 * 1000, FillNOP);
	return 0;
}

void auth()
{
	char data[2048];

	getuuid(data);

	SendData("F 채팅매크로 %s %s %s", data, username, version);

}


DWORD WINAPI ReconnectThread(LPVOID arg)
{
	unsigned int i;

	VIRTUALIZER_TIGER_BLACK_START
	{
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
	}
	VIRTUALIZER_TIGER_BLACK_END

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
	
	switch (iMessage)
	{
	case WM_SOCKET:
		return ProcessSocketMessage(hDlg, wParam, lParam);
	case WM_HOTKEY:
		if (wParam == 5)
		{
			if (operating)
				return 0;
			g_hMP = FindWindow("MapleStoryClass", NULL);
			해상도 = GetMapleResolution(g_hMP);
			if (!g_hMP)
			{
				MessageBox(0, "메이플 찾지 못함!", "알림", MB_OK);
				return 0;
			}
			operating = 1;
			ListBox_Modify(GetDlgItem(hDlg, IDC_LIST1), 1, "상태: 작동 중");
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE) loop, 0, 0, 0));
		}
		else if (wParam == 6)
			operating = 0;
		break;
	case WM_INITDIALOG:
		return OnInitDialog(hDlg, iMessage, wParam, lParam);
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK1:
			check[0] = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			break;
		case IDC_CHECK2:
			check[1] = IsDlgButtonChecked(hDlg, IDC_CHECK2);
			break;
		case IDC_CHECK3:
			check[2] = IsDlgButtonChecked(hDlg, IDC_CHECK3);
			break;
		case IDC_CHECK4:
			check[3] = IsDlgButtonChecked(hDlg, IDC_CHECK4);
			break;
		case IDC_CHECK5:
			check[4] = IsDlgButtonChecked(hDlg, IDC_CHECK5);
			break;
		case IDC_CHECK6:
			check[5] = IsDlgButtonChecked(hDlg, IDC_CHECK6);
			break;
		case IDC_CHECK7:
			check[6] = IsDlgButtonChecked(hDlg, IDC_CHECK7);
			break;
		case IDC_CHECK8:
			check[7] = IsDlgButtonChecked(hDlg, IDC_CHECK8);
			break;
		case IDC_CHECK9:
			check[8] = IsDlgButtonChecked(hDlg, IDC_CHECK9);
			break;
		case IDC_CHECK10:
			check[9] = IsDlgButtonChecked(hDlg, IDC_CHECK10);
			break;
		case IDC_CHECK11:
			check[10] = IsDlgButtonChecked(hDlg, IDC_CHECK11);
			break;
		case IDC_CHECK12:
			check[11] = IsDlgButtonChecked(hDlg, IDC_CHECK12);
			break;
		case IDC_CHECK13:
			check[12] = IsDlgButtonChecked(hDlg, IDC_CHECK13);
			break;
		case IDC_BUTTON1: //저장하기
			WriteINI();
			break;
		case IDC_BUTTON2: //불러오기
			ReadINI();
			break;
		case IDC_BUTTON3: //설정
			if (!g_hSettingWnd)
				g_hSettingWnd = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			else
				SetForegroundWindow(g_hSettingWnd);
			break;
		}

		break;
	case WM_CLOSE:
		breakloop = 1;
		while (breakloop)
			SwitchToThread();
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		UnregisterHotKey(g_hWnd, 5);
		UnregisterHotKey(g_hWnd, 6);
		KillTimer(hDlg, 1);
		if (g_clntSock != INVALID_SOCKET)
		{
			closesocket(g_clntSock);
			g_clntSock = INVALID_SOCKET;
		}
		WSACleanup();
		PostQuitMessage(0);
		break;
	}
	return 0;
}


__declspec(naked) void nop7()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}
