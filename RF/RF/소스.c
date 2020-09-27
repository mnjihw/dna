#include "RF.h"


typedef struct {
	unsigned int size;
	unsigned char data[60];
} packet_t;
extern char* gen_uuid(char buffer[48]);

HINSTANCE g_hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void NpfStop(void);
void NpfCheckMessage(void);
LPSTR lpszClass = "";
HWND g_hWnd, g_hMP, g_hList1, g_hList2, g_hCheck;
unsigned int userId, signalState, startup, shopSel, wariState, 갈바Sel, adapterSel, sendState, stop, 복호화Sel, 해상도;
unsigned int USER_MINIROOM_BALLON, EMPLOYEE_LEAVE, SERVERINFO, USERINFO, S_MINIROOM, S_MINIROOM_CREATE, SHOP_INFO;
char shopTitle[128] = "　";
packet_t packet;
int open_success;
image_t *상점, *상점나가기, *취소, *상점열기, *상점닫기, *일고랜, *확인;

typedef struct session
{
	unsigned int auth, recv_iv, send_iv, ver, size;
	unsigned char data[128 * 1024];
} session_t;

ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];

void MouseMove(int x, int y)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
}

void MouseClick(int x, int y, int k)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
	for (int i = 0; i < k; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}

void SendPacket(const void *data, unsigned int size)
{
	COPYDATASTRUCT cds;


	if (IsWindow(g_hMP) == FALSE)
		g_hMP = FindWindow("MapleStoryClass", NULL);
	if (g_hMP)
	{
		cds.dwData = 0x02100713;
		cds.cbData = size;
		cds.lpData = (void *)data;
		SendMessage(g_hMP, WM_COPYDATA, 0, (LPARAM)&cds);
	}
}


static void *engine;
static unsigned int lan_flag;

void packet_disallow()
{
	FWPM_FILTER0 filter;

	if (lan_flag)
		return;
	VIRTUALIZER1_START
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
	VIRTUALIZER_END
}

void packet_allow()
{
	GUID guid;

	if (!lan_flag)
		return;
	VIRTUALIZER1_START
	{
		lan_flag = 0;

		if (engine)
		{
			FwpmFilterDeleteByKey0(engine, (GUID *)memcpy(&guid, "mbc-cafe-movntq1", 16));
			FwpmFilterDeleteByKey0(engine, (GUID *)memcpy(&guid, "mbc-cafe-movntq2", 16));
		}
	}
	VIRTUALIZER_END
}

void packet_init()
{
	FWPM_SESSION0 session;

	VIRTUALIZER1_START
	{
		memset(&session, 0, sizeof(session));
		session.flags = 1; /* FWPM_SESSION_FLAG_DYNAMIC */

		FwpmEngineOpen0(NULL, 0xFFFFFFFF, NULL, &session, &engine);
	}
	VIRTUALIZER_END
}

void packet_term()
{
	VIRTUALIZER1_START
	{
		if (engine)
		{
			FwpmEngineClose0(engine);
			engine = NULL;
		}
	}
	VIRTUALIZER_END
}


int FileNameCheck(char *fileName)
{
	int i;
	char myFileName[128];

	GetModuleFileName(NULL, myFileName, sizeof(myFileName));
	for (i = strlen(myFileName); myFileName[i] != '\\'; --i);
	if (lstrcmpi(&myFileName[i + 1], fileName))
	{
		MoveFile(fileName, "RF(old).exe");
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
		if (GetDIBits(hdc, hbitmap, 0, 0, NULL, (BITMAPINFO *)&bmi, DIB_RGB_COLORS) != 0)
		{
			bmi.bmiHeader.biBitCount = 32; /* 32비트가 아닐경우 이미지 사이즈를 4바이트 정렬 해야됨 */
			if ((image = (image_t *)HeapAlloc(GetProcessHeap(), 0, sizeof(image_t)+((bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) << 2))) != NULL)
			{
				image->xs = (unsigned int)bmi.bmiHeader.biWidth;
				image->ys = (unsigned int)bmi.bmiHeader.biHeight;
				bmi.bmiHeader.biHeight = -bmi.bmiHeader.biHeight;
				if (GetDIBits(hdc, hbitmap, 0, -bmi.bmiHeader.biHeight, image->data, (BITMAPINFO *)&bmi, DIB_RGB_COLORS) == 0)
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
	POINT pt;
	RECT rect;
	int ys, xs;
	HBITMAP hbitmap;
	HDC hdc, hmemdc;


	if (!GetClientRect(hwnd, &rect))
	{
		MessageBox(0, "메이플 찾지 못함!", "메세지박스", MB_OK);
		return 0;
	}
	pt.y = pt.x = 0;
	ClientToScreen(hwnd, &pt); 



	hdc = GetDC(NULL);

	ys = rect.bottom - rect.top;
	xs = rect.right - rect.left;
	
	if (xs == 800 && ys == 600)
		해상도 = 800;
	else if (xs == 1024 && ys == 768)
		해상도 = 1024;
	else if (xs == 1366 && ys == 768)
		해상도 = 1366;
	
	hbitmap = CreateCompatibleBitmap(hdc, xs, ys);
	hmemdc = CreateCompatibleDC(hdc);
	SelectObject(hmemdc, hbitmap);
	BitBlt(hmemdc, 0, 0, xs, ys, hdc, pt.x, pt.y, SRCCOPY);
	DeleteDC(hmemdc);
	ReleaseDC(NULL, hdc);
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



DWORD WINAPI NpfLoop(LPVOID arg)
{
	
	for (;;)
		NpfCheckMessage();
	NpfStop();
	
	
	return 0;
}

void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey[] = "제시카ㅎㅎ";

	for (int i = 0; i != size; ++i)
		((char *)out)[i] = ((char *)in)[i] ^ CryptKey[i % (sizeof(CryptKey)-1)];
}

u_long host2ip(const char *host)
{
	u_long ipaddr;
	struct hostent *h;

	if ((ipaddr = inet_addr(host)) == INADDR_NONE)
	if ((h = gethostbyname(host)) != NULL)
		ipaddr = *(u_long *)h->h_addr;

	return ipaddr;
}



SOCKET ConnectToServer()
{
	SOCKET hSock;
	SOCKADDR_IN servAdr;

	VIRTUALIZER1_START
	{
		hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = host2ip("mnjihw.oa.to");
		servAdr.sin_port = htons(atoi("8915"));
		if (connect(hSock, (SOCKADDR*) &servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		{
			closesocket(hSock);
			hSock = INVALID_SOCKET;
		}
	}
	VIRTUALIZER_END
	return hSock;
	
}


void 일상재대기()
{
	MSG message;
	int i;

	PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
	SetForegroundWindow(g_hMP);
	BlockInput(1);
	Sleep(100);

	switch (해상도)
	{
	case 800:
		MouseClick(773, 36, 1);
		break;
	case 1024:
		MouseClick(998, 36, 1);
		break;
	case 1366:
		MouseClick(1342, 36, 1);
		break;
	}
	Sleep(100);
	switch (해상도)
	{
	case 800:
		MouseClick(652, 65, 2);
		break;
	case 1024:
		MouseClick(879, 62, 2);
		break;
	case 1366:
		MouseClick(1222, 65, 2);
		break;
	}
	for (i = 0; Global_ImageSearch(취소) == -1; ++i)
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
			if (i < 20)
				break;
			if (stop)
				return;
			Sleep(50);
			switch (해상도)
			{
			case 800: 
				MouseClick(652, 65, 2);
				break;
			case 1024:
				MouseClick(879, 62, 2);
				break;
			case 1366:
				MouseClick(1222, 65, 2);
				break;
			}
			Sleep(50);
		}
	}
	if (stop)
		return;


	
	keybd_event('1', MapVirtualKey(2, 0), 0, 0);
	keybd_event('1', MapVirtualKey(2, 0), KEYEVENTF_KEYUP, 0);
	

	BlockInput(0);
}

void 고상재대기()
{
	MSG message;
	

	PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
	SetForegroundWindow(g_hMP);
	BlockInput(1);
	Sleep(100);
	switch (해상도)
	{
	case 800:
		MouseClick(773, 36, 1);
		break;
	case 1024:
		MouseClick(998, 36, 1);
		break;
	case 1366:
		MouseClick(1342, 36, 1);
		break;
	}
	Sleep(100);
	switch (해상도)
	{
	case 800:
		MouseClick(690, 65, 2);
		break;
	case 1024:
		MouseClick(916, 65, 2);
		break;
	case 1366:
		MouseClick(1256, 67, 2);
		break;
	}
	while (Global_ImageSearch(취소) == -1)
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
			if (stop)
				return;
			Sleep(50);
			switch (해상도)
			{
			case 800:
				MouseClick(690, 65, 2);
				break;
			case 1024:
				MouseClick(916, 65, 2);
				break;
			case 1366:
				MouseClick(1256, 67, 2);
				break;
			}
			Sleep(50);
		}
	}
	if (stop)
		return;
	keybd_event('1', MapVirtualKey(2, 0), 0, 0);
	keybd_event('1', MapVirtualKey(2, 0), KEYEVENTF_KEYUP, 0);


	Sleep(100);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
	BlockInput(0);
}

void 상점개설(int shopSel)
{
	MSG message;
	int i;

	PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
	SetForegroundWindow(g_hMP);
	BlockInput(1);
	Sleep(100);
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
	}
	Sleep(100);
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
			MouseClick(1226, 65, 1);
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
		}
	}
	Sleep(100);
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
	}
	Sleep(100);
	for (i = 0; i < 10; ++i)
		PostMessage(g_hMP, WM_CHAR, '9', 0);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
	Sleep(50);
	for (i = 0; i < 15; ++i)
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
			}
			Sleep(100);
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
			}
			Sleep(100);
			if (Global_ImageSearch(상점열기) == -1)
				break;
			if (stop)
				break;
		}
	}
	BlockInput(0);
	signalState = 1;
}

DWORD WINAPI ImageSearchLoop(LPVOID *arg)
{
	while (!stop)
	{
		if (Global_ImageSearch(상점열기) != -1 || open_success == 1)
		{
			if (SendMessage(g_hCheck, BM_GETCHECK, BST_CHECKED, 0))
				PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
			signalState = 0;
			상점개설(shopSel);
			SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON3, 0);
			open_success = 0;
			return 0;
		}
		else if (Global_ImageSearch(확인) != -1 || open_success == -1)
		{
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
			if (SendMessage(g_hCheck, BM_GETCHECK, BST_CHECKED, 0))
				PlaySound(MAKEINTRESOURCE(IDR_WAVE2), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
			signalState = 0;
			if (!sendState)
			{
				if (shopSel == 0)
					일상재대기();
				else
					고상재대기();
			}
			signalState = 1;
			open_success = 0;
			return 0;
		}
		Sleep(50);
	}
	return 0;
}

void packet_handle(session_t *s, unsigned char *data, unsigned int size)
{
	static SOCKET hSock;
	static unsigned int miniroomCount, employeeCount, currentChannel;
	char buf[128];
	

	if (*(unsigned short*)data == SERVERINFO)
	{
		if (*(unsigned char*)&data[2] > 19)
			return;
		else if (*(unsigned char*)&data[2] == 0)
			currentChannel = 1;
		else if (*(unsigned char*)&data[2] == 1)
			currentChannel = 20;
		else
			currentChannel = *(unsigned char*)&data[2];
		if (currentChannel == 20)
			wsprintf(buf, "현재 채널: %d세", currentChannel);
		else
			wsprintf(buf, "현재 채널: %d채널", currentChannel);
		SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
		SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)buf);
	}
	else if (*(unsigned short*)data == USERINFO)
	{
		if (userId)
			return;
		userId = *(unsigned int*)&data[2];
		wsprintf(buf, "캐릭터 고유 ID: %X", userId);
		SendMessage(g_hList1, LB_DELETESTRING, 0, 0);
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM)buf);
	}
	else if (*(unsigned short*)data == USER_MINIROOM_BALLON)
	{
		if (size != 0x7 || !signalState)
			return;
		if (갈바Sel) //갈바Sel이 0이면 OFF 1이면 ON
		if (*(unsigned int*)&data[2] != userId || userId == 0)
			return;
		if (sendState)
			SendPacket(packet.data, packet.size);
		else
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
		wsprintf(buf, "일상 반응횟수: %d회", ++miniroomCount);
		SendMessage(g_hList2, LB_DELETESTRING, 0, 0);
		SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)buf);
		PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
		SetForegroundWindow(g_hMP);
		Sleep(300);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ImageSearchLoop, 0, 0, 0);
	}
	else if (*(unsigned short*)data == EMPLOYEE_LEAVE)
	{
		if (!signalState)
			return;
		if (갈바Sel)
		if (userId != 0)
			return;
		if (sendState)
			SendPacket(packet.data, packet.size);
		else
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
		wsprintf(buf, "고상 반응횟수: %d회", ++miniroomCount);
		SendMessage(g_hList2, LB_DELETESTRING, 1, 0);
		SendMessage(g_hList2, LB_INSERTSTRING, 1, (LPARAM)buf);
		PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
		SetForegroundWindow(g_hMP);
		Sleep(300);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ImageSearchLoop, 0, 0, 0);
	}
	else if (*(unsigned short*)data == SHOP_INFO)
	{
		if (*(unsigned char*)&data[4] == 0x7) //개설 성공
			open_success = 1;
		else if (*(unsigned char*)&data[4] == 0xD) //개설 실패
			open_success = -1;
	}
	/*else if (*(unsigned short*)data == 0x14E) // 맵?방? 바뀔 때 오는 패킷
	{
		nicksize = *(unsigned short*)&data[7];
		memcpy(nick, &data[9], nicksize);
		nick[nicksize] = '\0';
		if (strcmp(nick, "WorldRecord") == 0)
			return;
		hSock = ConnectToServer();
		if (hSock == INVALID_SOCKET)
			return;
		nicksize = wsprintf(buf, "RN %s", nick);
		RFCrypt(nicksize, buf, buf);
		send(hSock, buf, nicksize, 0);
		closesocket(hSock);

	}*/
	/*
	switch (*(unsigned short*)data)
	{

		case 0x2FF:
			if (*(unsigned char*)&data[2] != 0x14 || !signalState)
				return;
			if (*(unsigned char*)&data[4] == 0xD) //개설실패
			{
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
				if (SendMessage(g_hCheck, BM_GETCHECK, BST_CHECKED, 0))
					PlaySound(MAKEINTRESOURCE(IDR_WAVE2), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
				if (!sendState)
				{
					if (shopSel == 0)
						SendMessage(g_hWnd, W_USER + 3, 0, 0);
					else
						SendMessage(g_hWnd, WMM_USER + 3, 0, 0);
					else
						SendMessage(g_hWnd, WM_USER + 4, 0, 0);
				}
			}
			else if ((*(unsigned char*)&data[3] == 0x5 || *(unsigned char*)&data[3] == 0x6) && *(unsigned char*)&data[5] == 0x0) //개설성공(일,고상)
			{
				if (SendMessage(g_hCheck, BM_GETCHECK, BST_CHECKED, 0))
					PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
				SendMessage(g_hWnd, WM_USER + 5, 0, 0);
			}
			break
	}
	*/
	
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
		/*dbg("S=%08X R=%08X v%u\n", s->send_iv, s->recv_iv, s->ver & 65535);
		dump(data, i);*/
	}
	else
		i = 0;

	k = s->recv_iv;

	for (; (size - i) >= (j = *(unsigned __int16 *)&data[i] ^ *(unsigned __int16 *)&data[i + 2u]) + 4u; i += j + 4u) {
		/*dbg("%u bytes HEAD=%04X, EXPECTED=%04X\n", j, *(unsigned __int16 *)&data[i], (k >> 16) ^ (65535 & ~s->ver));*/

		if (signalState)
			wzcrypt_decrypt(7, k, &data[i + 4u], pkt);
		else
			wzcrypt_decrypt(j, k, &data[i + 4u], pkt);
		packet_handle(s, pkt, j);
		k = wzcrypt_nextiv(k);
	}

	s->recv_iv = k;
	return i;
}

void packet_handler_sub(session_t *s, unsigned char *data, unsigned int size)
{
	char buf[128];
	unsigned int i;
	static unsigned int miniroomCount;
	if (복호화Sel)
	{
		if (signalState && size == 11)
		{
			if (sendState)
				SendPacket(packet.data, packet.size);
			else
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
			wsprintf(buf, "일상 반응횟수: %d회", ++miniroomCount);
			SendMessage(g_hList2, LB_DELETESTRING, 0, 0);
			SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)buf);
			PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
			SetForegroundWindow(g_hMP);
			while (!stop)
			{
				if (Global_ImageSearch(상점열기) != -1)
				{
					if (SendMessage(g_hCheck, BM_GETCHECK, BST_CHECKED, 0))
						PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
					signalState = 0;
					상점개설(shopSel);
					SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON3, 0);
					return;
				}
				else if (Global_ImageSearch(확인) != -1)
				{
					PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
					if (SendMessage(g_hCheck, BM_GETCHECK, BST_CHECKED, 0))
						PlaySound(MAKEINTRESOURCE(IDR_WAVE2), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
					signalState = 0;
					if (!sendState)
					{
						if (shopSel == 0)
							일상재대기();
						else
							고상재대기();
					}
					signalState = 1;
					return;
				}
				Sleep(50);
			}
		}
	}
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

	ih = (iphdr_t *)(pkt_data + 14u);
	th = (tcphdr_t *)(pkt_data + 14u + (ih->ihl << 2));

	if (th->fin != 0u || th->rst != 0u)
	{
		if ((s = session[th->dport]) != NULL)
		{
			session[th->dport] = NULL;
			HeapFree(GetProcessHeap(), 0u, s);
			//dbg("%p %s\n", s, "인식종료");
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
			//dbg("%p %s\n", s, "인식시작");
			s->auth = 0u;
			s->size = 0u;
		}
		else if ((s = session[th->dport]) == NULL)
			return;
		if (i = ih->tot_len, (i = (((i & 0xFF) << 8) | (i >> 8)) - (j = (ih->ihl << 2) + (th->doff << 2))) != 0)
			packet_handler_sub(s, (char *)pkt_data + j + 14u, i);
	}
}

unsigned int NpfFindAllDevices(void)
{
	unsigned int i, size;
	char *name, *desc;

	memset(npf_devicelist, 0, sizeof(npf_devicelist));

	if ((size = sizeof(npf_devicelist), PacketGetAdapterNames(npf_devicelist, &size) == FALSE) || size == 0)
		return 0;

	for (name = npf_devicelist, desc = &npf_devicelist[2]; desc[-2] || desc[-1]; ++desc);

	for (i = 0; *name; ++i) {
		name += strlen(name) + 1u;
		desc += strlen(desc) + 1u;
	}

	strcpy(npf_device, npf_devicelist);
	return i;
}

unsigned int NpfGetDeviceCount(void)
{
	unsigned int i;
	char *name, *desc;

	for (name = npf_devicelist, desc = &npf_devicelist[2]; desc[-2] || desc[-1]; ++desc);

	for (i = 0; *name; ++i) {
		name += strlen(name) + 1u;
		desc += strlen(desc) + 1u;
	}

	return i;
}

unsigned int __stdcall NpfGetDeviceInfo(unsigned int index, char *device_name, char *device_desc)
{
	unsigned int i;
	char *name, *desc;

	for (name = npf_devicelist, desc = &npf_devicelist[2]; desc[-2] || desc[-1]; ++desc);

	for (i = 1; *name; ++i) {
		if (i == index) {
			if (device_name)
				strcpy(device_name, name);
			if (device_desc)
				strcpy(device_desc, desc);
			return i;
		}
		name += strlen(name) + 1u;
		desc += strlen(desc) + 1u;
	}

	return 0;
}

unsigned int __stdcall NpfSetDevice(const char *device_name)
{
	unsigned int i;
	char *name;

	for (i = 1, name = npf_devicelist; *name; ++i) {
		if (strcmp(name, device_name) == 0) {
			strcpy(npf_device, name);
			return i;
		}
		name += strlen(name) + 1u;
	}

	return 0;
}

unsigned int NpfStart(void)
{
	static struct bpf_program bpf_code =
	{
		17, (struct bpf_insn *) /* tcp && src portrange 8585-8589 */
		"\x28\x00\x00\x00\x0C\x00\x00\x00\x15\x00\x00\x04\xDD\x86\x00\x00"
		"\x30\x00\x00\x00\x14\x00\x00\x00\x15\x00\x00\x0C\x06\x00\x00\x00"
		"\x28\x00\x00\x00\x36\x00\x00\x00\x35\x00\x08\x0A\x89\x21\x00\x00"
		"\x15\x00\x00\x09\x00\x08\x00\x00\x30\x00\x00\x00\x17\x00\x00\x00"
		"\x15\x00\x00\x07\x06\x00\x00\x00\x28\x00\x00\x00\x14\x00\x00\x00"
		"\x45\x00\x05\x00\xFF\x1F\x00\x00\xB1\x00\x00\x00\x0E\x00\x00\x00"
		"\x48\x00\x00\x00\x0E\x00\x00\x00\x35\x00\x00\x02\x89\x21\x00\x00"
		"\x25\x00\x01\x00\x8D\x21\x00\x00\x06\x00\x00\x00\x00\x00\x01\x00"
		"\x06\x00\x00\x00\x00\x00\x00\x00"
	};
	ADAPTER *adapter;

	if ((adapter = PacketOpenAdapter(npf_device)) != NULL)
	{
		if (adapter->Flags == INFO_FLAG_NDIS_ADAPTER)
		if (PacketSetBpf(adapter, &bpf_code) != FALSE)
		if (PacketSetBuff(adapter, 1048576u) != FALSE)
		if (PacketSetHwFilter(adapter, 1/*NDIS_PACKET_TYPE_DIRECTED*/) != FALSE)
		{
			npf_adapter = adapter;
			return 1u;
		}
		PacketCloseAdapter(adapter);
	}

	return 0u;
}

void NpfStop(void)
{
	if (npf_adapter)
	{
		PacketCloseAdapter(npf_adapter);
		npf_adapter = NULL;
	}
}

void NpfCheckMessage(void)
{
	static unsigned int pkt_size;
	static unsigned char pkt_data[65536];
	unsigned int i, j;
	ADAPTER *adapter;

	pkt_size = 0u;

	if ((adapter = npf_adapter) != NULL)
	if (WaitForSingleObject(adapter->ReadEvent, 1) == WAIT_OBJECT_0)
	if (ReadFile(adapter->hFile, pkt_data, sizeof(pkt_data), &pkt_size, NULL))
		for (i = 0, j = pkt_size; i < j; i += (((struct bpf_hdr *)&pkt_data[i])->bh_caplen + ((struct bpf_hdr *)&pkt_data[i])->bh_hdrlen + 3u) & ~3)
			packet_handler(NULL, (struct pcap_pkthdr *)&pkt_data[i], &pkt_data[i + ((struct bpf_hdr *)&pkt_data[i])->bh_hdrlen]);
}


int 블럭체크(unsigned int xy)
{
	unsigned int i;
	for (i = 0; i < block_count; ++i)
	if (block_xy[i] == xy)
		return 1;
	return 0;
}

void 블럭초기화(void)
{
	block_count = 0;
}

void 블럭추가(unsigned int xy)
{
	if (블럭체크(xy) == 0)
	if (block_count != MAX_BLOCK_XY)
		block_xy[++block_count] = xy;
}

void 블럭삭제(unsigned int xy)
{
	unsigned int i;
	for (i = 0; i < block_count; ++i)
	if (block_xy[i] == xy) {
		memcpy(&block_xy[i], &block_xy[i + 1], sizeof(int)* (block_count - i - 1u));
		break;
	}
}

unsigned int ImageSearch2sub(image_t *from, image_t *target)
{
	unsigned int x, y, z, n;

	n = 0;

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
					if (--z == 0 && n != MAX_SEARCH_XY)
						image_xy[n++] = ((y & 65535) << 16) | (x & 65535);
					if (memcmp(&target->data[target->xs * z], &from->data[from->xs * (y + z) + x], target->xs * 4u))
						break;
				}
		}
	}

	return n;
}




unsigned int Global_ImageSearch2(image_t *image) /* 반환값: 찾은 좌표 개수 */
{
	unsigned int i = 0;
	image_t *screen;
	if ((screen = get_screen(g_hMP)) != NULL)
	{
		i = ImageSearch2sub(screen, image);
		HeapFree(GetProcessHeap(), 0, screen);
	}

	return i;
}

void CALLBACK 와리(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	unsigned int i;

	if (!wariState)
		return;
	signalState = 0;

	SetCursorPos(30, 30);
	//SetCursorPos(i & 65535, (i >> 16));
	PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
	SetForegroundWindow(g_hMP);
	Sleep(300);
	i = Global_ImageSearch(취소);
	if (i != -1)
	{
		MouseClick(i & 65535, (i >> 16), 1);
		Sleep(50);
	}
	else
	{
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
		while (Global_ImageSearch(확인) == -1)
		{
			Sleep(50);
			if (stop)
				return;
		}
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);

	}
	while (wariState)
	{
		if (stop)
			return;
		SetCursorPos(i & 65535, (i >> 16));
		Sleep(150);
		i = Global_ImageSearch2(상점);
		while (i > 0)
		{
			--i;
			if (블럭체크(image_xy[i]) == 0)
				break;
		}
		if (i == 0)
			break;
		i = image_xy[i];
		MouseClick(i & 65535, (i >> 16), 2);

		while ((i = Global_ImageSearch(상점나가기)) == -1)
		{
			Sleep(50);
			if (stop)
				return;
		}
		if (i != -1)
		{
			MouseClick(i & 65535, (i >> 16), 1);
			Sleep(100);
			break;
		}
		블럭추가(i);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
	}
	Sleep(300);
	if (!sendState)
	{
		if (shopSel == 0)
			일상재대기();
		else
			고상재대기();
	}
	signalState = 1;
}

INT_PTR CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int count, i;
	char name[128], desc[128];

	switch (iMessage)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"일반상점");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"고용상점");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, shopSel, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)"OFF");
		SendDlgItemMessage(hDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)"ON");
		SendDlgItemMessage(hDlg, IDC_COMBO2, CB_SETCURSEL, 갈바Sel, 0);
		count = NpfFindAllDevices();
		for (i = 1; i <= count; ++i)
		{
			NpfGetDeviceInfo(i, name, desc);
			SendDlgItemMessage(hDlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)desc);
		}
		SendDlgItemMessage(hDlg, IDC_COMBO3, CB_SETCURSEL, adapterSel, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_ADDSTRING, 0, (LPARAM)"ON");
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_ADDSTRING, 0, (LPARAM)"OFF");
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_SETCURSEL, 복호화Sel, 0);
		if (복호화Sel)
			MessageBox(0, 0, 0, 0);
		if (strcmp(shopTitle, "　") == 0)
			SetDlgItemText(hDlg, IDC_EDIT1, "　");
		else
			SetDlgItemText(hDlg, IDC_EDIT1, shopTitle);
		return 0;


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT1)) == 0)
				wsprintf(shopTitle, "　");
			else
				GetDlgItemText(hDlg, IDC_EDIT1, shopTitle, sizeof(shopTitle));
			shopSel = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
			갈바Sel = SendDlgItemMessage(hDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0);
			adapterSel = SendDlgItemMessage(hDlg, IDC_COMBO3, CB_GETCURSEL, 0, 0);
			복호화Sel = SendDlgItemMessage(hDlg, IDC_COMBO4, CB_GETCURSEL, 0, 0);
			NpfGetDeviceInfo(adapterSel + 1, name, desc);
			NpfSetDevice(name);
			NpfStart();
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return 0;
	}
	return 0;
}



DWORD WINAPI 일고랜작(LPVOID *arg)
{
	int i;

	g_hMP = FindWindow("MapleStoryClass", NULL);
	PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
	SetForegroundWindow(g_hMP);
	HeapFree(GetProcessHeap(), 0, get_screen(g_hMP));
	Sleep(200);
	i = Global_ImageSearch(상점닫기);
	if (i == -1)
	{
		MessageBox(0, "상점닫기 버튼 보이지 않음!", "알림", MB_OK);
		return 0;
	}
	Sleep(50);
	switch (해상도)
	{
	case 800:
		MouseClick(773, 39, 1);
		break;
	case 1024:
		MouseClick(999, 36, 1);
		break;
	case 1366:
		MouseClick(1341, 38, 1);
		break;
	}
	Sleep(50);
	switch (해상도)
	{
	case 800:
		MouseMove(691, 64);
		break;
	case 1024:
		MouseMove(915, 65);
		break;
	case 1366:
		MouseMove(1256, 64);
		break;
	}
	Sleep(100);
	packet_disallow();
	Sleep(200);
	for (int i = 0; i < 1000; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
	Sleep(500);
	MouseClick(i & 65535, (i >> 16), 1);
	Sleep(300);
	switch (해상도)
	{
	case 800:
		MouseClick(656, 64, 2);
		break;
	case 1024:
		MouseClick(879, 62, 2);
		break;
	case 1366:
		MouseClick(1219, 61, 2);
		break;
	}
	Sleep(300);
	PostMessage(g_hMP, WM_CHAR, '/', 0);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
	Sleep(500);
	packet_allow();
	while (Global_ImageSearch(상점열기) == -1)
	{
		Sleep(50);
		if (stop)
			return 0;
	}
	Sleep(200);
	packet_disallow();
	Sleep(500);
	PostMessage(g_hMP, WM_KEYDOWN, VK_ESCAPE, 0);
	Sleep(200);
	if (Global_ImageSearch(취소) != -1)
	{
		switch (해상도)
		{
		case 800:
			MouseClick(311, 328, 2);
			break;
		case 1024:
			MouseClick(428, 411, 2);
			break;
		case 1366:
			MouseClick(607, 412, 2);
			break;
		}
		Sleep(100);
		keybd_event('1', MapVirtualKey(2, 0), 0, 0);
		keybd_event('1', MapVirtualKey(2, 0), KEYEVENTF_KEYUP, 0);

		Sleep(100);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
		Sleep(50);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 1835009);
		Sleep(500);
		packet_allow();
		while (Global_ImageSearch(상점열기) == -1)
		{
			Sleep(50);
			if (stop)
				return 0;
		}
		상점개설(1);
		signalState = 0;
	}
	else
	{
		switch (해상도)
		{
		case 800:
			MouseClick(656, 64, 2);
			break;
		case 1024:
			MouseClick(879, 62, 2);
			break;
		case 1366:
			MouseClick(1219, 61, 2);
			break;
		}
		while (Global_ImageSearch(상점열기) == -1)
		{
			Sleep(50);
			if (stop)
				return 0;
		}
		일고랜작(0);
	}
	return 0;
}

INT_PTR CALLBACK AdditionDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			packet_allow();
			break;
		case IDC_BUTTON2:
			packet_disallow();
			break;
		case IDC_BUTTON3:
			if (MessageBox(0, "영정 먹을 확률 있음.계속하려면 확인", "알림", MB_ICONSTOP | MB_OKCANCEL) == IDOK)
				CreateThread(0, 0, 일고랜작, 0, 0, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return 0;
	}
	return 0;
	
}

void PrepareShopPacket()
{
	static const char *shopName = "　";
	unsigned int i;

	packet.size = (i = strlen(shopName)) + 13u;
	*(unsigned __int16*)&packet.data[0] = (unsigned __int16)S_MINIROOM;
	*(unsigned __int8*)&packet.data[2] = (unsigned __int8)S_MINIROOM_CREATE;
	*(unsigned __int8*)&packet.data[3] = 5;
	*(unsigned __int16*)&packet.data[4] = (unsigned __int16)i;
	memcpy(&packet.data[6], shopName, i);
	*(unsigned __int8*)&packet.data[i + 6u] = 0;
	*(unsigned __int16*)&packet.data[i + 7u] = 1;
	*(unsigned __int32*)&packet.data[i + 9u] = 5140000u;

}


void ReadINI()
{
	char INIPath[128];

	GetModuleFileName(g_hInst, INIPath, sizeof(INIPath));

	INIPath[strlen(INIPath) - 3] = 'i';
	INIPath[strlen(INIPath) - 2] = 'n';
	INIPath[strlen(INIPath) - 1] = 'i';

	if ((S_MINIROOM = GetPrivateProfileInt("PACKET", "S_MINIROOM", 0, INIPath)) && (S_MINIROOM_CREATE = GetPrivateProfileInt("PACKET", "S_MINIROOM_CREATE", 0, INIPath)))
		sendState = 1;
	if (!(SERVERINFO = GetPrivateProfileInt("PACKET","SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SERVERINFO", "", INIPath);
	if (!(USERINFO = GetPrivateProfileInt("PACKET", "USERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "USERINFO", "", INIPath);
	if (!(USER_MINIROOM_BALLON = GetPrivateProfileInt("PACKET", "USER_MINIROOM_BALLON", 0, INIPath)))
		WritePrivateProfileString("PACKET", "USER_MINIROOM_BALLON", "", INIPath);
	if (!(EMPLOYEE_LEAVE = GetPrivateProfileInt("PACKET", "EMPLOYEE_LEAVE", 0, INIPath)))
		WritePrivateProfileString("PACKET", "EMPLOYEE_LEAVE", "", INIPath);
	if (!(SHOP_INFO = GetPrivateProfileInt("PACKET", "SHOP_INFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SHOP_INFO", "", INIPath);
}


INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int i;

	switch (iMessage)
	{
	case WM_HOTKEY:
		if (wParam == 10)
		{
			signalState = 0;
			stop = 0;
		}
		else if (wParam == 11)
		{
			BlockInput(0);
			signalState = 1;
			stop = 1;
		}
		SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON3, 0);
		break;
	/*case WM_USER + 3:
		signalState = 0;
		일상재대기();
		signalState = 1;
		break;
	case WM_USER + 4:
		signalState = 0;
		고상재대기();
		signalState = 1;
		break;
	case WM_USER + 5:
		signalState = 0;
		상점개설(shopSel);
		SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON3, 0);
		return 0;*/
	case WM_USER + 5:
		상점개설(shopSel);
		return 0;
	case WM_USER + 6:
		PostMessage(g_hMP, WM_MOUSEMOVE, 0, MAKELONG(1217, 37));
		PostMessage(g_hMP, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(1217, 37));
		PostMessage(g_hMP, WM_LBUTTONDOWN, 0, MAKELONG(1217, 37));
		Sleep(100);
		PostMessage(g_hMP, WM_MOUSEMOVE, 0, MAKELONG(1226, 65));
		PostMessage(g_hMP, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(1226, 65));
		PostMessage(g_hMP, WM_LBUTTONDOWN, 0, MAKELONG(1226, 65));
		Sleep(100);
		PostMessage(g_hMP, WM_MOUSEMOVE, 0, MAKELONG(610, 385));
		PostMessage(g_hMP, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(610, 385));
		PostMessage(g_hMP, WM_LBUTTONDOWN, 0, MAKELONG(610, 385));
		Sleep(100);
		for (i = 0; i < 10; ++i)
			PostMessage(g_hMP, WM_CHAR, '9', 0);
		Sleep(100);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		SendMessage(g_hMP, WM_USER + 3, 0, 0);

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			userId = 0;
			SendMessage(g_hList1, LB_DELETESTRING, 0, 0);
			SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM)"캐릭터 고유 ID: 0");
			break;
		case IDC_BUTTON2:
			MessageBox(g_hWnd, "신호를 처음 켰으면 캐시샵을 갔다 온 뒤\n현재 채널이 뜨는 걸 확인하셈 ", "알림", MB_ICONINFORMATION);
			MessageBox(g_hWnd, "닫히면 반응해야 할 상점이 일상이라면,\n갈바 방어를 ON으로 하고 캐릭터 고유 ID를\n초기화 하고 대기탈 일상에 들어가서\n판매자정보 버튼을 누르고 대기를 시작하면 됨", "알림", MB_ICONINFORMATION);
			MessageBox(g_hWnd, "닫히면 반응해야 할 상점이 고상이라면,\n갈바 방어를 ON으로 하고 캐릭터 고유 ID를\n초기화 하고 대기를 시작하면 됨", "알림", MB_ICONINFORMATION);
			MessageBox(g_hWnd, "갈바 방어를 OFF로 했다면 반응할 상점이 뭐든\n캐릭터 고유 ID는 신경 쓸 필요 없음", "알림", MB_ICONINFORMATION);
			MessageBox(g_hWnd, "복호화를 OFF로 하면 독 갈바에 반응하고 고상펑을 잡지 못함\n대신 속도는 살짝 빠름", "알림", MB_ICONINFORMATION);
			break;
		case IDC_BUTTON3:
			if (!startup)
			{
				MessageBox(g_hWnd, "캐시샵을 갔다 와서 현재 채널이\n제대로 표시된 후 대기를 시작해주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}
			if (!signalState)
			{
				stop = 0;
				g_hMP = FindWindow("MapleStoryClass", NULL);
				HeapFree(GetProcessHeap(), 0, get_screen(g_hMP));
				PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
				SetForegroundWindow(g_hMP);
				SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
				if (g_hMP == NULL)
				{
					MessageBox(0, "메이플 찾지 못함!", 0, 0);
					return 0;
				}
				SetDlgItemText(hDlg, IDC_BUTTON3, "인식 중지[F11]");
				Sleep(100);
				if (!sendState)
				{
					if (shopSel == 0)
						일상재대기();
					else
						고상재대기();
				}
				signalState = 1;
				wariState = 1;
				SetTimer(g_hWnd, 1, 39 * 60 * 1000, 와리);
				
			}
			else
			{
				stop = 1;
				SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				SetDlgItemText(hDlg, IDC_BUTTON3, "인식 시작[F10]");
				i = Global_ImageSearch(취소);
				if (i != -1)
					MouseClick(i & 65535, (i >> 16), 1);
				signalState = 0;
				wariState = 0;
				KillTimer(g_hWnd, 1);
			}
			break;
		case IDC_BUTTON4:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		case IDC_BUTTON5:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG3), 0, AdditionDlgProc);
			break;
			
		}
		break;
	case WM_INITDIALOG:
		g_hMP = FindWindow("MapleStoryClass", NULL);
		g_hWnd = hDlg;
		g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
		g_hList2 = GetDlgItem(hDlg, IDC_LIST2);
		g_hCheck = GetDlgItem(hDlg, IDC_CHECKBOX1);
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM)"캐릭터 고유 ID: 0");
		SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"현재 채널: NULL");
		SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)"일상 반응횟수: 0회");
		SendMessage(g_hList2, LB_INSERTSTRING, 1, (LPARAM)"고상 반응횟수: 0회");
		SendMessage(g_hCheck, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(g_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1)));
		SystemParametersInfo(SPI_SETKEYBOARDDELAY, 0, 0, SPIF_UPDATEINIFILE);
		SystemParametersInfo(SPI_SETKEYBOARDSPEED, 31, 0, SPIF_UPDATEINIFILE);
		상점 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1)));
		상점나가기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2)));
		취소 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3)));
		상점열기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP4)));
		상점닫기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP5)));
		확인 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP6)));
		RegisterHotKey(g_hWnd, 10, 0, VK_F10);
		RegisterHotKey(g_hWnd, 11, 0, VK_F11);
		packet_init();
		ReadINI();
		PrepareShopPacket();
		NpfFindAllDevices();
		if (NpfStart())
			CreateThread(0, 0, NpfLoop, 0, 0, 0);
		else
		{
			MessageBox(0, "NpfStart() 실패", "알림", MB_ICONERROR);
			ExitProcess(1);
		}
		return 0;
	case WM_CLOSE:
		DeleteObject(상점);
		DeleteObject(상점나가기);
		DeleteObject(취소);
		DeleteObject(상점열기);
		DeleteObject(상점닫기);
		DeleteObject(확인);
		packet_term();
		UnregisterHotKey(g_hWnd, 10);
		UnregisterHotKey(g_hWnd, 11);
		wariState = 0;
		signalState = 0;
		KillTimer(g_hWnd, 1);
		WSACleanup();
		PostQuitMessage(0);
		return 0;
	}
	return 0;
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

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
	TerminateProcess(GetCurrentProcess(), 0);
	return EXCEPTION_EXECUTE_HANDLER;
}

void vmemcpy(void *addr, const void *data, unsigned int size)
{
	DWORD protect;

	VIRTUALIZER1_START
	{
		if (VirtualProtect(addr, size, PAGE_READWRITE, &protect)) {
			memcpy(addr, data, size);
			VirtualProtect(addr, size, protect, &protect);
			FlushInstructionCache(GetCurrentProcess(), addr, size);
		}
	}
		VIRTUALIZER_END
}

void getuuid(char *data)
{
	VIRTUALIZER3_START
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
	VIRTUALIZER_END
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	int size;
	char message[1024];
	char data[2048];
	char username[128];
	SOCKET hSock;
	DWORD username_size = sizeof(username) - 1;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);

	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자권한으로 다시 켜주세요.", "메세지박스", MB_OK);
		return 0;
	}
	if (!FileNameCheck("RF.exe"))
		return 0;
	if (WSAStartup(MAKEWORD(2, 2), (LPWSADATA)message))
	{
		MessageBox(0, "WinSock 초기화 오류", 0, 0);
		return 0;
	}
	if ((CreateMutex(NULL, FALSE, "RFMultiAuth")) == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		SetForegroundWindow(FindWindow(NULL, "Rhythm Function"));
		return 0;
	}
	VIRTUALIZER1_START
	{
		hSock = ConnectToServer();
		if (hSock == INVALID_SOCKET)
			MessageBox(0, "서버 닫힘!", 0, 0);
		else
		{
			getuuid(data);
			GetUserName(username, &username_size);
			size = wsprintf(message, "RC %s %s", data, username);
			RFCrypt(size, message, message);
			send(hSock, message, size, 0);

			size = recv(hSock, message, size, 0);
			RFCrypt(size, message, message);
			closesocket(hSock);
			if (size != SOCKET_ERROR)
			{
				message[size] = '\0';
				if (message[0] == 'O')
				{
					CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
					MessageLoop();
				}
				else if (message[0] == 'X')
					MessageBox(0, "하드인증실패!", "알림", MB_ICONSTOP);
				else if (message[0] == 'N')
					MessageBox(0, "기간 만료!", "알림", MB_ICONINFORMATION);
			}
			else
				MessageBox(0, "recv() 에러!", 0, 0);
		}
	} 
	VIRTUALIZER_END
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
