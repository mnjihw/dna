#include "common.h"

HBITMAP g_hBitmap1, g_hBitmap2;
HWND g_hWnd, g_hWnd2, g_hWnd3, g_hMP;
HINSTANCE g_hInst;
unsigned int cx, cy;
HANDLE g_hEvent;
HBITMAP g_hMemBitmap;


void dbg(const char *fmt, ...)
{
	char buf[1024];
	wvsprintf(buf, fmt, (char*)&fmt + sizeof(void*));
	OutputDebugString(buf);
}

BOOL CALLBACK WebDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		if (OleInitialize(NULL) != S_OK) //쓰레드 문제였다 씨발 더 짧은 코드 html.c로 쓰고 쓰레드 생각해서코딩하자 시발 시발 시발 좆같네 ㅋ 브금플도 수정하고
			return FALSE;
		if (EmbedBrowserObject(hDlg))
			break;
		DisplayHTMLPage(hDlg, "https://www.naver.com/");
		break;
	case WM_CLOSE:
		UnEmbedBrowserObject(hDlg);
		DestroyWindow(hDlg);
		break;
	default:
		return FALSE;
	}
	return TRUE;

}


DWORD WINAPI func(LPVOID arg)
{
	RECT rect;
	HANDLE hProcess;
	PROCESSENTRY32 pe32 = { 0, };
	BOOL bRun = TRUE;
	static DWORD counter;
	POINT pt;

	g_hMP = FindWindow("MapleStoryClass", NULL);

	while (bRun)
	{
		if (!IsWindow(g_hMP))
		{
			bRun = FALSE;

			hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hProcess != INVALID_HANDLE_VALUE)
			{
				pe32.dwSize = sizeof(PROCESSENTRY32);
				if (Process32First(hProcess, &pe32))
				{
					do
					{
						if (!strcmp(pe32.szExeFile, "MapleStory.exe"))
						{
							bRun = TRUE;
							continue;
						}
					} while (Process32Next(hProcess, &pe32));
				}
				CloseHandle(hProcess);
			}
			else
				break;
			
			g_hMP = FindWindow("MapleStoryClass", NULL);
		}
		GetClientRect(g_hMP, &rect);
		if (((rect.right - rect.left) != cx) || ((rect.bottom - rect.top) != cy));
			SetWindowPos(g_hMP, 0, 0, 0, cx, cy, SWP_NOMOVE);
		Sleep(300);
		counter += 300;
		if (counter >= 1000 * 60 * 10)
		{
			counter = 0;
			GetCursorPos(&pt);
			SetCursorPos(0, 0);
			Sleep(30);
			SetCursorPos(pt.x, pt.y);
		}
		
	}
	PostMessage(g_hWnd, WM_QUIT, 0, 0);
	return 0;
}


void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;


	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;


	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}



//3번 모니터 1680 x 1050
//2번 모니터 1280 x 1024

INT_PTR CALLBACK SecretDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, hMemDC;
	PAINTSTRUCT ps;
	static BITMAP bit, bit2;
	HBITMAP hOldBitmap;
	static int x = 1262, y = 20, clicked_x = -1, clicked_y = -1, text_x = 1560, text_y = 1000;
	RECT rect;
	POINT pt;
	SYSTEMTIME st;
	static char buf[128];
	HFONT hFont, OldFont;

	switch (iMessage)
	{
	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON && clicked_x != -1)
		{
			x += LOWORD(lParam) - clicked_x;
			y += HIWORD(lParam) - clicked_y;
			clicked_x = LOWORD(lParam);
			clicked_y = HIWORD(lParam);

			InvalidateRect(hDlg, NULL, FALSE);
		}
		break;
	case WM_LBUTTONDOWN:
		SetRect(&rect, x, y, x + 400, y + 30);
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		if (PtInRect(&rect, pt))
		{
			clicked_x = pt.x;
			clicked_y = pt.y;
		}
		else
		{
			clicked_x = -1;
			clicked_y = -1;
		}
		break;

	case WM_KEYDOWN:
		if (wParam == VK_UP)--y;
		else if (wParam == VK_DOWN)++y;
		else if (wParam == VK_RIGHT) ++x;
		else if (wParam == VK_LEFT) --x;
		InvalidateRect(hDlg, NULL, FALSE);
		break;
	case WM_TIMER:
		GetLocalTime(&st);
		wsprintf(buf, "%s %u:%02u %02u.%02u초", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		InvalidateRect(hDlg, NULL, FALSE);
		break;
	case WM_INITDIALOG:
		g_hWnd2 = hDlg;
		g_hBitmap1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(GetSystemMetrics(SM_CXSCREEN) == 1680 ? IDB_BITMAP2 : IDB_BITMAP1));
		g_hBitmap2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
		GetObject(g_hBitmap1, sizeof(BITMAP), &bit);
		GetObject(g_hBitmap2, sizeof(BITMAP), &bit2);
		SetTimer(hDlg, 1, 10, 0);
		SetWindowPos(hDlg, 0, 0, 0, bit.bmWidth,bit.bmHeight, 0);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		if(!g_hMemBitmap)
			g_hMemBitmap = CreateCompatibleBitmap(hdc, bit.bmWidth, bit.bmHeight);
		hMemDC = CreateCompatibleDC(hdc);

		hOldBitmap = (HBITMAP)SelectObject(hMemDC, g_hMemBitmap);
		DrawBitmap(hMemDC, 0, 0, g_hBitmap1);
		DrawBitmap(hMemDC, x, y, g_hBitmap2);
		
		hFont = CreateFont(15, 0, 0, 0, 550, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, "맑은 고딕");
		OldFont = (HFONT)SelectObject(hMemDC, hFont);
		SetTextColor(hMemDC, RGB(255, 255, 255));
		SetBkColor(hMemDC, RGB(0, 0, 0));
		SetBkMode(hMemDC, TRANSPARENT);
		SetRect(&rect, text_x, text_y, text_x + 60, text_y + 30);
		DrawText(hMemDC, buf, strlen(buf), &rect, DT_WORDBREAK | DT_CENTER);
		SelectObject(hMemDC, OldFont);
		DeleteObject(hFont);


		BitBlt(hdc, 0, 0, bit.bmWidth, bit.bmHeight, hMemDC, 0, 0, SRCCOPY);


		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
		
		EndPaint(hDlg, &ps);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

DWORD WINAPI DlgThread(LPVOID arg)
{
	CreateDialog(g_hInst, MAKEINTRESOURCE(*(int*)arg), g_hWnd, *(int*)arg == IDD_DIALOG2 ? SecretDlgProc : WebDlgProc);
	SetEvent(g_hEvent);
	MessageLoop();

	return 0;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HWND hMP;
	static int id;

	switch (iMessage)
	{
	case WM_INITDIALOG:
		g_hEvent = CreateEvent(0, FALSE, FALSE, 0);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1)));
		cx = 1150;
		cy = 700;
		SetDlgItemInt(hDlg, IDC_EDIT1, cx, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT2, cy, FALSE);
		//RegisterHotKey(hDlg, 1, MOD_NOREPEAT, VK_F1);
		//RegisterHotKey(hDlg, 2, MOD_NOREPEAT, VK_F2);
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, 0, 0, 0)); 
		//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DlgThread, (id = IDD_DIALOG2, &id), 0, 0));
		//WaitForSingleObject(g_hEvent, INFINITE); 
		//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DlgThread, (id = IDD_DIALOG3, &id), 0, 0));
		//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DlgThread, (id = IDD_DIALOG4, &id), 0, 0));
		break;
	case WM_HOTKEY:
		if (!IsWindow(g_hMP))
			g_hMP = FindWindow("MapleStoryClass", NULL);
		switch (wParam)
		{
		case 1:
			ShowWindow(g_hMP, SW_HIDE);
			ShowWindow(g_hWnd, SW_HIDE);
			ShowWindow(g_hWnd2, SW_SHOW);
			ShowWindow(g_hWnd3, SW_SHOW);
			break;
		case 2:
			ShowWindow(g_hMP, SW_SHOW);
			ShowWindow(g_hWnd, SW_SHOW);
			ShowWindow(g_hWnd2, SW_HIDE);
			ShowWindow(g_hWnd3, SW_HIDE);
			SetForegroundWindow(g_hMP);
			break;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			cx = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
			cy = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);
			break;
		case IDC_BUTTON2:
			hMP = FindWindow("MapleStoryClass", NULL);
			if (IsWindow(hMP))
			{
				PostMessage(hMP, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				PostMessage(hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
			}
			break;
		case IDC_BUTTON3:
			hMP = FindWindow("MapleStoryClass", NULL);
			if (IsWindow(hMP));
				SetWindowPos(hMP, 0, 20, 20, 0, 0, SWP_NOSIZE);
			break;
		}
		break;
	case WM_CLOSE:
		DeleteObject(g_hBitmap1);
		DeleteObject(g_hBitmap2);
		DeleteObject(g_hMemBitmap);
		CloseHandle(g_hEvent);
		DestroyWindow(hDlg);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

VOID MessageLoop()
{ 
	MSG Message;

	while (GetMessage(&Message, 0, 0, 0) > 0)
	{
		if (!IsWindow(g_hWnd) || !IsDialogMessage(g_hWnd, &Message))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HKEY hkey = 0;
	HWND hMP;
	DWORD cbData = 255, dwType;
	char MaplePath[256], buf[256];
	WIN32_FIND_DATA fd;
	HANDLE hFind;

	g_hInst = hInstance;

	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR);
		return 0;
	}

	
	hMP = FindWindow("MapleStoryClass", NULL);
	if (!hMP)
	{

		RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wizet\\MapleStory", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS, NULL, &hkey, NULL);

		if (RegQueryValueEx(hkey, "ExecPath", NULL, &dwType, (LPBYTE)MaplePath, &cbData) != ERROR_SUCCESS)
		{
			RegCloseKey(hkey);
			wsprintf(buf, "%s() %u%s", "RegQueryValueEx", GetLastError(), "\n관리자 권한으로 켜 보시거나 포맷 직후라면\n메이플을 직접 한 번이라도 켠 후에 다시 켜 주세요.");
			MessageBox(0, buf, 0, MB_ICONERROR);
			return 0;
		}
		RegCloseKey(hkey);


		SetCurrentDirectory(MaplePath);

		wsprintf(buf, "*.avi");

		if ((hFind = FindFirstFile(buf, &fd)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				wsprintf(buf, "%s", fd.cFileName);
				DeleteFile(buf);
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		wsprintf(buf, "BlackCipher\\*.log");

		if ((hFind = FindFirstFile(buf, &fd)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				wsprintf(buf, "BlackCipher\\%s", fd.cFileName);
				DeleteFile(buf);
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		wsprintf(buf, "XignCode\\*.log");

		if ((hFind = FindFirstFile(buf, &fd)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				wsprintf(buf, "XignCode\\%s", fd.cFileName);
				DeleteFile(buf);
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		strcat(MaplePath, "\\MapleStory.exe GameLaunching");


		WinExec(MaplePath, SW_SHOW);
	}
	if (OleInitialize(NULL) != S_OK)
		return 0;

    g_hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	MessageLoop();
	
	OleUninitialize();

	return 0;
}


