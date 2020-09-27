#include "common.h"

char version[] = "1.15";

#define INTERNAL_IP "192.168.219.100"
#define EXTERNAL_IP "182.211.75.197"

SOCKET g_clntSock = INVALID_SOCKET;
char packet_data[4096];
int packet_size;

char username[128];
static void *engine;
static unsigned int lan_flag;
char title[51], pin[17];

image_t *상점나가기, *취소, *상점열기, *상점닫기, *일고랜, *확인, *캐럿, *메소, *확인2;
unsigned int 해상도, check, operating, stop;
HWND g_hWnd, g_hMP, g_hList1;


void dbg(const char *fmt, ...)
{ 
	char msg[1024];

	OutputDebugStringA((wvsprintf(msg, fmt, (va_list) ((unsigned int) &fmt + sizeof(void *))), msg));
}

void ListBox_Modify(HWND hwnd, int sel, char *str)
{
	SendMessage(hwnd, LB_DELETESTRING, sel, 0);
	SendMessage(hwnd, LB_INSERTSTRING, sel, (LPARAM)str);
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


__declspec(naked) void nop1()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
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

	VIRTUALIZER_SHARK_WHITE_START
	{
		if (*packet == 'O')
			check = 1;
		else if (*packet == 'X')
			MessageBox(GetDesktopWindow(), "하드인증 실패!자동 일고랜 이용 불가", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
		else if (*packet == 'N')
			MessageBox(GetDesktopWindow(), "기간 만료!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
		else if (*packet == 'E')
		{
			MessageBox(GetDesktopWindow(), "최신 버전이 아닙니다.\nhttp://182.211.75.197:55에서 파일을 새로 받아주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
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


__declspec(naked) void nop2()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


void auth()
{
	char data[2048];

	dbg(""); //이거빼면 바로꺼짐 ㅅㅂ
	getuuid(data);

	SendData("A 리상 %s %s %s", data, username, version);

	
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

void Clipboard(char *source)
{
	int srclen;
	HGLOBAL clipbuffer;
	char * buffer;

	if (!OpenClipboard(NULL))
		return;

	EmptyClipboard();
	srclen = strlen(source) + 1;
	clipbuffer = GlobalAlloc(GMEM_MOVEABLE, srclen);
	buffer = (char*) GlobalLock(clipbuffer);
	strcpy_s(buffer, srclen, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
}


void MouseMove(int x, int y)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
}

void MouseClick(unsigned int x, unsigned int y, unsigned int k)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
	Sleep(30);
	for (unsigned int i = 0; i < k; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}




void packet_disallow()
{
	FWPM_FILTER0 filter;

	if (lan_flag)
		return;
	VIRTUALIZER_TIGER_BLACK_START
	{
		lan_flag = 1;

		if (engine)
		{
			memset(&filter, 0, sizeof(filter));
			filter.displayData.name = L"<~@X0)5Dg$#?E<~>"; /* Base85 "by movntq" */
			memcpy(&filter.layerKey, "\xBF\xD1\x6F\xC8\xCD\x21\x7E\x49\xA0\xBB\x17\x42\x5C\x88\x5C\x58", 16); /* FWPM_LAYER_INBOUND_IPPACKET_V4 */
			filter.numFilterConditions = 0;
			filter.filterCondition = NULL;
			filter.action.type = 0x00001001; /* FWP_ACTION_BLOCK */
			memcpy(&filter.filterKey, "mbc-cafe-movntq1", 16);
			FwpmFilterAdd0(engine, &filter, NULL, NULL);
			memset(&filter, 0, sizeof(filter));
			filter.displayData.name = L"<~@X0)5Dg$#?E<~>"; /* Base85 "by movntq" */
			memcpy(&filter.layerKey, "\xAE\x9F\x5C\x1E\x84\x8A\x35\x41\xA3\x31\x95\x0B\x54\x22\x9E\xCD", 16); /* FWPM_LAYER_OUTBOUND_IPPACKET_V4 */
			filter.numFilterConditions = 0;
			filter.filterCondition = NULL;
			filter.action.type = 0x00001001; /* FWP_ACTION_BLOCK */
			memcpy(&filter.filterKey, "mbc-cafe-movntq2", 16);
			FwpmFilterAdd0(engine, &filter, NULL, NULL);
		}
	}
		VIRTUALIZER_TIGER_BLACK_END
}

void packet_allow()
{
	GUID guid;

	if (!lan_flag)
		return;
	VIRTUALIZER_TIGER_BLACK_START
	{
		lan_flag = 0;

		if (engine)
		{
			FwpmFilterDeleteByKey0(engine, (GUID *) memcpy(&guid, "mbc-cafe-movntq1", 16));
			FwpmFilterDeleteByKey0(engine, (GUID *) memcpy(&guid, "mbc-cafe-movntq2", 16));
		}
	}
		VIRTUALIZER_TIGER_BLACK_END
}

void packet_init()
{
	FWPM_SESSION0 session;

	VIRTUALIZER_TIGER_BLACK_START
	{
		memset(&session, 0, sizeof(session));
		session.flags = 1; /* FWPM_SESSION_FLAG_DYNAMIC */

		FwpmEngineOpen0(NULL, 0xFFFFFFFF, NULL, &session, &engine);
	}
		VIRTUALIZER_TIGER_BLACK_END
}

void packet_term()
{
	VIRTUALIZER_TIGER_BLACK_START
	{
		if (engine)
		{
			FwpmEngineClose0(engine);
			engine = NULL;
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

unsigned int ImageSearchsub(image_t *from, image_t *target)
{
	unsigned int x, y, z;


	for (y = 0; y < from->ys; ++y)
	{
		if (target->ys > from->ys - y)
			break;
		for (x = 0; x < from->xs; ++x)
		{
			if (target->xs > from->xs - x)
				break;
			if (memcmp(target->data, &from->data[from->xs * y + x], target->xs * 4u) == 0)
				for (z = target->ys;;)
				{
					if (--z == 0)
						return ((y & 65535) << 16) | (x & 65535);
					if (memcmp(&target->data[target->xs * z], &from->data[from->xs * (y + z) + x], target->xs * 4u))
						break;
				}
		}
	}


	return 0xFFFFFFFF;
}


image_t* get_imagesub(HBITMAP hbitmap)
{
	image_t *image;
	HDC hdc;
	struct {
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD bmiColors[256];
	} bmi;

	image = NULL;
	if ((hdc = GetDC(NULL)) != NULL)
	{
		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biBitCount = 0;
		if (GetDIBits(hdc, hbitmap, 0, 0, NULL, (BITMAPINFO *) &bmi, DIB_RGB_COLORS) != 0)
		{
			bmi.bmiHeader.biBitCount = 32; /* 32비트가 아닐경우 이미지 사이즈를 4바이트 정렬 해야됨 */
			if ((image = (image_t *) HeapAlloc(GetProcessHeap(), 0, sizeof(image_t) + ((bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) << 2))) != NULL)
			{
				image->xs = (unsigned int) bmi.bmiHeader.biWidth;
				image->ys = (unsigned int) bmi.bmiHeader.biHeight;
				bmi.bmiHeader.biHeight = -bmi.bmiHeader.biHeight;
				if (GetDIBits(hdc, hbitmap, 0, -bmi.bmiHeader.biHeight, image->data, (BITMAPINFO *) &bmi, DIB_RGB_COLORS) == 0)
				{
					HeapFree(GetProcessHeap(), 0, image);
					image = NULL;
				}
			}
		}
		ReleaseDC(NULL, hdc);
	}

	if (image)
		for (unsigned int y = 0; y < image->ys; ++y)
			for (unsigned int x = 0; x < image->xs; ++x)
				image->data[image->xs * y + x] &= 0xFFFFFF;

	return image;
}





image_t* get_screen(HWND hwnd)
{
	image_t *image;
	RECT rect;
	int xs, ys;
	HBITMAP hbitmap;
	HDC hdc, hmemdc;


	if (!GetClientRect(hwnd, &rect))
	{
		MessageBox(g_hWnd, "메이플 찾지 못함!", "메세지박스", MB_ICONERROR | MB_SYSTEMMODAL);
		return 0;
	}

	xs = rect.right - rect.left;
	ys = rect.bottom - rect.top;


	hdc = GetDC(g_hMP);

	if (xs == 800 && ys == 600)
		해상도 = 800;
	else if (xs == 1024 && ys == 768)
		해상도 = 1024;
	else if (xs == 1366 && ys == 768)
		해상도 = 1366;
	else if (xs == 910 && ys == 512)
		해상도 = 910;
	if (해상도 == 910)
		hbitmap = CreateCompatibleBitmap(hdc, 1366, 768);
	else
		hbitmap = CreateCompatibleBitmap(hdc, xs, ys);
	hmemdc = CreateCompatibleDC(hdc);
	SelectObject(hmemdc, hbitmap);
	if (해상도 == 910)
		BitBlt(hmemdc, 0, 0, 1366, 768, hdc, 0, 0, SRCCOPY);
	else
		BitBlt(hmemdc, 0, 0, xs, ys, hdc, 0, 0, SRCCOPY);

	DeleteDC(hmemdc);
	ReleaseDC(g_hMP, hdc);
	image = get_imagesub(hbitmap);
	DeleteObject(hbitmap);

	return image;
}

unsigned int Global_ImageSearch(image_t *image)
{
	unsigned int i = 0xFFFFFFFF;
	image_t *screen;
	if ((screen = get_screen(g_hMP)) != NULL)
	{
		i = ImageSearchsub(screen, image);
		HeapFree(GetProcessHeap(), 0, screen);
	}
	return i;
}


void 상점개설(int shopSel)
{
	MSG message;
	unsigned int i;

	Sleep(50);

	switch (해상도)
	{
	case 800:
		MouseClick(656, 34, 1);
		break;
	case 1024:
		MouseClick(875, 38, 1);
		break;
	case 1366:
		MouseClick(1217, 37, 1);
		break;
	case 910:
		MouseClick(812, 27, 1);
		break;
	}
	Sleep(101);
	if (shopSel == 0)
	{
		switch (해상도)
		{
		case 800:
			MouseClick(655, 58, 1);
			break;
		case 1024:
			MouseClick(875, 65, 1);
			break;
		case 1366:
			MouseClick(1215, 64, 1);
			break;
		case 910:
			MouseClick(811, 41, 1);
			break;
		}
	}
	else
	{
		switch (해상도)
		{
		case 800:
			MouseClick(688, 63, 1);
			break;
		case 1024:
			MouseClick(912, 66, 1);
			break;
		case 1366:
			MouseClick(1253, 63, 1);
			break;
		case 910:
			MouseClick(835, 45, 1);
			break;
		}
	}
	Sleep(102);
	switch (해상도)
	{
	case 800:
		MouseClick(323, 253, 1);
		break;
	case 1024:
		MouseClick(444, 337, 1);
		break;
	case 1366:
		MouseClick(610, 385, 1);
		break;
	case 910:
		MouseClick(398, 233, 1);
		break;
	}
	Sleep(50);
	for (i = 10; i != 0; --i)
		PostMessage(g_hMP, WM_CHAR, '9', 0);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);
	for (i = 15; i != 0; --i)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				ExitProcess(0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			switch (해상도)
			{
			case 800:
				MouseClick(321, 121, 2);
				break;
			case 1024:
				MouseClick(433, 202, 2);
				break;
			case 1366:
				MouseClick(600, 200, 2);
				break;
			case 910:
				MouseClick(405, 136, 2);
				break;
			}
			Sleep(110);
			switch (해상도)
			{
			case 800:
				MouseClick(660, 36, 2);
				break;
			case 1024:
				MouseClick(881, 35, 2);
				break;
			case 1366:
				MouseClick(1220, 35, 2);
				break;
			case 910:
				MouseClick(811, 23, 2);
				break;
			}
			Sleep(100);
			if (Global_ImageSearch(상점열기) == -1 || stop)
				break;
		}
	}

}



__declspec(naked) void nop3()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


DWORD WINAPI 일고랜작(LPVOID *arg)
{
	int i;

	
	if (GetWindowTextLength(GetDlgItem(g_hWnd, IDC_EDIT1)) == 0)
		SetDlgItemText(g_hWnd, IDC_EDIT1, "리듬 리상");
	GetDlgItemText(g_hWnd, IDC_EDIT1, title, sizeof(title));

	SetForegroundWindow(g_hMP);
	Sleep(101);
	if ((i = Global_ImageSearch(상점닫기)) == -1)
	{
		MessageBox(g_hWnd, "상점닫기 버튼 보이지 않음!", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		goto $exit;
	}
	ListBox_Modify(g_hList1, 0, "상태: 일고랜작 중");
	Sleep(50);
	if (해상도 == 800)
		MouseClick(773, 39, 1);
	else if (해상도 == 1024)
		MouseClick(999, 36, 1);
	else if (해상도 == 1366)
		MouseClick(1341, 38, 1);
	else if (해상도 == 910)
		MouseClick(894, 24, 1);
	Sleep(50);
	if (해상도 == 800)
		MouseMove(691, 64);
	else if (해상도 == 1024)
		MouseMove(915, 65);
	else if (해상도 == 1366)
		MouseMove(1256, 64);
	else if (해상도 == 910)
		MouseMove(836, 42);
	Sleep(100);
	packet_disallow();
	Sleep(200);
	for (unsigned int i = 0; i < 1000; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
	Sleep(500);
	if (해상도 == 910)
		MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
	else
		MouseClick(i & 65535, (i >> 16), 1);
	Sleep(300);
	do
	{
		Sleep(50);
		if (해상도 == 800)
			MouseClick(652, 65, 2);
		else if (해상도 == 1024)
			MouseClick(879, 62, 2);
		else if (해상도 == 1366)
			MouseClick(1222, 65, 2);
		else if (해상도 == 910)
			MouseClick(813, 43, 2);
		Sleep(50);
	}
	while (Global_ImageSearch(취소) == -1 && !stop);
	if (stop)
		goto $exit;
	PostMessage(g_hMP, WM_CHAR, '/', 0);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
	Sleep(500);
	packet_allow();
	while (Global_ImageSearch(상점열기) == -1 && !stop)
		Sleep(50);
	if (stop)
		goto $exit;
	Sleep(200);
	packet_disallow();
	Sleep(500);
	PostMessage(g_hMP, WM_KEYDOWN, VK_ESCAPE, 0);
	Sleep(200);
	if (Global_ImageSearch(취소) != -1)
	{
		if (해상도 == 800)
			MouseClick(311, 328, 2);
		else if (해상도 == 1024)
			MouseClick(428, 411, 2);
		else if (해상도 == 1366)
			MouseClick(607, 412, 2);
		else if (해상도 == 910)
			MouseClick(410, 275, 2);
		while (Global_ImageSearch(취소) == -1 && !stop)
			Sleep(50);
		if (stop)
			goto $exit;
		do
		{
			Clipboard(title);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
			Sleep(50);
			PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
			Sleep(50);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(100);
		} while (Global_ImageSearch(캐럿) != -1 && !stop);
		if (stop)
			goto $exit;
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(50);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(500);
		packet_allow();
		while (Global_ImageSearch(상점열기) == -1 && !stop)
			Sleep(50);
		if (stop)
			goto $exit;
		상점개설(1);
	}
	else
	{
		if (해상도 == 800)
			MouseClick(656, 64, 2);
		else if (해상도 == 1024)
			MouseClick(879, 62, 2);
		else if (해상도 == 1366)
			MouseClick(1219, 61, 2);
		else if (해상도 == 910)
			MouseClick(814, 41, 2);
		while (Global_ImageSearch(상점열기) == -1 && !stop)
			Sleep(50);
		if (stop)
			goto $exit;
		일고랜작(0);
	}
	
$exit:;
	operating = 0;
	ListBox_Modify(g_hList1, 0, "상태: 작동 안 함");
	return 0;
}

DWORD WINAPI 일상펴기(LPVOID arg)
{
	unsigned int i;
	MSG message;
	 
	
	if (GetWindowTextLength(GetDlgItem(g_hWnd, IDC_EDIT1)) == 0)
		SetDlgItemText(g_hWnd, IDC_EDIT1, "리듬 리상");
	GetDlgItemText(g_hWnd, IDC_EDIT1, title, sizeof(title));

	SetForegroundWindow(g_hMP);
	Sleep(100);
	if (Global_ImageSearch(메소) == -1) 
	{
		if (해상도 == 800)
			MouseClick(712, 551, 1);
		else if (해상도 == 1024 || 해상도 == 1366)
			MouseClick(718, 721, 1);
		else if (해상도 == 910)
			MouseClick(477, 481, 1);
	}
	Sleep(105);
	if (해상도 == 800)
		MouseClick(773, 36, 1);
	else if (해상도 == 1024)
		MouseClick(998, 36, 1);
	else if (해상도 == 1366)
		MouseClick(1342, 36, 1);
	else if (해상도 == 910)
		MouseClick(892, 27, 1);
	Sleep(50);
	if ((i = Global_ImageSearch(상점닫기)) != -1)
	{
		ListBox_Modify(g_hList1, 0, "상태: 일일리상 중");
		packet_disallow();
		Sleep(200); 
		if (해상도 == 910)
			MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
		else
			MouseClick(i & 65535, (i >> 16), 1);
	}
	else
		ListBox_Modify(g_hList1, 0, "상태: 일상 펴는 중");
	Sleep(100);
	do
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				packet_allow();
				ExitProcess(0);
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Sleep(50);
			if (해상도 == 800)
				MouseClick(652, 65, 2);
			else if (해상도 == 1024)
				MouseClick(879, 62, 2);
			else if (해상도 == 1366)
				MouseClick(1222, 65, 2);
			else if (해상도 == 910)
				MouseClick(813, 43, 2);
			Sleep(50);
		}
	} while (Global_ImageSearch(취소) == -1 && !stop);
	if (stop)
		goto $exit;
	do
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				packet_allow();
				ExitProcess(0);
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Clipboard(title);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
			Sleep(50);
			PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
			Sleep(50);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(100);
		}
	} while (Global_ImageSearch(캐럿) != -1 && !stop);
	if (stop)
		goto $exit;
	Sleep(50);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(200);
	packet_allow();
	while (!stop)
	{
		if (Global_ImageSearch(상점열기) != -1)
			break;
		else if (Global_ImageSearch(확인) != -1)
		{
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			goto $exit;
		}
		Sleep(50);
	}
	if (stop)
		goto $exit;
	상점개설(0);
$exit:;
	operating = 0;
	ListBox_Modify(g_hList1, 0, "상태: 작동 안 함");
	
	return 0;
}


__declspec(naked) void nop4()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


DWORD WINAPI 고상펴기(LPVOID arg)
{
	MSG message;
	unsigned int i;

	
	if (GetWindowTextLength(GetDlgItem(g_hWnd, IDC_EDIT1)) == 0)
		SetDlgItemText(g_hWnd, IDC_EDIT1, "리듬 리상");
	GetDlgItemText(g_hWnd, IDC_EDIT1, title, sizeof(title));

	SetForegroundWindow(g_hMP);
	Sleep(100);
	if (Global_ImageSearch(메소) == -1)
	{
		if (해상도 == 800)
			MouseClick(712, 551, 1);
		else if (해상도 == 1024 || 해상도 == 1366)
			MouseClick(718, 721, 1);
		else if (해상도 == 910)
			MouseClick(477, 481, 1);	
	}
	Sleep(51);
	if (해상도 == 800)
		MouseClick(773, 36, 1);
	else if (해상도 == 1024)
		MouseClick(998, 36, 1);
	else if (해상도 == 1366)
		MouseClick(1342, 36, 1);
	else if (해상도 == 910)
		MouseClick(892, 27, 1);
	Sleep(100);
	if ((i = Global_ImageSearch(상점닫기)) != -1)
	{
		ListBox_Modify(g_hList1, 0, "상태: 일고리상 중");
		if (해상도 == 910)
			MouseClick((unsigned int)((i & 65535) * 0.67), (unsigned int)((i >> 16) * 0.67), 1);
		else
			MouseClick(i & 65535, (i >> 16), 1);
	}
	else
		ListBox_Modify(g_hList1, 0, "상태: 고상 펴는 중");
	Sleep(200);
	do
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				packet_allow();
				ExitProcess(0);
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Sleep(50);
			if (해상도 == 800)
				MouseClick(690, 65, 2);
			else if (해상도 == 1024)
				MouseClick(916, 65, 2);
			else if (해상도 == 1366)
				MouseClick(1256, 67, 2);
			else if (해상도 == 910)
				MouseClick(837, 47, 2);
			Sleep(50);
		}
	} while (Global_ImageSearch(취소) == -1 && !stop);
	if (stop)
		goto $exit;
	do
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				packet_allow();
				ExitProcess(0);
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Clipboard(title);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
			Sleep(50);
			PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
			Sleep(50);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(100);
		}
	} while (Global_ImageSearch(캐럿) != -1 && !stop);
	if (stop)
		goto $exit;
	Sleep(50);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(51);
	while (!stop)
	{
		if (Global_ImageSearch(상점열기) != -1)
			break;
		else if (Global_ImageSearch(확인) != -1)
		{
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			goto $exit;
		}
		Sleep(50);
	}
	if (stop)
		goto $exit;
	상점개설(1);
$exit:;
	operating = 0;
	ListBox_Modify(g_hList1, 0, "상태: 작동 안 함");

	return 0;
}



DWORD WINAPI 고고리상(LPVOID arg)
{
	unsigned int i;

	
	if (GetWindowTextLength(GetDlgItem(g_hWnd, IDC_EDIT1)) == 0)
		SetDlgItemText(g_hWnd, IDC_EDIT1, "리듬 리상");
	GetDlgItemText(g_hWnd, IDC_EDIT1, title, sizeof(title));
	if (GetWindowTextLength(GetDlgItem(g_hWnd, IDC_EDIT2)) == 0)
	{
		MessageBox(g_hWnd, "2차비번을 입력해주세요", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
		goto $exit;
	}
	GetDlgItemText(g_hWnd, IDC_EDIT2, pin, sizeof(pin));

	SetForegroundWindow(g_hMP);
	Sleep(100);

	ListBox_Modify(g_hList1, 0, "상태: 고고리상 중");

	while (Global_ImageSearch(취소) == -1 && !stop)
		Sleep(50);
	if (stop)
		goto $exit;
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);
	MouseMove(30, 30);
	for (i = 0; i < 5; ++i)
	{
		if (Global_ImageSearch(확인2) != -1)
			break;
		Sleep(50);
	}
	if (Global_ImageSearch(확인2) == -1)
		goto $exit;
	for (i = 0; i < 3; ++i)
	{
		Clipboard(pin);
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
		Sleep(50);
		PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
		Sleep(50);
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		Sleep(107);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(200);
		if (Global_ImageSearch(확인2) == -1)
			break;
		else if (i == 2)
			goto $exit;
	}
	while ((i = Global_ImageSearch(상점닫기)) == -1 && !stop)
		Sleep(50);
	if (stop)
		goto $exit;

	if (해상도 == 800)
		MouseClick(773, 36, 1);
	else if (해상도 == 1024)
		MouseClick(998, 36, 1);
	else if (해상도 == 1366)
		MouseClick(1342, 36, 1);
	else if (해상도 == 910)
		MouseClick(891, 24, 1);
	Sleep(100);
	if (해상도 == 910)
		MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
	else if (i != -1)
		MouseClick(i & 65535, (i >> 16), 1);
	Sleep(100);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(100);
	while (Global_ImageSearch(확인) == -1 && !stop)
		Sleep(50);
	if (stop)
		goto $exit;
$1:;
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);
	do
	{
		if (해상도 == 800)
			MouseClick(690, 65, 2);
		else if (해상도 == 1024)
			MouseClick(916, 65, 2);
		else if (해상도 == 1366)
			MouseClick(1256, 67, 2);
		else if (해상도 == 910)
			MouseClick(837, 47, 2);
		Sleep(50);
				
	} while (Global_ImageSearch(취소) == -1 && !stop);
	if (stop)
		goto $exit;
	do
	{
		Clipboard(title);
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
		Sleep(50);
		PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
		Sleep(50);
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		Sleep(100);
			
	} while (Global_ImageSearch(캐럿) != -1 && !stop);
	if (stop)
		goto $exit;
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);
	while (!stop)
	{
		if (Global_ImageSearch(상점열기) != -1)
			break;
		else if (Global_ImageSearch(확인) != -1)
			goto $1;
		Sleep(50);
	}
	if (stop)
		goto $exit;
	상점개설(1);
		

$exit:;
	operating = 0;
	ListBox_Modify(g_hList1, 0, "상태: 작동 안 함");
	
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
		g_hMP = FindWindow("MapleStoryClass", NULL);
		switch (wParam)
		{
		case 4: //고고리상
			if (operating)
				return 0;
			operating = 1;
			stop = 0;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE) 고고리상, 0, 0, 0);
			break;
		case 5: //고상펴기(일고리상)
			if (operating)
				return 0;
			operating = 1;
			stop = 0;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE) 고상펴기, 0, 0, 0);
			break;
		case 6://일상피기(일일리상)
			if (operating)
				return 0;
			operating = 1;
			stop = 0;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE) 일상펴기, 0, 0, 0);
			break;
		case 7://작동 중지
			stop = 1;
			operating = 0;
			packet_allow();
			break;
		}
		break;
	case WM_INITDIALOG: 
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
		g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
		상점나가기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1)));
		상점닫기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2)));
		상점열기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3)));
		취소 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP4)));
		캐럿 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP5)));
		확인 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP6)));
		메소 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP7)));
		확인2 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP8)));
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1)));
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, 50, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT2, EM_LIMITTEXT, 16, 0);
		g_hMP = FindWindow("MapleStoryClass", NULL);
		SetDlgItemText(hDlg, IDC_EDIT1, "리듬 리상");
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM)"상태: 작동 안 함");
		packet_init();
		
		RegisterHotKey(g_hWnd, 4, MOD_NOREPEAT, VK_F4);
		RegisterHotKey(g_hWnd, 5, MOD_NOREPEAT, VK_F5);
		RegisterHotKey(g_hWnd, 6, MOD_NOREPEAT, VK_F6);
		RegisterHotKey(g_hWnd, 7, MOD_NOREPEAT, VK_F7);
		CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
		SetFocus(GetDlgItem(hDlg, IDC_BUTTON1));
		SetTimer(hDlg, 1, 5 * 60 * 1000, FillNOP);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK1:
			if (IsDlgButtonChecked(hDlg, IDC_CHECK1) == BST_CHECKED)
			{
				RegisterHotKey(g_hWnd, 4, MOD_NOREPEAT, VK_F4);
				RegisterHotKey(g_hWnd, 5, MOD_NOREPEAT, VK_F5);
				RegisterHotKey(g_hWnd, 6, MOD_NOREPEAT, VK_F6);
				RegisterHotKey(g_hWnd, 7, MOD_NOREPEAT, VK_F7);
			}
			else
			{
				UnregisterHotKey(g_hWnd, 4);
				UnregisterHotKey(g_hWnd, 5);
				UnregisterHotKey(g_hWnd, 6);
				UnregisterHotKey(g_hWnd, 7);
			}
			SetFocus(GetDlgItem(hDlg, IDC_BUTTON1));
			break;
		case IDC_BUTTON1:
			VIRTUALIZER_TIGER_BLACK_START
			{
				if (!check || operating)
					goto $exit;
				operating = 1;
				stop = 0;
				if (MessageBox(hDlg, "영정 먹을 확률 있음.계속하려면 확인 누르셈\n마우스 움직이면 삑날 수 있음", "알림", MB_ICONSTOP | MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK)
					CreateThread(0, 0, (LPTHREAD_START_ROUTINE) 일고랜작, 0, 0, 0);
			$exit:;
			}
			VIRTUALIZER_TIGER_BLACK_END
			break;
		case IDC_BUTTON2: //사용법
			MessageBox(g_hWnd, "핫키 체크박스는 체크하면 핫키 등록하고 체크 해제하면 등록 해제함\n템창은 안 열려있어도 알아서 엶. 템창에 메소버튼만 안 가리면 됨\n일상 펴져있는 상태로 핫키 누르면 일일리상, 고고리상 됨\n고고리상은 F4누르고 자기 고상 더블클릭하면 알아서 됨\n일일리상은 랜작식이고 자동일고랜은 광클식이라 영정 확률이 극미하게 있음", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			break;
		}
		break;
	case WM_CLOSE:
		DeleteObject(상점나가기);
		DeleteObject(상점닫기);
		DeleteObject(상점열기);
		DeleteObject(취소);
		DeleteObject(캐럿);
		DeleteObject(확인);
		DeleteObject(메소);
		DeleteObject(확인2);
		packet_allow();
		packet_term();
		if (g_clntSock != INVALID_SOCKET)
		{
			closesocket(g_clntSock);
			g_clntSock = INVALID_SOCKET;
		}
		WSACleanup();
		UnregisterHotKey(g_hWnd, 4);
		UnregisterHotKey(g_hWnd, 5);
		UnregisterHotKey(g_hWnd, 6);
		UnregisterHotKey(g_hWnd, 7);
		KillTimer(hDlg, 1);
		PostQuitMessage(0);
		break;
	}
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




__declspec(naked) void nop7()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


VOID CALLBACK FillNOP(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	unsigned char code[NOP_SIZE];
	DWORD oldprotect;
	unsigned int i, j;
	void(*pt_nop[7])() = { 0, };
	pt_nop[0] = nop1;
	pt_nop[1] = nop2;
	pt_nop[2] = nop3;
	pt_nop[3] = nop4;
	pt_nop[4] = nop5;
	pt_nop[5] = nop6;
	pt_nop[6] = nop7;

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
	//SYSTEMTIME st;
	typedef void (WINAPI *PFN_GET_NATIVE_SYSTEM_INFO)(LPSYSTEM_INFO);
	PFN_GET_NATIVE_SYSTEM_INFO pGetNativeSystemInfo;
	SYSTEM_INFO SystemInfo;
	WSADATA wsaData;
	
	VIRTUALIZER_SHARK_WHITE_START
	{
		SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);
		 
		if (IsUserAnAdmin() == FALSE)
		{
			MessageBox(g_hWnd, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

		pGetNativeSystemInfo = (PFN_GET_NATIVE_SYSTEM_INFO) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
		if (pGetNativeSystemInfo)
			pGetNativeSystemInfo(&SystemInfo);
		else
			GetNativeSystemInfo(&SystemInfo);
		if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		{
			MessageBox(g_hWnd, "32비트 미지원", "메세지박스", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

		if (!FileNameCheck("리듬 리상.exe"))
			KillProcess();


		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			MessageBox(g_hWnd, "WSAStartup error!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

		if (FindWindow("#32770", "리듬 리상"))
		{
			SetForegroundWindow(FindWindow("#32770", "리듬 리상"));
			KillProcess();
		}

		/*GetLocalTime(&st);
		if (st.wMonth == 2 && st.wDay == 10)
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), 0, BirthdayDlgProc);*/
		init_rand(GetCurrentProcessId() ^ 0xBADF00Du);

		GetUserName(username, &username_size);


		FillNOP(0, 0, 0, 0);

		CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
		MessageLoop();

	}
	VIRTUALIZER_SHARK_WHITE_END
	return 0;
}