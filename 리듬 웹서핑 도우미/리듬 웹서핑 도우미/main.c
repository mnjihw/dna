#include "common.h"

HINSTANCE g_hInstance;
unsigned int setting;
HWND target_window, g_hWnd;
unsigned int keycode1, keycode2, keycode3;

void dbg(const char *fmt, ...)
{
	char msg[1024];

	OutputDebugStringA((wvsprintf(msg, fmt, (va_list)((unsigned int)&fmt + sizeof(void *))), msg));

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

DWORD WINAPI func(LPVOID arg)
{
	POINT pt;
	HWND hwnd;
	FLASHWINFO fi = { 0, };

	while (1)
	{
		if (setting && (GetAsyncKeyState(VK_RBUTTON) & 0x8000))
			SendMessage(g_hWnd, WM_USER + 7, 0, 0);
		
		else if (GetAsyncKeyState(keycode1 & ~((0x1 << 8) + (0x1 << 9))) & 0x8000)
		{
			if ((keycode1 != 0 && !(keycode1 & (0x1 << 9)) && !(keycode1 & (0x1 << 8))) || ((keycode1 & (0x1 << 9)) && (GetAsyncKeyState(VK_CONTROL) & 0x8000 || GetAsyncKeyState(VK_PROCESSKEY) & 0x8000)) || ((keycode1 & (0x1 << 8)) && GetAsyncKeyState(VK_SHIFT) & 0x8000))
			{
				GetCursorPos(&pt);
				PostMessage(target_window, WM_MOUSEWHEEL, MAKEWPARAM(0, WHEEL_DELTA), MAKELPARAM(pt.x, pt.y));
			}
		}
		else if (GetAsyncKeyState(keycode2 & ~((0x1 << 8) + (0x1 << 9))) & 0x8000)
		{
			if ((keycode2 != 0 && !(keycode2 & (0x1 << 9)) && !(keycode2 & (0x1 << 8))) || ((keycode2 & (0x1 << 9)) && (GetAsyncKeyState(VK_CONTROL) & 0x8000 || GetAsyncKeyState(VK_PROCESSKEY) & 0x8000)) || ((keycode2 & (0x1 << 8)) && GetAsyncKeyState(VK_SHIFT) & 0x8000))
			{
				GetCursorPos(&pt);
				PostMessage(target_window, WM_MOUSEWHEEL, MAKEWPARAM(0, -WHEEL_DELTA), MAKELPARAM(pt.x, pt.y));
			}
		}
		else if (GetAsyncKeyState(keycode3 & ~((0x1 << 8) + (0x1 << 9))) & 0x8000)
		{
			if ((keycode3 != 0 && !(keycode3 & (0x1 << 9)) && !(keycode3 & (0x1 << 8))) || ((keycode3 & (0x1 << 9)) && (GetAsyncKeyState(VK_CONTROL) & 0x8000 || GetAsyncKeyState(VK_PROCESSKEY) & 0x8000)) || ((keycode3 & (0x1 << 8)) && GetAsyncKeyState(VK_SHIFT) & 0x8000))
			{
				GetCursorPos(&pt);
				ScreenToClient(target_window, &pt);
				hwnd = GetForegroundWindow();
				keybd_event(VK_SHIFT, 0x2A, KEYEVENTF_KEYUP, 0);
				SendMessage(target_window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
				SendMessage(target_window, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
				SetForegroundWindow(hwnd);
				fi.cbSize = sizeof(FLASHWINFO);
				fi.dwFlags = FLASHW_STOP;
				fi.hwnd = target_window;
				FlashWindowEx(&fi);
			}

		}
		Sleep(50);
	}
	return 0;
}




LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	char buf[128] = { 0, };
	unsigned char keycode, scancode;

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
			|| keycode == VK_MENU || keycode == VK_BACK
			|| keycode == VK_LWIN || keycode == VK_RWIN
			|| keycode == VK_APPS || keycode == VK_LEFT
			|| keycode == VK_RIGHT || keycode == VK_UP
			|| keycode == VK_DOWN || keycode == VK_CAPITAL
			|| keycode == VK_OEM_2 || keycode == VK_ESCAPE)
			return 0;
		else if (keycode == VK_CONTROL || keycode == VK_PROCESSKEY || keycode == VK_SHIFT)
			return 0;
		else if (keycode >= VK_F1 && keycode <= VK_F12)
		{
			if (keycode == VK_F4 && GetAsyncKeyState(VK_MENU) & 0x8000)
				PostQuitMessage(0);
			wsprintf(buf, "F%u", keycode - 111);
		}
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

		*(unsigned char*)dwRefData = keycode;
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000 || GetAsyncKeyState(VK_PROCESSKEY) & 0x8000)
		{
			memcpy(&buf[7], buf, strlen(buf));
			memcpy(buf, "CTRL + ", 7);
			*(unsigned int*)dwRefData |= 0x1 << 9;

		}
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			memcpy(&buf[8], buf, strlen(buf));
			memcpy(buf, "SHIFT + ", 8);
			*(unsigned int*)dwRefData |= 0x1 << 8;
		}
		SetWindowText(hWnd, buf);
		DestroyCaret();
		return 0;
	case WM_CHAR:
		wParam = 0;
		return 0;
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char buf[128];

	switch (iMessage)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			SetFocus(GetDlgItem(hDlg, IDC_LIST1));
		break;
	case WM_USER + 7:
		target_window = GetForegroundWindow();
		GetWindowText(target_window, buf, sizeof(buf));
		SendDlgItemMessage(hDlg, IDC_LIST1, LB_DELETESTRING, 0, 0);
		SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, 0, (LPARAM)buf);
		SetDlgItemText(hDlg, IDC_BUTTON1, "윈도우 세팅");
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1), TRUE);
		setting = 0;

		break;
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, 0, 0, 0));
		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), EditSubclassProc, 0, (DWORD_PTR)&keycode1);
		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT2), EditSubclassProc, 1, (DWORD_PTR)&keycode2);
		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT3), EditSubclassProc, 2, (DWORD_PTR)&keycode3);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			setting = 1;
			SetDlgItemText(hDlg, IDC_BUTTON1, "우클릭");
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1), FALSE);
			break;
		}
		break;
	case WM_CLOSE:
		RemoveWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), EditSubclassProc, 0);
		RemoveWindowSubclass(GetDlgItem(hDlg, IDC_EDIT2), EditSubclassProc, 1);
		RemoveWindowSubclass(GetDlgItem(hDlg, IDC_EDIT3), EditSubclassProc, 2);
		PostQuitMessage(0);
		break;
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	g_hInstance = hInstance;

	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(GetDesktopWindow(), "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
		return 0;
	}

	CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();
	return 0;
}