#include "common.h"


HWND g_hWnd, g_HiddenPowerDlg, g_SearchDlg;
HANDLE hThread;


int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	LVITEMW li;
	wchar_t buf[64], buf2[64];

	li.iSubItem = ((sortinfo_t*)lParamSort)->iSubItem;
	li.cchTextMax = sizeof(buf);
	li.pszText = buf;

	SendDlgItemMessageW(g_SearchDlg, IDC_LIST1, LVM_GETITEMTEXTW, lParam1, (LPARAM)&li);


	li.iSubItem = ((sortinfo_t*)lParamSort)->iSubItem;
	li.cchTextMax = sizeof(buf2);
	li.pszText = buf2;

	SendDlgItemMessageW(g_SearchDlg, IDC_LIST1, LVM_GETITEMTEXTW, lParam2, (LPARAM)&li);
	
	return ((sortinfo_t*)lParamSort)->ascending[((sortinfo_t*)lParamSort)->iSubItem] ? StrCmpLogicalW(buf, buf2) : -StrCmpLogicalW(buf, buf2);
}



BOOL IsRowSelected(HWND hWnd, int row)
{
	return ListView_GetItemState(hWnd, row, LVIS_SELECTED);
}

BOOL CALLBACK SearchDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	LVCOLUMN col;
	int idx = 0;
	static sortinfo_t si;
	static unsigned int sorted;
	RECT rect;
	int row, bSelected;
	static bool flip;
	char buf[128];

	switch (iMessage)
	{
	case WM_NCHITTEST:
		if (DefWindowProc(hDlg, iMessage, wParam, lParam) == HTCLIENT)
			SetWindowLong(hDlg, DWL_MSGRESULT, HTCAPTION);
		break;
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->idFrom == IDC_LIST1)
		{
			if (((LPNMLISTVIEW)lParam)->hdr.code == LVN_COLUMNCLICK)
			{
				si.ascending[((LPNMLISTVIEW)lParam)->iSubItem] = !si.ascending[((LPNMLISTVIEW)lParam)->iSubItem];
				si.iSubItem = ((LPNMLISTVIEW)lParam)->iSubItem;
				ListView_SortItemsEx(GetDlgItem(hDlg, IDC_LIST1), CompareFunc, (LPARAM)&si);
				sorted = 1;
			}
			else if (((LPNMITEMACTIVATE)lParam)->hdr.code == NM_DBLCLK)
			{
				if ((idx = ((LPNMITEMACTIVATE)lParam)->iItem) == -1)
					break;
				ListView_GetItemText(GetDlgItem(hDlg, IDC_LIST1), idx, 1, buf, sizeof(buf));
				SetDlgItemText(g_hWnd, IDC_EDIT20, buf);
				SendMessage(g_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, 0), 0);
			}
			else if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
			{
				switch (((LPNMLVCUSTOMDRAW)lParam)->nmcd.dwDrawStage)
				{
				case CDDS_POSTPAINT:
					if (!flip)
					{
						flip = 1;
						si.ascending[1] = !si.ascending[1];
						si.iSubItem = 1;
						ListView_SortItemsEx(GetDlgItem(hDlg, IDC_LIST1), CompareFunc, (LPARAM)&si);
					}
					break;
				case CDDS_PREPAINT:
					SetWindowLong(hDlg, DWL_MSGRESULT, CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT);
					break;
				case CDDS_ITEMPREPAINT:
					row = ((LPNMLVCUSTOMDRAW)lParam)->nmcd.dwItemSpec;
					bSelected = IsRowSelected(GetDlgItem(hDlg, IDC_LIST1), row);
					if (bSelected)
					{
						((LPNMLVCUSTOMDRAW)lParam)->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
						((LPNMLVCUSTOMDRAW)lParam)->clrTextBk = GetSysColor(COLOR_HIGHLIGHT);
					}
					SetWindowLong(hDlg, DWL_MSGRESULT, CDRF_DODEFAULT);
				
					break;
				}
			}
		}

		break;
	case WM_INITDIALOG:
		SetLayeredWindowAttributes(hDlg, 0, 255, LWA_ALPHA);

		ListView_SetExtendedListViewStyle(GetDlgItem(hDlg, IDC_LIST1), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		col.fmt = LVCFMT_LEFT;
		col.cx = 50;
		col.pszText = "도감 번호";
		col.cchTextMax = strlen("도감 번호");
		col.iSubItem = 0;
		ListView_InsertColumn(GetDlgItem(hDlg, IDC_LIST1), 0, &col);

		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		col.fmt = LVCFMT_LEFT;
		col.cx = 150;
		col.pszText = "이름";
		col.cchTextMax = strlen("이름");
		ListView_InsertColumn(GetDlgItem(hDlg, IDC_LIST1), 1, &col);
		
		GetWindowRect(GetDlgItem(g_hWnd, IDC_EDIT12), &rect);
		SetWindowPos(hDlg, 0, rect.right, rect.top, 0, 0, SWP_NOSIZE);
		
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK HiddenPowerDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap;
	RECT rect;
	stats_t iv;
	unsigned int index;
	const char types[][8] = {"격투", "비행", "독", "땅", "바위", "벌레", "고스트", 
		"강철", "불", "물", "풀", "전기", "에스퍼", "얼음", "드래곤", "악", "페어리"};
	char buf[128];
	BOOL result;

	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			index = 0;
			iv.h = GetDlgItemInt(hDlg, IDC_EDIT1, &result, FALSE);
			if (result == 0)
				SetDlgItemInt(hDlg, IDC_EDIT1, 0, FALSE);
			iv.a = GetDlgItemInt(hDlg, IDC_EDIT2, &result, FALSE);
			if (result == 0)
				SetDlgItemInt(hDlg, IDC_EDIT2, 0, FALSE);
			iv.b = GetDlgItemInt(hDlg, IDC_EDIT3, &result, FALSE);
			if (result == 0)
				SetDlgItemInt(hDlg, IDC_EDIT3, 0, FALSE);
			iv.c = GetDlgItemInt(hDlg, IDC_EDIT4, &result, FALSE);
			if (result == 0)
				SetDlgItemInt(hDlg, IDC_EDIT4, 0, FALSE);
			iv.d = GetDlgItemInt(hDlg, IDC_EDIT5, &result, FALSE);
			if (result == 0)
				SetDlgItemInt(hDlg, IDC_EDIT5, 0, FALSE);
			iv.s = GetDlgItemInt(hDlg, IDC_EDIT6, &result, FALSE);
			if (result == 0)
				SetDlgItemInt(hDlg, IDC_EDIT6, 0, FALSE);
			if (iv.h > 31 || iv.a > 31 || iv.b > 31 || iv.c > 31 || iv.d > 31 || iv.s > 31)
			{
				MessageBox(hDlg, "개체값을 제대로 입력해주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}
			index += iv.h & 1;
			index += (iv.a & 1) << 1;
			index += (iv.b & 1) << 2;
			index += (iv.c & 1) << 3;
			index += (iv.d & 1) << 4;
			index += (iv.s & 1) << 5;
			index = index * 15 / 63;
			iv.h &= 3;
			if (iv.h == 2 || iv.h == 3)
				iv.h = 1;
			iv.a &= 3;
			if (iv.a == 2 || iv.a == 3)
				iv.a = 2;
			iv.b &= 3;
			if (iv.b == 2 || iv.b == 3)
				iv.b = 4;
			iv.c &= 3;
			if (iv.c == 2 || iv.c == 3)
				iv.c = 8;
			iv.d &= 3;
			if (iv.d == 2 || iv.d == 3)
				iv.d = 16;
			iv.s &= 3;
			if (iv.s == 2 || iv.s == 3)
				iv.s = 32;
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_RESETCONTENT, 0, 0);
			wsprintf(buf, "타입: %s", types[index]);
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, 0, (LPARAM)buf);
			wsprintf(buf, "위력: %u", (iv.h + iv.a + iv.b + iv.c + iv.d + iv.s) * 40 / 63 + 30);
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, 1, (LPARAM)buf);
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, 2, (LPARAM)"(6세대 이후는 위력 60 고정)");
			break;
		}
		break;
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, (WPARAM)2, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT2, EM_LIMITTEXT, (WPARAM)2, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT3, EM_LIMITTEXT, (WPARAM)2, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT4, EM_LIMITTEXT, (WPARAM)2, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT5, EM_LIMITTEXT, (WPARAM)2, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT6, EM_LIMITTEXT, (WPARAM)2, 0);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
		hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2));
		SendDlgItemMessage(hDlg, IDC_PICTURE2, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);
		GetClientRect(GetDlgItem(hDlg, IDC_PICTURE2), &rect);
		SetWindowPos(GetDlgItem(hDlg, IDC_PICTURE2), 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
		break;
	case WM_CLOSE:
		g_HiddenPowerDlg = NULL;
		DeleteObject((HBITMAP)hBitmap);
		EndDialog(hDlg, 0);
		break;
	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LONG)GetStockObject(NULL_BRUSH);
	default:
		return FALSE;
	}
	return TRUE;
}

VOID MessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		if ((!IsWindow(g_hWnd) || !IsDialogMessage(g_hWnd, &msg)) && (!IsWindow(g_HiddenPowerDlg) ||  !IsDialogMessage(g_HiddenPowerDlg, &msg)))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}





BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap;
	RECT rect;
	char name[128], *s, *p, index[8], buf[128], buf2[128], str_min[32], str_max[32];
	HANDLE hRes, hResInfo;
	static void *data; 
	stats_t base_stats;
	DWORD size;
	int idx;
	unsigned int i, j, count, effort_value, level, nature_sel, stat, base_stat, min, max;
	LVITEM li;
	static sortinfo_t si;
	double nature_effect_of_stat;
	LPDRAWITEMSTRUCT lpdis;
	nature_effect_t nature;

	typedef enum
	{
		H, A, B, C, D, S, COUNT
	} STAT;

	struct
	{
		unsigned int len_difference;
		int idx;
	}unnamed;

	switch (iMessage)
	{
	case WM_MOVE:
		GetWindowRect(GetDlgItem(hDlg, IDC_EDIT12), &rect);
		SetWindowPos(g_SearchDlg, 0, rect.right, rect.top, 0, 0, SWP_NOSIZE);
		break;

	case WM_ACTIVATE:
		if (wParam == TRUE)
			SetWindowPos(g_SearchDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		else
			SetWindowPos(g_SearchDlg, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EDIT20:
			if (HIWORD(wParam) == EN_UPDATE)
			{
				if (g_SearchDlg)
				{
					if (GetDlgItemText(hDlg, IDC_EDIT20, name, sizeof(name)) == 0)
						break;
					ShowWindow(g_SearchDlg, SW_SHOWNA);
					count = ListView_GetItemCount(GetDlgItem(g_SearchDlg, IDC_LIST1));
					unnamed.len_difference = 0xFFFFFFFF;
					unnamed.idx = -1;
					for (i = 0; i < count; ++i)
					{
						ListView_GetItemText(GetDlgItem(g_SearchDlg, IDC_LIST1), i, 1, buf, sizeof(buf));

						if (strstr(buf, name))
						{
							if (strlen(buf) - strlen(name) < unnamed.len_difference)
							{
								unnamed.idx = i;
								unnamed.len_difference = strlen(buf) - strlen(name);
							}

						}
					}
					SelectLVItem(GetDlgItem(g_SearchDlg, IDC_LIST1), unnamed.idx);
				}
			}
			break;
		case IDOK:
			if (GetFocus() == GetDlgItem(hDlg, IDC_EDIT20))
			{
				//i = SendDlgItemMessage(g_SearchDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);
				//if (i != -1)
				{
					SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, 0), 0);
				}
			}
			break;
		case IDC_BUTTON1: //개체값 계산
			if ((nature_sel = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0)) == -1)
			{
				MessageBox(hDlg, "성격을 선택해주세요.", "알림", MB_OK);
				SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SHOWDROPDOWN, (WPARAM)TRUE, 0);
				break;
			}


			memset(&nature, 0, sizeof(nature));


			switch (nature_sel)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				nature.a = 1;
				break;
			case 4:
			case 5:
			case 6:
			case 7:
				nature.b = 1;
				break;
			case 8:
			case 9:
			case 10:
			case 11:
				nature.c = 1;
				break;
			case 12:
			case 13:
			case 14:
			case 15:
				nature.d = 1;
				break;
			case 16:
			case 17:
			case 18:
			case 19:
				nature.s = 1;
				break;
			}

			switch (nature_sel)
			{
			case 4:
			case 8:
			case 12:
			case 16:
				nature.a = 2;
				break;
			case 0:
			case 9:
			case 13:
			case 17:
				nature.b = 2;
				break;
			case 1:
			case 5:
			case 14:
			case 18:
				nature.c = 2;
				break;
			case 2:
			case 6:
			case 10:
			case 19:
				nature.d = 2;
				break;
			case 3:
			case 7:
			case 11:
			case 15:
				nature.s = 2;
				break;
			}
			level = GetDlgItemInt(hDlg, IDC_EDIT19, FALSE, FALSE);
			if (level == 0 || level > 100)
			{
				MessageBox(hDlg, "레벨을 제대로 입력해주세요.", "알림", MB_OK);
				return TRUE;
			}
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_RESETCONTENT, 0, 0);

			for (STAT stat_i = H; stat_i < COUNT; ++stat_i)
			{
				if ((base_stat = GetDlgItemInt(hDlg, IDC_EDIT1 + stat_i, FALSE, FALSE)) == 0)
				{
					MessageBox(hDlg, "포켓몬을 검색하거나 종족값을 입력해주세요.", "알림", MB_OK);
					return TRUE;
				}
				if ((effort_value = GetDlgItemInt(hDlg, IDC_EDIT7 + stat_i, FALSE, FALSE)) > 255)
					SetDlgItemInt(hDlg, IDC_EDIT7 + stat_i, (effort_value = 255), FALSE);

				min = 0xFFFFFFFF;
				max = 0xFFFFFFFF;
				stat = GetDlgItemInt(hDlg, IDC_EDIT13 + stat_i, FALSE, FALSE);


				for (j = 0; j < 32; ++j)
				{
					if (stat_i == H)
					{
						if (CalHP(base_stat, j, effort_value, level) == stat)
						{
							if (min == 0xFFFFFFFF)
							{
								min = j;
								max = j;
							}
							else
								max = j;
						}
					}
					else
					{
						nature_effect_of_stat = 1.0;

						switch (stat_i)
						{
						case A:
							if(nature.a == 1)
								nature_effect_of_stat = 1.1;
							else if(nature.a == 2)
								nature_effect_of_stat = 0.9;
							break;
						case B:
							if(nature.b == 1)
								nature_effect_of_stat = 1.1;
							else if(nature.b == 2)
								nature_effect_of_stat = 0.9;
							break;
						case C:
							if(nature.c == 1)
								nature_effect_of_stat = 1.1;
							else if(nature.c == 2)
								nature_effect_of_stat = 0.9;
							break;
						case D:
							if(nature.d == 1)
								nature_effect_of_stat = 1.1;
							else if(nature.d == 2)
								nature_effect_of_stat = 0.9;
							break;
						case S:
							if(nature.s == 1)
								nature_effect_of_stat = 1.1;
							else if(nature.s == 2)
								nature_effect_of_stat = 0.9;
							break;

						}
						if (CalOther(base_stat, j, effort_value, level, nature_effect_of_stat) == stat)
						{
							if (min == 0xFFFFFFFF)
							{
								min = j;
								max = j;
							}
							else
								max = j;
						}
					}
				}
				switch (stat_i)
				{
				case H:
					strcpy(buf2, "HP");
					break;
				case A:
					strcpy(buf2, "공격");
					break;
				case B:
					strcpy(buf2, "방어");
					break;
				case C:
					strcpy(buf2, "특공");
					break;
				case D:
					strcpy(buf2, "특방");
					break;
				case S:
					strcpy(buf2, "스피드");
					break;
				}
				switch (min)
				{
				case 0:
					strcpy(str_min, "0(Z)");
					break;
				case 30:
					strcpy(str_min, "30(U)");
					break;
				case 31:
					strcpy(str_min, "31(V)");
					break;
				case 0xFFFFFFFF:
					strcpy(str_min, "실능 수치 오류");
					break;
				default:
					wsprintf(str_min, "%u", min);
					break;
				}
				switch (max)
				{
				case 30:
					strcpy(str_max, "30(U)");
					break;
				case 31:
					strcpy(str_max, "31(V)");
					break;
				default:
					wsprintf(str_max, "%u", max);
					break;
				}



				if (min < max)
					wsprintf(buf, "%s: %s ~ %s", buf2, str_min, str_max);
				else
					wsprintf(buf, "%s: %s", buf2, str_min);
				SendDlgItemMessage(hDlg, IDC_LIST1, LB_INSERTSTRING, stat_i, (LPARAM)buf);
			}
			break;

		case IDC_BUTTON2: //결과 복사
			count = SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCOUNT, 0, 0);
			if (!count)
			{
				MessageBox(hDlg, "복사할 결과가 없습니다.", "알림", MB_OK);
				break;
			}
			p = buf;
			for (i = 0; i < count; ++i)
			{
				SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETTEXT, i, (LPARAM)buf2);
				p += _strcpy(p, buf2) + 2;
				*(unsigned short*)(p - 2) = '\r\n';
			}
			*(p - 2) = '\0';
			Clipboard(buf);
			break;

		case IDC_BUTTON3: //검색
			ShowWindow(g_SearchDlg, SW_HIDE);
			if (!GetDlgItemText(hDlg, IDC_EDIT20, name, sizeof(name)))
			{
				MessageBox(hDlg, "포켓몬 이름을 입력해주세요.", "알림", MB_OK);
				break;
			}
			CharUpper(name);

			count = ListView_GetItemCount(GetDlgItem(g_SearchDlg, IDC_LIST1));
			s = data;

			for (i = 0; i < count; ++i)
			{
				sscanf(s, "%*s %s %*u %*u %*u %*u %*u %*u", buf);
				if (!strcmp(buf, name))
				{
					i = 0xFFFFFFFF;
					break;
				}
				s = strchr(s, '\n') + 1;
			}
			if (i == 0xFFFFFFFF)
			{
				sscanf(s, "%*s %*s %u %u %u %u %u %u", &base_stats.h, &base_stats.a, &base_stats.b, &base_stats.c, &base_stats.d, &base_stats.s);
				SetDlgItemInt(hDlg, IDC_EDIT1, base_stats.h, FALSE);
				SetDlgItemInt(hDlg, IDC_EDIT2, base_stats.a, FALSE);
				SetDlgItemInt(hDlg, IDC_EDIT3, base_stats.b, FALSE);
				SetDlgItemInt(hDlg, IDC_EDIT4, base_stats.c, FALSE);
				SetDlgItemInt(hDlg, IDC_EDIT5, base_stats.d, FALSE);
				SetDlgItemInt(hDlg, IDC_EDIT6, base_stats.s, FALSE);
			}
			else
			{
				MessageBox(hDlg, "입력하신 포켓몬을 찾지 못했습니다.", "알림", MB_OK);
				break;
			}

			break;
		case IDC_BUTTON4: //잠재파워
			if (!g_HiddenPowerDlg)
				g_HiddenPowerDlg = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG2), 0, HiddenPowerDlgProc);
			else
				SetForegroundWindow(g_HiddenPowerDlg);
			break;
		}
		break;
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT2, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT3, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT4, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT5, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT6, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT7, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT8, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT9, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT10, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT11, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT12, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT13, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT14, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT15, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT16, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT17, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT18, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT19, EM_LIMITTEXT, (WPARAM)3, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT20, CB_LIMITTEXT, (WPARAM)20, 0);


		SetDlgItemInt(hDlg, IDC_EDIT7, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT8, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT9, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT10, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT11, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT12, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT13, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT14, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT15, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT16, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT17, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT18, 0, FALSE);
		SetDlgItemInt(hDlg, IDC_EDIT19, 50, FALSE);


		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
		hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
		SendDlgItemMessage(hDlg, IDC_PICTURE1, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"외로움(공격 ↑, 방어 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"고집(공격 ↑, 특공 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"개구쟁이(공격 ↑, 특방 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"용감(공격 ↑, 스핏 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"대담(방어 ↑, 공격 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"장난꾸러기(방어 ↑, 특공 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"촐랑(방어 ↑, 특방 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"무사태평(방어 ↑, 스핏 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"조심(특공 ↑, 공격 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"의젓(특공 ↑, 방어 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"덜렁(특공 ↑, 특방 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"냉정(특공 ↑, 스핏 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"차분(특방 ↑, 공격 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"얌전(특방 ↑, 방어 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"신중(특방 ↑, 특공 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"건방(특방 ↑, 스핏 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"겁쟁이(스핏 ↑, 공격 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"성급(스핏 ↑, 방어 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"명랑(스핏 ↑, 특공 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"천진난만(스핏 ↑, 특방 ↓)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"노력(무보정)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"온순(무보정)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"성실(무보정)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"수줍(무보정)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"변덕(무보정)");

		g_SearchDlg = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG3), 0, SearchDlgProc);


		hResInfo = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TXT1), "TXT");
		hRes = LoadResource(GetModuleHandle(NULL), (HRSRC)hResInfo);
		size = SizeofResource(GetModuleHandle(NULL), (HRSRC)hResInfo);
		data = (char*)malloc(size + 1);
		strcpy(data, LockResource(hRes));
		s = data;

		idx = 0;

		do
		{
			sscanf(s, "%s %s %*s %*s %*s %*s %*s %*s", index, name);

			p = s;
			while (++p, *p != '\n' && *p != '\0')
				if (*p == '_')
					*p = ' ';

			p = name;
			while (++p, *p != '\n' && *p != '\0')
				if (*p == '_')
					*p = ' ';

			s = strchr(s, '\n') + 1;

			li.mask = LVIF_TEXT;
			li.pszText = index;
			li.iItem = idx;
			li.iSubItem = 0;
			ListView_InsertItem(GetDlgItem(g_SearchDlg, IDC_LIST1), &li);
			ListView_SetItemText(GetDlgItem(g_SearchDlg, IDC_LIST1), idx, 1, name);
			++idx;
		} while ((unsigned int)(s - (char*)data) < size);
		GetClientRect(GetDlgItem(hDlg, IDC_PICTURE1), &rect);
		SetWindowPos(GetDlgItem(hDlg, IDC_PICTURE1), 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
		break;
	case WM_CLOSE:
		free(data);
		DeleteObject((HBITMAP)hBitmap);
		DestroyWindow(hDlg);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CTLCOLORSTATIC:
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LONG)GetStockObject(NULL_BRUSH);

	case WM_DRAWITEM:
		lpdis = (LPDRAWITEMSTRUCT)lParam;
		if (lpdis->itemID == -1)
			break;		
		
		SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM)buf);
			
		if (IsStatUV(buf))
			SetTextColor(lpdis->hDC, RGB(255, 0, 0));
		else if (IsStatZ(buf))
			SetTextColor(lpdis->hDC, RGB(128, 128, 128));
		else
			SetTextColor(lpdis->hDC, RGB(0, 0, 0));
			
		TextOut(lpdis->hDC, lpdis->rcItem.left + 1, lpdis->rcItem.top, buf, strlen(buf));
		return TRUE;

	default:
		return FALSE;

	}
	return TRUE;
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_SYSTEMMODAL);
		return 0;
	}

	if(SetForegroundWindow(FindWindow("#32770", "리듬 개체값 계산기")))
		return 0;


	CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();

	return 0;
}