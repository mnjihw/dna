#include "common.h"


VOID MessageLoop()
{
	MSG Message;

	while (GetMessage(&Message, 0, 0, 0) > 0)
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (iMessage)
	{
	case WM_CHAR:
		if (wParam == VK_RETURN)
		{
			SendMessage(GetParent(hWnd), WM_COMMAND, IDC_BUTTON1, 0);
			return 0;
		}
		break;
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}


void trimstring(char *str)
{
	unsigned int i;

	for (i = 0; i < strlen(str); ++i)
		if (str[i] == ' ')
			while (str[i] == ' ')
				memcpy(&str[i], &str[i + 1], strlen(str) - i); //스페이스바 제거
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	unsigned int i, j, sel;
	char before[1024], after[1024], buf[1024], temp[16];

	switch (iMessage)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, (WPARAM)sizeof(before) / 2 - 1, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT2, EM_LIMITTEXT, (WPARAM)sizeof(after) / 2 - 1, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"문자열->패킷");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"패킷->문자열");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"10진수->패킷");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"패킷->10진수");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, 0, 0);
		SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, 0, (LPARAM)"사이즈 : 0");
		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), EditSubclassProc, 0, 0);
		SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			sel = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
			if (sel == CB_ERR)
			{
				MessageBox(0, "변환할 종류를 선택해주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}
			if (GetDlgItemText(hDlg, IDC_EDIT1, before, sizeof(before) - 1) == 0)
			{
				MessageBox(0, "변환할 내용을 입력해주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}

			switch (sel)
			{
			case 0:
				for (i = 0, j = 0; i < strlen(before); ++i, j += 3)
				{
					*(unsigned short*)&after[j] = *(unsigned short*)CharUpper(_itoa(before[i] & 0xFF, temp, 16));
					after[j + 2] = ' ';
				}
				after[j - 1] = '\0'; //마지막 공백 대신 널문자 넣어줌
				wsprintf(buf, "사이즈 : 0x%X", i);
				break;
			case 1:
				trimstring(before);
				for (i = 0, j = 0;  i < strlen(before) >> 1; ++i, j += 2)
				{
					memcpy(temp, &before[j], 2);
					temp[2] = '\0';
					after[i] = (char)strtol(temp, NULL, 16);
				}
				after[i] = '\0';
				wsprintf(buf, "사이즈 : 0x%X", i);
				break;
			case 2:
				for (i = 0; i < strlen(before); ++i)
				{
					if (!isdigit(before[i]))
					{
						MessageBox(0, "숫자만 입력해주세요", "알림", MB_ICONINFORMATION);
						return 0;
					}
				}

				CharUpper(_itoa(_byteswap_ulong(GetDlgItemInt(hDlg, IDC_EDIT1, 0, FALSE)), after, 16));
				wsprintf(buf, "사이즈 : 4");
				break;
			case 3:
				trimstring(before);
				for (i = 0, j = 0; i < strlen(before) >> 1; ++i, j += 2)
					*(unsigned short*)&buf[j] = *(unsigned short*)&before[strlen(before) - j - 2];
				buf[j] = '\0';
				wsprintf(after, "%u", strtol(buf, NULL, 16));
				wsprintf(buf, "사이즈 : 4");
				break;
			}
			SetDlgItemText(hDlg, IDC_EDIT2, after);
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_DELETESTRING, 0, 0);
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, 0, (LPARAM)buf);
			break;
		}
		break;
	case WM_CLOSE:
		RemoveWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), EditSubclassProc, 0);
		PostQuitMessage(0);
		break;
	}
	return 0;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hwnd;

	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR);
		return 0;
	}

	if ((hwnd = FindWindow("#32770", "리듬 패킷 컨버터")))
	{
		SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		SetForegroundWindow(hwnd);
		return 0;
	}

	CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();

	return 0;
}