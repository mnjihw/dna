#include "common.h"

#define SERVERINFO 0x14F
#define SHOP_INFO 0x3FA
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#define CARD_WIDTH 45 //30
#define CARD_HEIGHT 60 //40
#define CARD_DISTANCE_W 56 //38
#define CARD_DISTANCE_H 67 //44

const unsigned char *aeskey = "\xCD\x00\x00\x00\x23\x00\x00\x00\x87\x00\x00\x00\x5A\x00\x00\x00\x99\x00\x00\x00\x60\x00\x00\x00\xAD\x00\x00\x00\x67\x00\x00\x00";
extern aes256_context ctx;
HWND g_hWnd, g_hMP, g_hList, g_hTransWnd;
unsigned int breakloop, adapter_sel, 해상도;
unsigned int card[30], game_sel, operating;
COLORREF color[15];

void dbg(const char *fmt, ...)
{
	char msg[1024];

	OutputDebugString((wvsprintf(msg, fmt, (va_list) ((unsigned int) &fmt + sizeof(void *))), msg));
}

void packet_handle(unsigned char *data, unsigned int size)
{
	char buf[128];
	unsigned int i, j;

	switch (*(unsigned short*) data)
	{
	case SERVERINFO:
		if (data[2] > 19)
			return;
		else if (data[2] == 0)
			i = 1;
		else if (data[2] == 1)
			i = 20;
		else
			i = data[2];
		if (i == 20)
			wsprintf(buf, "현재 채널: %u세", i);
		else
			wsprintf(buf, "현재 채널: %u채널", i);
		SendMessage(g_hList, LB_DELETESTRING, 0, 0);
		SendMessage(g_hList, LB_INSERTSTRING, 0, (LPARAM) buf);
		g_hMP = FindWindow("MapleStoryClass", NULL);
		해상도 = GetMapleResolution(g_hMP);

		break;
	case SHOP_INFO:
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
				card[i] = *(unsigned int*) &data[i * 4 + 5];
			operating = 1;
			InvalidateRect(g_hTransWnd, NULL, 0);
		}
		break;
	}
}

unsigned int atoi32(const char *string)
{
	unsigned int value = 0;

	while (*string >= '0' && *string <= '9')
		value = value * 10u + (*string++ - '0');

	return value;
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
		s->ver = atoi32((data[j + 6u] = 0u, (char *) &data[6]));
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
			packet_handler_sub(s, (unsigned char *) pkt_data + j + 14u, i);
	}
}


VOID MessageLoop()
{
	MSG Message;

	while (GetMessage(&Message, 0, 0, 0) > 0)
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

	if (VirtualProtect(addr, size, PAGE_READWRITE, &protect))
	{
		memcpy(addr, data, size);
		VirtualProtect(addr, size, protect, &protect);
		FlushInstructionCache(GetCurrentProcess(), addr, size);
	}
	
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

DWORD WINAPI NpfLoop(LPVOID arg)
{
	while (!breakloop)
		NpfCheckMessage();
	NpfStop();

	breakloop = 0;
	return 0;
}


void MouseMove(unsigned int x, unsigned int y)
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
	Sleep(10);
	for (unsigned int i = 0; i < k; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}


void PaintRect(HDC hdc, RECT *rect, COLORREF colour)
{
	COLORREF oldcr = SetBkColor(hdc, colour);
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, rect, "", 0, 0);
	SetBkColor(hdc, oldcr);
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
			//color[i] = RGB(gen_rand() % 256, gen_rand() % 256, gen_rand() % 256);
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
		if (operating)
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
		//DestroyWindow(hDlg);
		PostQuitMessage(0);
		break;
	}
	return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char name[128], desc[128];
	RECT rect;

	switch (iMessage)
	{
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		g_hList = GetDlgItem(hDlg, IDC_LIST1);
		g_hMP = FindWindow("MapleStoryClass", NULL);
		if (g_hMP == NULL)
			MessageBox(0, "메이플 찾지 못함", 0, 0);
		SendMessage(g_hList, LB_INSERTSTRING, 0, (LPARAM)"현재 채널: NULL");
		init_rand(GetCurrentProcessId() ^ 0xBADF00Du);
		NpfFindAllDevices();
		NpfGetDeviceInfo(adapter_sel + 1, name, desc);
		NpfSetDevice(name);
		if (NpfStart())
			CloseHandle(CreateThread(0, 0, NpfLoop, 0, 0, 0));
		else
		{
			MessageBox(GetDesktopWindow(), "NpfStart() 실패", "알림", MB_ICONERROR);
			ExitProcess(1);
		}


		GetWindowRect(g_hMP, &rect);
		
		해상도 = GetMapleResolution(g_hMP);
		해상도 = 910;//여기

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

		g_hTransWnd = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG3), g_hWnd, TransparentDlgProc);
		break;
	case WM_CLOSE:
		breakloop = 1;
		while (breakloop)
			SwitchToThread();
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		}
		break;
	}

	return 0;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);

	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(g_hWnd, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
		return 0;
	}

	if (FindWindow("#32770", "리듬 같은그림찾기 도우미"))
	{
		SetForegroundWindow(FindWindow("#32770", "리듬 같은그림찾기 도우미"));
		return 0;
	}

	aes256_init(&ctx, aeskey);
	CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();

	return 0;

}