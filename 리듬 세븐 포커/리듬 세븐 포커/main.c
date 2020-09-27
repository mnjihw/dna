#include "common.h"

HINSTANCE g_hInst;
HWND g_hWnd;
HBITMAP g_hBitmap[4][13];
SIZE window_size;
RECT window_rect;
list_t *list_of_cards, *my_cards;
int textout_idx[2];
char textout_buf[2][256];


void DrawBitmap(HDC hdc, int x, int y, int bx, int by, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;


	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	TransparentBlt(hdc, x, y, bx, by, MemDC, 0, 0, bx, by, RGB(255, 123, 201));

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}

const char* get_suit_string(int suit)
{
	switch (suit)
	{
	case CLUB:
		return "♣";
		break;
	case HEART:
		return "♥";
		break;
	case DIAMOND:
		return "◆";
		break;
	case SPADE:
		return "♠";
		break;
	}
	return NULL;
}

const char* get_number_string(int number)
{
	static char buf[32];

	switch (number)
	{
	case 1:
		return "A";
	case 11:
		return "J";
	case 12:
		return "Q";
	case 13:
		return "K";
	default:
		wsprintf(buf, "%d", number);
		break;
	}
	return buf;
}

void display_cards(HDC hdc, list_t *list)
{
	const int GAP = 10, WIDTH = 99, HEIGHT = 145;
	node_t *pnode;
	int i, x = GAP, y = GAP;

	pnode = list->head.next;


	for (i = 0; i < list->count; ++i)
	{
		DrawBitmap(hdc, x, y, WIDTH, HEIGHT, g_hBitmap[pnode->suit][pnode->number - 1]);
		x += WIDTH + GAP;
		pnode = pnode->next;
	}
}



int get_suit_of_number(list_t *list, int number)
{
	int i;
	node_t *pnode;

	for (i = 0; i < list->count; ++i)
	{
		pnode = get_element(list, i);

		if (pnode->number == number)
			return pnode->suit;
	}
	return -1;
}

result_t judge_rank_of_cards(list_t* list)
{
	int i, j, number_count[13], suit_count[4], cards_by_suit[4][7] = { 0, };
	node_t *p, *p2;
	result_t result;
	struct
	{
		int number[7];
		int idx;
	}continuous_cards;

	memset(number_count, 0, sizeof(number_count));
	memset(suit_count, 0, sizeof(suit_count));

	for (i = 0, continuous_cards.idx = 0, continuous_cards.number[continuous_cards.idx++] = get_element(list, 0)->number; i < 7; ++i)
	{
		p2 = get_element(list, i);

		if (i != 0)
		{
			p = get_element(list, i - 1);

			if (!(p->number == p2->number + 1) && !(p->number == p2->number) && continuous_cards.idx < 4)
				continuous_cards.idx = 0;
			continuous_cards.number[continuous_cards.idx++] = p2->number;
		}
		++number_count[p2->number - 1]; //넘버가 1부터 시작함
		cards_by_suit[p2->suit][suit_count[p2->suit]++] = p2->number;
	}

	result.rank = TOP;

	for (i = CLUB; i <= SPADE; ++i)
	{
		if (suit_count[i] >= 5)
			break;
	}

	if (i <= SPADE)
	{
		result.rank = FLUSH;
		result.suit = i;
		result.top = cards_by_suit[i][0];

		for (j = 0; j < suit_count[i] - 4; ++j)
		{
			if ((cards_by_suit[i][j] == cards_by_suit[i][j + 1] + 1) && (cards_by_suit[i][j] == cards_by_suit[i][j + 2] + 2) && (cards_by_suit[i][j] == cards_by_suit[i][j + 3] + 3))
			{
				if (cards_by_suit[i][j] == 13 && cards_by_suit[i][suit_count[i] - 1] == 1)
				{
					result.rank = RTF;
					break;
				}
				else if (cards_by_suit[i][j] == cards_by_suit[i][j + 4] + 4)
				{
					result.rank = STF;
					break;
				}
			}
		}
		return result;
	}

	for (i = 0; i < 13; ++i)
	{
		if (number_count[i] == 4)
		{
			result.rank = FOUR_CARD;
			result.top = i + 1;
			result.suit = 0;

			return result;
		}
		else if (number_count[i] == 3)
		{
			if (result.rank == TRIPLE && result.top == 1)
				return result;

			result.top = i + 1;
			result.suit = get_suit_of_number(list, i + 1);

			if (result.rank == ONE_PAIR || result.rank == TWO_PAIR)
			{
				result.rank = FULL_HOUSE;
				return result;
			}
			else
				result.rank = TRIPLE;
		}
		else if (number_count[i] == 2)
		{
			if (result.rank == TRIPLE)
			{
				result.rank = FULL_HOUSE;
				return result;
			}
			else
			{
				if (result.rank == ONE_PAIR && result.top == 1)
				{
					result.rank = TWO_PAIR;
					return result;
				}
				result.top = i + 1;
				result.suit = get_suit_of_number(list, i + 1);

				if (result.rank == ONE_PAIR || result.rank == TWO_PAIR)
					result.rank = TWO_PAIR;
				else
					result.rank = ONE_PAIR;
			}
		}
	}

	for (i = 0; i < continuous_cards.idx - 4; ++i)
	{
		if ((continuous_cards.number[i] == continuous_cards.number[i + 1] + 1) && (continuous_cards.number[i] == continuous_cards.number[i + 2] + 2) && (continuous_cards.number[i] == continuous_cards.number[i + 3] + 3))
		{
			if (continuous_cards.number[i] == 13 && continuous_cards.number[continuous_cards.idx - 1] == 1)
			{
				result.rank = MOUNTAIN;
				result.top = 1;
				result.suit = get_suit_of_number(list, continuous_cards.number[i]);

				return result;
			}
			else if (continuous_cards.number[i] == continuous_cards.number[i + 4] + 4)
			{
				result.top = continuous_cards.number[i];
				result.suit = get_suit_of_number(list, continuous_cards.number[i]);

				if (continuous_cards.number[i] == 5)
					result.rank = BACK_STRAIGHT;
				else
					result.rank = STRAIGHT;
				return result;
			}
		}
	}

	if (result.rank == TOP)
	{
		result.top = 0;
		result.suit = 0;
	}
	return result;
}

void add_whole_cards(list_t *list)
{
	int i, j;
	node_t node;

	for (j = CLUB; j <= SPADE; ++j)
	{
		for (i = 1; i <= 13; ++i)
		{
			node.number = i;
			node.suit = j;
			add_element(list, node, FALSE);
		}
	}
}



void shuffle_cards(list_t *list)
{
	int i, pos, numberof_cards;
	node_t *pnode;

	numberof_cards = list->count;

	for (i = 0; i < numberof_cards; ++i)
	{
		pos = rand() % (numberof_cards - i);
		pnode = get_element(list, pos);
		add_element(list, *pnode, FALSE);
		remove_element(list, pos);
	}
}

void dbg(const char *fmt, ...)
{
	char data[1024];
	wvsprintf(data, fmt, (va_list)((unsigned int)&fmt + sizeof(void*)));
	OutputDebugString(data);

}

BOOL OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc, hMemDC;
	HBITMAP hMemBitmap, hOldBitmap;
	HBRUSH hBrush, hOldBrush;
	int i;

	hdc = BeginPaint(hWnd, &ps);
	hMemDC = CreateCompatibleDC(hdc);
	hMemBitmap = CreateCompatibleBitmap(hdc, window_size.cx, window_size.cy);
	hBrush = CreateSolidBrush(RGB(0, 128, 64));
	hOldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hMemBitmap);
	FillRect(hMemDC, &window_rect, hBrush);

	display_cards(hMemDC, my_cards);
	SetBkMode(hMemDC, TRANSPARENT);

	for (i = 0; i < _countof(textout_idx); ++i)
	{
		if (textout_idx[i])
		{
			TextOut(hMemDC, 30, 170, textout_buf[i], strlen(textout_buf[i]));
		}
	}
	
	BitBlt(hdc, 0, 0, window_size.cx, window_size.cy, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBrush);
	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBrush);
	DeleteObject(hMemBitmap);
	DeleteDC(hMemDC);

	EndPaint(hWnd, &ps);

	return TRUE;
}


BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	node_t node, *pnode;
	result_t result;
	static const char ranks[][64] = {"탑", "원페어", "투페어", "트리플", "스트레이트", "백스트레이트", "마운틴", "플러시", "풀하우스", "포카드", "스트레이트 플러시", "로얄 스트레이트 플러시"};
	RECT rect;
	static const int WIDTH = 99, HEIGHT = 145, GAP = 10;
	POINT pt;
	static int clicked_idx, discard_state;
	mystruct_t mystruct;
	

	switch (iMessage)
	{
	case WM_PAINT:
		return OnPaint(hDlg);
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			textout_idx[1] = 0;
			clear_list(list_of_cards);
			clear_list(my_cards);
			add_whole_cards(list_of_cards);
			shuffle_cards(list_of_cards);

			for (i = 0; i < 4; ++i)
			{
				pnode = get_element(list_of_cards, 0);
				add_element(my_cards, *pnode, TRUE);
				remove_element(list_of_cards, 0);
			}
			InvalidateRect(hDlg, NULL, FALSE);
			//display_cards(mystruct.hMemDC, my_cards);
			//SetBkMode(mystruct.hMemDC, TRANSPARENT);
			//TextOut(mystruct.hMemDC, 30, 170, "버릴 카드를 선택해주세요.", strlen("버릴 카드를 선택해주세요."));
			textout_idx[0] = 1;
			discard_state = 1;
			break;
		case IDC_BUTTON2:
			if (discard_state != -1 || my_cards->count == 7)
				break;
			
			pnode = get_element(list_of_cards, 0);
			add_element(my_cards, *pnode, TRUE);
			remove_element(list_of_cards, 0);
			//display_cards(mystruct.hMemDC, my_cards);
			InvalidateRect(hDlg, NULL, FALSE);

			if (my_cards->count == 7)
			{
				result = judge_rank_of_cards(my_cards);
				wsprintf(textout_buf[1], "%s%s %s", get_number_string(result.top), get_suit_string(result.suit), ranks[result.rank]);
				textout_idx[1] = 1;
				//SetBkMode(mystruct.hMemDC, TRANSPARENT);
				//TextOut(mystruct.hMemDC, 30, 170, buf, strlen(buf));
			}

			break;
		}
		break;
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		GetClientRect(hDlg, &window_rect);
		window_size.cx = window_rect.right - window_rect.left;
		window_size.cy = window_rect.bottom - window_rect.top;
		wsprintf(textout_buf[0], "버릴 카드를 선택해주세요.");

		for (i = CLUB; i <= SPADE; ++i)
		{
			for (j = 0; j < 13; ++j)
				g_hBitmap[i][j] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1 + i * 13 + j));
		}
		srand(GetCurrentProcessId() ^ GetTickCount());
		create_list(&list_of_cards);
		create_list(&my_cards);
		break;
	case WM_LBUTTONDOWN:
		if (discard_state != 1)
			break;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		for (i = 0; i < my_cards->count; ++i)
		{
			rect.left = GAP + (WIDTH + GAP) * i;
			rect.top = GAP;
			rect.right = rect.left + WIDTH;
			rect.bottom = rect.top + HEIGHT;

			if (PtInRect(&rect, pt))
			{
				clicked_idx = i;
				break;
			}
		}
		break;
	case WM_LBUTTONUP:
		if (discard_state != 1)
			break;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		for (i = 0; i < my_cards->count; ++i)
		{
			rect.left = GAP + (WIDTH + GAP) * i;
			rect.top = GAP;
			rect.right = rect.left + WIDTH;
			rect.bottom = rect.top + HEIGHT;

			if (PtInRect(&rect, pt) && clicked_idx == i)
			{
				remove_element(my_cards, i);
				discard_state = -1;
				textout_idx[0] = 0;
				InvalidateRect(hDlg, NULL, FALSE);

				//before_painting(&mystruct);
				//display_cards(mystruct.hMemDC, my_cards);
				//after_painting(&mystruct);
				
				break;
			}
		}
		break;
	case WM_CLOSE:
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
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	g_hInst = hInstance;
	CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();
	return 0;
}


