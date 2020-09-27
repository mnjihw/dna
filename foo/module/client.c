#include "common.h"

enum {
	kTimerNone = 0x2C80,
	kTimerShopCloseSub1,
	kTimerShopCloseSub2,
	kTimerWariClose,
	kTimerWari,
	kTimerShop,
	kTimerLogin,
	kTimerPacketSpam,
	kTimerWarp,
	kTimerEmptyWorkingSet,
	kTimerCSWari,
	kTimerMouse,
};

typedef struct {
	unsigned int size;
	unsigned char data[60];
} packet_t;

typedef struct {
	int v, c;
} pos_t;

typedef struct {
	unsigned int n;
	pos_t p[16];
} sect_t;

unsigned int secure_value_A;
HWND MainWnd, PacketWnd;
WNDPROC MainWndProc;
static LPARAM mouse_pos;
static unsigned int pt_sendpacket;
static unsigned int pt_CUIStatusBar, pt_chatlog;
static unsigned int pt_CUniqueModeless, pt_shoptype;
static unsigned int pt_CEmployeePool, pt_eshopid, pt_eshopx, pt_eshopy;
static unsigned int pt_CUserPool, pt_ushopid, pt_ushopmode, pt_uservec, pt_userx, pt_usery, pt_userid;
static unsigned int pt_CUserLocal, pt_getpos, pt_getvecctrl, pt_rawmove;
static unsigned int pt_CWvsContext, pt_skipfadein, pt_skipfadeout, pt_accid;
static unsigned int pt_CStage, pt_CUITitle, pt_CUIWorldSelect, pt_CUIChannelSelect, pt_CUICharSelect;
static unsigned int pt_waitresp, pt_charidx, pt_worlditem, pt_softkeyboard;
static unsigned int pt_CInputSystem, pt_mousevec, pt_mousex, pt_mousey;
static unsigned int signal_mode, wari_mode, cswari_mode, shop_mode, login_mode, timer_mode, mouse_mode;
static unsigned int shop_pos_idx, shop_pos_num;
static packet_t packet[8];
static pos_t shop_pos[64], save_pos[10];

void mouseinput1(HWND hwnd, int x, int y)
{
	VIRTUALIZER1_START
	{
		MainWndProc(hwnd, WM_MOUSEMOVE, 0, mouse_pos = MAKELONG(x, y));
		MainWndProc(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, mouse_pos = MAKELONG(x, y));
		MainWndProc(hwnd, WM_LBUTTONUP, 0, mouse_pos = MAKELONG(x, y));
	}
		VIRTUALIZER_END
}

void mouseinput2(HWND hwnd, int x, int y)
{
	VIRTUALIZER1_START
	{
		MainWndProc(hwnd, WM_MOUSEMOVE, 0, mouse_pos = MAKELONG(x, y));
		MainWndProc(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, mouse_pos = MAKELONG(x, y));
		MainWndProc(hwnd, WM_LBUTTONUP, 0, mouse_pos = MAKELONG(x, y));
		MainWndProc(hwnd, WM_LBUTTONDBLCLK, MK_LBUTTON, mouse_pos = MAKELONG(x, y));
		MainWndProc(hwnd, WM_LBUTTONUP, 0, mouse_pos = MAKELONG(x, y));
	}
		VIRTUALIZER_END
}

void keyboardinput1(HWND hwnd, unsigned int vk, unsigned int repeat)
{
	unsigned int i;

	VIRTUALIZER1_START
	{
		if (repeat) {
			i = ((unsigned int) SECURE_API(s_MapVirtualKeyA)(vk, 0) << 16) | 0x00000001;
			while (MainWndProc(hwnd, WM_KEYDOWN, vk, i), --repeat);
			MainWndProc(hwnd, WM_KEYUP, vk, i | 0xC0000000);
		}
	}
		VIRTUALIZER_END
}

void keyboardinput2(HWND hwnd, const char *ascii)
{
	VIRTUALIZER1_START
	{
		while (*ascii)
		MainWndProc(hwnd, WM_CHAR, *ascii++, 0);
	}
		VIRTUALIZER_END
}

////////////////////////////////////////////////////////////////////////////////

void msg(int type, const char *fmt, ...)
{
	char *p, data[1024];

	/*
	0 일반말
	1 귓말
	2 파티말
	3 친구말
	4 길드말
	5 연합말
	6 회색
	7 진한노란색
	8 연한노란색
	9 청색
	10 운영자말
	11 분홍색
	12 일반확성기
	*/

	VIRTUALIZER1_START
	{
		SECURE_API(s_wvsprintfA)(data, fmt, (va_list) ((unsigned int) &fmt + sizeof(void *)));
		((void(__cdecl *)(const char **, int))pt_chatlog)((p = data, &p), type);
	}
		VIRTUALIZER_END
}

void send_packet(const void *data, unsigned int size)
{
	struct {
		unsigned int zero;
		const void *data;
		unsigned int size;
	} param;

	/*
	어떤 쓰래드에서 보냈나 체크를 하기 때문에 막 보내면 영정익스프레스임
	[send함수를 dump해서 쓰던지, CRC처리를 하고 직접 수정을 가하던지, 메인쓰래드에서 패킷을 보내던지]
	내가 패킷 보낼 때는 메인쓰래드를 통해서 보내니까 상관없는듯ㅋ
	*/

	param.zero = 0;
	param.data = data;
	param.size = size;

	((void(__cdecl *)(void *))pt_sendpacket)(&param);
}

static unsigned int version = 192;
static unsigned int pt_MakeBufferList = 0x84ACA0u;
static unsigned int pt_CClientSocket = 0x14096E4u, pt_Flush = 0x500330u, pt_bufferlist = 84u, pt_sendiv = 140u, pt_recviv = 144u;
static unsigned int pt_CSecurityClient = 0x140A140u, pt_OnSendPacket = 0xF50250u;

void custom_send_packet(const void *data, unsigned int size)
{
	static unsigned char buffer[68] = "\x40\x00\x00\x00";
	unsigned int i, j;
	struct {
		unsigned int zero;
		const void *data;
		unsigned int size;
	} param1, param2;

	dbg("%s", "custom_send_packet");

	param1.zero = 0;
	param1.data = data;
	param1.size = size;

	param2.zero = 0;
	param2.data = &buffer[4];
	param2.size = 0;

	if ((i = *(unsigned int *) pt_CClientSocket) != 0) {
		if ((j = *(unsigned int *) pt_CSecurityClient) != 0)
			while (thiscall2((void *) pt_OnSendPacket, (void *) j, *(void **) (i + pt_sendiv), &param2)) {
				thiscall5((void *) pt_MakeBufferList, &param2, (void *) (i + pt_bufferlist), (void *) version, (void *) (i + pt_sendiv), (void *) 1, *(void **) (i + pt_sendiv));
				*(unsigned int *) (i + pt_sendiv) = wzcrypt_nextiv(*(unsigned int *) (i + pt_sendiv));
			}
		thiscall5((void *) pt_MakeBufferList, &param1, (void *) (i + pt_bufferlist), (void *) version, (void *) (i + pt_sendiv), (void *) 1, *(void **) (i + pt_sendiv));
		*(unsigned int *) (i + pt_sendiv) = wzcrypt_nextiv(*(unsigned int *) (i + pt_sendiv));
		thiscall0((void *) pt_Flush, (void *) i);
	}

	dbg("%s", "custom_send_packet retn");
}

void update_title(HWND hwnd)
{
	unsigned int size;
	wchar_t data[512];

	VIRTUALIZER1_START
	{
		size = _wcscpy(data, L"MapleStory");

		if (lan_mode)
			size += _wcscpy(&data[size], L" (LAN)");

		if (signal_mode)
			if (signal_mode == 2)
				size += _wcscpy(&data[size], L" (SIGNAL+)");
			else
				size += _wcscpy(&data[size], L" (SIGNAL)");

		if (wari_mode)
			size += _wcscpy(&data[size], L" (WARI)");

		if (cswari_mode)
			size += _wcscpy(&data[size], L" (CSWARI)");

		if (shop_mode)
			size += _wcscpy(&data[size], L" (SHOP)");

		if (login_mode)
			size += _wcscpy(&data[size], L" (LOGIN)");

		if (timer_mode)
			size += _wcscpy(&data[size], L" (TIMER)");

		if (mouse_mode)
			size += _wcscpy(&data[size], L" (MOUSE)");

		SECURE_API(s_SetWindowTextW)(hwnd, data);
	}
		VIRTUALIZER_END
}

////////////////////////////////////////////////////////////////////////////////

unsigned int CEmplyeePool_GetAt(unsigned int id)
{
	unsigned int i, j, k;

	if ((i = *(unsigned int *) pt_CEmployeePool) != 0) {
		i += 4u; /* ZMap<unsigned long, POSITION> */
		if ((j = *(unsigned int *) (i + 4u)) != 0)
			for (k = *(unsigned int *) (j + ((_rotr(id, 5) % *(unsigned int *) (i + 8u)) << 2)); k; k = *(unsigned int *) (k + 4u))
				if (*(unsigned int *) (k + 8u) == id)
					return *(unsigned int *) (*(unsigned int *) (k + 12u) + 16u);
	}

	return 0;
}

void CEmplyeePool_ZMap_Dump(void)
{
	unsigned int i, j, k, l;

	if ((i = *(unsigned int *) pt_CEmployeePool) != 0) {
		i += 4u; /* ZMap<unsigned long, POSITION> */
		dbg("CEmplyeePool_ZMap: array=%p size=%u", *(unsigned int *) (i + 4u), *(unsigned int *) (i + 8u));
		if ((j = *(unsigned int *) (i + 4u)) != 0)
			for (k = j + (*(unsigned int *) (i + 8u) << 2); j < k; j += 4u)
				for (l = *(unsigned int *) j; l; l = *(unsigned int *) (l + 4u))
					dbg("DATA %p (ID %p) [%p]", *(unsigned int *) (*(unsigned int *) (l + 12u) + 16u), *(unsigned int *) (l + 8u), CEmplyeePool_GetAt(*(unsigned int *) (l + 8u)));
	}
}

void CEmplyeePool_ZList_Dump(void)
{
	unsigned int i, j;

	if ((i = *(unsigned int *) pt_CEmployeePool) != 0) {
		i += 28u; /* ZList<EMPLOYEEENTRY> */
		dbg("CEmplyeePool_ZList: list=%p size=%u", *(unsigned int *) (i + 12u), *(unsigned int *) (i + 8u));
		for (j = *(unsigned int *) (i + 12u); j; j = *(unsigned int *) (j + 4u))
			dbg("DATA %p", *(unsigned int *) (j + 16u));
	}
}

unsigned int CUserPool_GetAt(unsigned int id)
{
	unsigned int i, j, k;

	if ((i = *(unsigned int *) pt_CUserPool) != 0) {
		i += 12u; /* ZMap<unsigned long, ZRef<USERREMOTE_ENTRY>> */
		if ((j = *(unsigned int *) (i + 4u)) != 0)
			for (k = *(unsigned int *) (j + ((_rotr(id, 5) % *(unsigned int *) (i + 8u)) << 2)); k; k = *(unsigned int *) (k + 4u))
				if (*(unsigned int *) (k + 8u) == id)
					return *(unsigned int *) (*(unsigned int *) (k + 16u) + 16u);
	}

	return 0;
}

void CUserPool_ZMap_Dump(void)
{
	unsigned int i, j, k, l;

	if ((i = *(unsigned int *) pt_CUserPool) != 0) {
		i += 12u; /* ZMap<unsigned long, ZRef<USERREMOTE_ENTRY>> */
		dbg("CUserPool_ZMap: array=%p size=%u", *(unsigned int *) (i + 4u), *(unsigned int *) (i + 8u));
		if ((j = *(unsigned int *) (i + 4u)) != 0)
			for (k = j + (*(unsigned int *) (i + 8u) << 2); j < k; j += 4u)
				for (l = *(unsigned int *) j; l; l = *(unsigned int *) (l + 4u))
					dbg("DATA %p (ID %p) [%p]", *(unsigned int *) (*(unsigned int *) (l + 16u) + 16u), *(unsigned int *) (l + 8u), CUserPool_GetAt(*(unsigned int *) (l + 8u)));
	}
}

void CUserPool_ZList_Dump(void)
{
	unsigned int i, j;

	if ((i = *(unsigned int *) pt_CUserPool) != 0) {
		i += 36u; /* ZList<ZRef<USERREMOTE_ENTRY>> */
		dbg("CUserPool_ZList: list=%p size=%u", *(unsigned int *) (i + 12u), *(unsigned int *) (i + 8u));
		for (j = *(unsigned int *) (i + 12u), j = j ? (j - 16u) : 0; j; j = *(unsigned int *) (j + 4u))
			dbg("DATA %p", *(unsigned int *) (*(unsigned int *) (j + 20u) + 16u));
	}
}

void DumpShopObject(void)
{
	unsigned int i, j, k;

	CEmplyeePool_ZMap_Dump();
	CEmplyeePool_ZList_Dump();

	if ((i = *(unsigned int *) pt_CEmployeePool) != 0) {
		dbg("------ ENTRUSTED SHOP LIST (%u) ------", *(unsigned int *) (i + 36u));
		for (j = *(unsigned int *) (i + 40u); j; j = *(unsigned int *) (j + 4)) {
			k = *(unsigned int *) (j + 16u);
			dbg("%p %08X (%d,%d) [OID:%08X]", k, *(unsigned int *) (k + pt_eshopid), *(int *) (k + pt_eshopx), *(int *) (k + pt_eshopy), *(unsigned int *) (k + pt_eshopid + 12u));
		}
	}

	if ((i = *(unsigned int *) pt_CUserPool) != 0) {
		dbg("------ PERSONAL SHOP LIST (%u) ------", *(unsigned int *) (i + 44u));
		for (j = *(unsigned int *) (i + 48u); j; j += 16u) {
			k = *(unsigned int *) (*(unsigned int *) (j + 4u) + 16u);
			dbg("%p %08X [%d] (%d,%d) [UID:%08X]",
				k,
				*(unsigned int *) (k + pt_ushopid),
				*(unsigned int *) (k + pt_ushopmode),
				(int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_userx),
				(int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_usery),
				*(unsigned int *) (k + pt_userid));
			if ((j = *(unsigned int *) (j - 12u)) == 0)
				break;
		}
	}

	CUserPool_ZMap_Dump();
	CUserPool_ZList_Dump();
}

////////////////////////////////////////////////////////////////////////////////

void shop_addpos1(sect_t *sect, int v, int c)
{
	unsigned int i;

	if (sect->n < sizeof(sect->p) / sizeof(sect->p[0])) {
		for (i = 0; i < sect->n && i < sizeof(sect->p) / sizeof(sect->p[0]); ++i)
			if (sect->p[i].v <= v) { /* right priority */
				_memmove(&sect->p[i + 1u], &sect->p[i], (sect->n - i) * sizeof(sect->p[0]));
				break;
			}
		sect->p[i].v = v;
		sect->p[i].c = c;
		++sect->n;
	}
}

// 포옆, 1층, 2층, 3층, 1층<->계단1<->계단2, 계단1<->계단2<->언덕, 계단2<->언덕
void shop_addpos2(sect_t *sect, int x, int y, int c)
{
	if (-146 - 120 < y && y < -146 + 120) { // 계단2<->[언덕]
		if (683 - 130 < x) /* left limit */
			shop_addpos1(&sect[1], x, c);
	}
	if (-26 - 120 < y && y < -26 + 120) { // 1층<->[계단1]<->계단2
		if (503 - 130 < x) /* left limit */
			shop_addpos1(&sect[2], x, c);
	}
	if (-86 - 120 < y && y < -86 + 120) { // 계단1<->[계단2]<->언덕
		if (593 - 130 < x) /* left limit */
			shop_addpos1(&sect[3], x, c);
	}
	if (34 - 120 < y && y < 34 + 120) { // [1층]
		if (x <= 669) /* right limit */
			shop_addpos1(&sect[4], x, c);
		else // (포옆 -420~669, 910~960)
			shop_addpos1(&sect[0], x, c);
	}
	else if (-206 - 120 < y && y < -206 + 120) { // [2층]
		if (x < 487 + 130) /* right limit */
			shop_addpos1(&sect[5], x, c);
	}
	else if (-416 - 120 < y && y < -416 + 120) // [3층]
		shop_addpos1(&sect[6], x, c);
}

int shop_searchfreepos(int strict)
{
	// [포옆, 언덕, 계단1, 계단2, 1층, 2층, 3층] 순으로 검색
	static int ypos[7] = { 34, -146, -26, -86, 34, -206, -416 };
	unsigned int i, j, k, n_pos, s_pos;
	sect_t sect[7];

	for (i = 0; i < sizeof(sect) / sizeof(sect[0]); ++i)
		sect[i].n = 0;

	// 포옆 limit
	shop_addpos1(&sect[0], 910 - 130, -1);
	shop_addpos1(&sect[0], 960 + 130, -1);

	// 언덕 limit
	shop_addpos1(&sect[1], 683 - 130, -1);
	shop_addpos1(&sect[1], 960 + 130, -1);

	// 계단1 limit
	shop_addpos1(&sect[2], 503 - 130, -1);
	shop_addpos1(&sect[2], 592 + 130, -1);

	// 계단2 limit
	shop_addpos1(&sect[3], 593 - 130, -1);
	shop_addpos1(&sect[3], 682 + 130, -1);

	// 1층 limit (포옆 -420~669, 910~960)
	shop_addpos1(&sect[4], -420 - 130, -1);
	shop_addpos1(&sect[4], 669 + 130, -1);

	// 2층 limit
	shop_addpos1(&sect[5], -307 - 130, -1);
	shop_addpos1(&sect[5], 487 + 130, -1);

	// 3층 limit
	shop_addpos1(&sect[6], -262 - 130, -1);
	shop_addpos1(&sect[6], 802 + 130, -1);

	VIRTUALIZER1_START
	{
		if ((i = *(unsigned int *) pt_CEmployeePool) != 0) {
			i += 28u; /* ZList<EMPLOYEEENTRY> */
			for (j = *(unsigned int *) (i + 12u); j; j = *(unsigned int *) (j + 4u)) {
				k = *(unsigned int *) (j + 16u);
				shop_addpos2(sect, *(int *) (k + pt_eshopx), *(int *) (k + pt_eshopy), 0);
			}
		}

		if ((i = *(unsigned int *) pt_CUserPool) != 0) {
			i += 36u; /* ZList<ZRef<USERREMOTE_ENTRY>> */
			for (j = *(unsigned int *) (i + 12u), j = j ? (j - 16u) : 0; j; j = *(unsigned int *) (j + 4u)) {
				k = *(unsigned int *) (*(unsigned int *) (j + 20u) + 16u);
				if (*(unsigned int *) (k + pt_ushopmode) != 0)
					shop_addpos2(sect,
					(int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_userx),
					(int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_usery),
					0);
			}
		}
	}
		VIRTUALIZER_END

		for (i = 0; i < 7u; ++i) /* search space */
			for (j = 1u; j < sect[i].n; ++j) { /* right to left */
				if (strict) {
					if (sect[i].p[j - 1u].v - sect[i].p[j].v >= 260) // 상점을 펼 거리가 나오면
						shop_addpos1(&sect[i], sect[i].p[j - 1u].v - 130, 1); // 딱붙여 펴기
					continue;
				}
				if (sect[i].p[j - 1u].v - sect[i].p[j].v >= 260) // 상점을 펼 거리가 나오면서
					if (sect[i].p[j - 1u].v - sect[i].p[j].v >= 520) { // 더 펼수 있는 거리가 나오면
						if (sect[i].p[j - 1u].c == -1) // 한계점이면 슬쩍 더가서 먹기 ㅋㅋ
							shop_addpos1(&sect[i], sect[i].p[j - 1u].v - 130 - 128, 1); // 애매하게 딸리게 ㅋㅋ
						else
							shop_addpos1(&sect[i], sect[i].p[j - 1u].v - 130 - 128, 1); // 애매하게 딸리게
					}
					else
						shop_addpos1(&sect[i], sect[i].p[j - 1u].v - ((sect[i].p[j - 1u].v - sect[i].p[j].v) / 2), 1); // 가운데 먹는다
			}

	n_pos = 0;

	for (i = 0; i < 4u; ++i) /* sort position */
		for (j = 0; j < sect[i].n; ++j)
			if (sect[i].p[j].c == 1) {
				shop_pos[n_pos].v = sect[i].p[j].v;
				shop_pos[n_pos].c = ypos[i];
				++n_pos;
			}

	s_pos = n_pos;

	/* 나머지를 먹음 */
	for (i = 4u; i < 6u; ++i) /* sort position */
		for (j = 0; j < sect[i].n; ++j) {
			if (sect[i].p[j].c == 1) {
				for (k = s_pos; k < n_pos; ++k)
					if (shop_pos[k].v <= sect[i].p[j].v) { /* right priority */
						if (i == 5 && (sect[i].p[j].v - shop_pos[k].v) < 130) // 2층이고, 거리차이가 많이 안나는경우 1층이 우선됨.
							continue;
						_memmove(&shop_pos[k + 1u], &shop_pos[k], (n_pos - k) * sizeof(shop_pos[0]));
						break;
					}
				shop_pos[k].v = sect[i].p[j].v;
				shop_pos[k].c = ypos[i];
				++n_pos;
			}
		}

	for (i = 6u; i < 7u; ++i) /* sort position */
		for (j = 0; j < sect[i].n; ++j)
			if (sect[i].p[j].c == 1) {
				shop_pos[n_pos].v = sect[i].p[j].v;
				shop_pos[n_pos].c = ypos[i];
				++n_pos;
			}

	shop_pos_idx = 0;
	shop_pos_num = n_pos;

	return n_pos != 0;
}

int shop_warpfreepos(HWND hwnd)
{
	int err = ~0;
	unsigned int i;

	VIRTUALIZER1_START
	{
		if (((SECURE_API(s_GetKeyState)(VK_CONTROL) & 0x8000) || shop_pos_idx >= shop_pos_num) && shop_searchfreepos(0) == 0)
		msg(10, "%s", "상점을 펼 자리가 없음");
		else if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
			i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
			thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) shop_pos[shop_pos_idx].v, (void *) shop_pos[shop_pos_idx].c);
			++shop_pos_idx;
			err = 0;
		}
	}
		VIRTUALIZER_END
		return err;
}

int shop_search(unsigned int type, unsigned int id, int *x, int *y)
{
	unsigned int i, j, k, l;

	if (type) {
		if ((i = *(unsigned int *) pt_CEmployeePool) != 0) {
			i += 4u; /* ZMap<unsigned long, POSITION> */
			if ((j = *(unsigned int *) (i + 4u)) != 0)
				for (k = *(unsigned int *) (j + ((_rotr(id, 5) % *(unsigned int *) (i + 8u)) << 2)); k; k = *(unsigned int *) (k + 4u))
					if (*(unsigned int *) (k + 8u) == id) {
						l = *(unsigned int *) (*(unsigned int *) (k + 12u) + 16u);
						*x = *(int *) (l + pt_eshopx);
						*y = *(int *) (l + pt_eshopy);
						return 0;
					}
		}
	}
	else if ((i = *(unsigned int *) pt_CUserPool) != 0) {
		i += 12u; /* ZMap<unsigned long, ZRef<USERREMOTE_ENTRY>> */
		if ((j = *(unsigned int *) (i + 4u)) != 0)
			for (k = *(unsigned int *) (j + ((_rotr(id, 5) % *(unsigned int *) (i + 8u)) << 2)); k; k = *(unsigned int *) (k + 4u))
				if (*(unsigned int *) (k + 8u) == id) {
					l = *(unsigned int *) (*(unsigned int *) (k + 16u) + 16u);
					*x = (int) *(double *) (*(unsigned int *) (l + pt_uservec) - 12u + 16u + pt_userx);
					*y = (int) *(double *) (*(unsigned int *) (l + pt_uservec) - 12u + 16u + pt_usery);
					return *(unsigned int *) (l + pt_ushopmode) != 5;
				}
	}

	return -1;
}

int shop_warpnearest(int v)
{
	int err = ~0;
	unsigned int i, j, k;
	unsigned char data[16];

	VIRTUALIZER1_START
	{
		if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
			thiscall1(*(void **) (*(unsigned int *) (i + 4u) + pt_getpos), (void *) (i + 4u), (void *) data);
			if (v == 0) {
				if ((i = *(unsigned int *) pt_CEmployeePool) != 0) {
					i += 28u; /* ZList<EMPLOYEEENTRY> */
					for (j = *(unsigned int *) (i + 12u); j; j = *(unsigned int *) (j + 4u)) {
						k = *(unsigned int *) (j + 16u);
						if (*(int *) &data[4] == *(int *) (k + pt_eshopy) && abs(*(int *) &data[0] - *(int *) (k + pt_eshopx)) < 65) {
							i = *(unsigned int *) pt_CUserLocal;
							i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
							thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) *(int *) (k + pt_eshopx), (void *) *(int *) (k + pt_eshopy));
							err = 0;
							goto $exit;
						}
					}
				}
				if ((i = *(unsigned int *) pt_CUserPool) != 0) {
					i += 36u; /* ZList<ZRef<USERREMOTE_ENTRY>> */
					for (j = *(unsigned int *) (i + 12u), j = j ? (j - 16u) : 0; j; j = *(unsigned int *) (j + 4u)) {
						k = *(unsigned int *) (*(unsigned int *) (j + 20u) + 16u);
						if (*(unsigned int *) (k + pt_ushopmode) == 5 && *(int *) &data[4] == (int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_usery) && abs(*(int *) &data[0] - (int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_userx)) < 65) {
							i = *(unsigned int *) pt_CUserLocal;
							i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
							thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) (int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_userx), (void *) (int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_usery));
							err = 0;
							goto $exit;
						}
					}
				}
				msg(10, "%s", "가까운 상점이 없음");
			}
			else {
				i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
				thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) (*(int *) &data[0] + v), (void *) *(int *) &data[4]);
				err = 0;
			}
		}
	$exit:;
	}
		VIRTUALIZER_END
		return err;
}

////////////////////////////////////////////////////////////////////////////////

int SendShopWariPacket(HWND hwnd);

VOID CALLBACK SendShopWariPacketSub(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	static unsigned int counter;
	unsigned int i;

	VIRTUALIZER1_START
	{
		if ((i = *(unsigned int *) pt_CUniqueModeless) != 0) {
			if (*(unsigned int *) (i + pt_shoptype) != 0)
				thiscall2(*(void **) (*(unsigned int *) (i + 4u) + 0u), (void *) (i + 4u), (void *) VK_ESCAPE, (void *) 0);
			counter = 0;
			SECURE_API(s_KillTimer)(hwnd, idTimer);
			msg(10, "%s", "와리 성공");
		}
		else if (wari_mode == 0 || counter == 32u || SendShopWariPacket(hwnd)) {
			SECURE_API(s_KillTimer)(hwnd, idTimer);
			msg(10, "%s", "와리 실패");
		}
		else
			++counter;
	}
		VIRTUALIZER_END
}

int SendShopWariPacket(HWND hwnd)
{
	static unsigned int counter;
	int err = ~0;
	unsigned int i, j, k, n, shop[64];
	SYSTEMTIME now;

	VIRTUALIZER1_START
	{
		if ((i = *(unsigned int *) pt_CUniqueModeless) != 0) {
			if (*(unsigned int *) (i + pt_shoptype) == 0) {
				msg(10, "%s", "상점이 개설된 상태라 와리 안함");
				goto $exit;
			}
			thiscall2(*(void **) (*(unsigned int *) (i + 4u) + 0u), (void *) (i + 4u), (void *) VK_ESCAPE, (void *) 0);
		}

		n = 0;

		if ((i = *(unsigned int *) pt_CEmployeePool) != 0) {
			i += 28u; /* ZList<EMPLOYEEENTRY> */
			for (j = *(unsigned int *) (i + 12u); j; j = *(unsigned int *) (j + 4u)) {
				k = *(unsigned int *) (j + 16u);
				if (n == sizeof(shop) / sizeof(shop[0]))
					break;
				if (*(unsigned int *) (*(unsigned int *) pt_CWvsContext + pt_accid) != *(unsigned int *) (k + pt_eshopid))
					shop[n++] = *(unsigned int *) (k + pt_eshopid + 12u);
			}
		}

		if ((i = *(unsigned int *) pt_CUserPool) != 0) {
			i += 36u; /* ZList<ZRef<USERREMOTE_ENTRY>> */
			for (j = *(unsigned int *) (i + 12u), j = j ? (j - 16u) : 0; j; j = *(unsigned int *) (j + 4u)) {
				k = *(unsigned int *) (*(unsigned int *) (j + 20u) + 16u);
				if (*(unsigned int *) (k + pt_ushopmode) == 5) {
					if (n == sizeof(shop) / sizeof(shop[0]))
						break;
					shop[n++] = *(unsigned int *) (k + pt_ushopid);
				}
			}
		}

		if (n != 0) {
			/* enter */
			*(unsigned __int32 *) &packet[5].data[3] = shop[++counter % n];
			send_packet(packet[5].data, packet[5].size);

			/* close */
			send_packet(packet[0].data, packet[0].size);

			/* close sub */
			SECURE_API(s_SetTimer)(hwnd, kTimerWariClose, 400, SendShopWariPacketSub);

			/* message */
			SECURE_API(s_GetLocalTime)(&now);
			msg(10, "[%02u:%02u:%02u] 와리", now.wHour, now.wMinute, now.wSecond);

			/* success */
			err = 0;
		}
		else
			msg(10, "%s", "와리할 상점이 없음");

	$exit:;
	}
		VIRTUALIZER_END
		return err;
}

VOID CALLBACK DoShopWariSub(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	VIRTUALIZER1_START
	{
		SendShopWariPacket(hwnd);
	}
		VIRTUALIZER_END
}

void DoShopWari(HWND hwnd)
{
	VIRTUALIZER1_START
	{
		if (wari_mode == 0) {
			wari_mode = 1;
			SECURE_API(s_SetTimer)(hwnd, kTimerWari, 30 * 60 * 1000, DoShopWariSub);
			update_title(hwnd);
			msg(10, "%s", "와리 시작");
		}
		else {
			wari_mode = 0;
			SECURE_API(s_KillTimer)(hwnd, kTimerWari);
			update_title(hwnd);
			msg(10, "%s", "와리 종료");
		}
	}
		VIRTUALIZER_END
}

////////////////////////////////////////////////////////////////////////////////

VOID CALLBACK DoCSWariSub(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	static DWORD dwLastTime;
	static unsigned char data[16];
	unsigned int i;

	if (cswari_mode == 0xFFFFFFFFu) {
		VIRTUALIZER1_START
		{
			cswari_mode = 0;
			SECURE_API(s_KillTimer)(hwnd, idTimer);
			update_title(hwnd);
			msg(10, "%s", "캐시샵 와리 종료");
		}
			VIRTUALIZER_END
	}
	else
		switch (cswari_mode) {
		case 1:
			VIRTUALIZER1_START
			{
				if ((i = *(unsigned int *) pt_CUIStatusBar) != 0) {
					cswari_mode = 2;
					dwLastTime = dwTime;
					thiscall1(*(void **) (*(unsigned int *) i + 32u), (void *) i, (void *) 1000);
					if ((i = *(unsigned int *) pt_CUserLocal) != 0)
						thiscall1(*(void **) (*(unsigned int *) (i + 4u) + pt_getpos), (void *) (i + 4u), (void *) data);
					msg(10, "%s", "캐시샵 입장 시도");
				}
				else {
					cswari_mode = 3;
					DoCSWariSub(hwnd, message, idTimer, dwTime);
				}
			}
				VIRTUALIZER_END
				break;

		case 2:
			VIRTUALIZER1_START
			{
				if ((i = *(unsigned int *) pt_CUIStatusBar) != 0) {
					if ((int) dwTime - (int) dwLastTime >= 500) {
						cswari_mode = 1;
						DoCSWariSub(hwnd, message, idTimer, dwTime);
					}
				}
				else {
					cswari_mode = 3;
					DoCSWariSub(hwnd, message, idTimer, dwTime);
				}
			}
				VIRTUALIZER_END
				break;

		case 3:
			VIRTUALIZER1_START
			{
				if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
					i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
					thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, *(void **) &data[0], *(void **) &data[4]);
					cswari_mode = 0xFFFFFFFFu;
					DoCSWariSub(hwnd, message, idTimer, dwTime);
				}
				else
					mouseinput1(hwnd, 5, 728);
			}
				VIRTUALIZER_END
				break;
	}
}

void DoCSWari(HWND hwnd)
{
	VIRTUALIZER1_START
	{
		if (cswari_mode == 0) {
			cswari_mode = 1;
			SECURE_API(s_SetTimer)(hwnd, kTimerCSWari, 100, DoCSWariSub);
			update_title(hwnd);
			msg(10, "%s", "캐시샵 와리 시작");
		}
		else {
			cswari_mode = 0xFFFFFFFFu;
			msg(10, "%s", "캐시샵 와리 취소");
		}
	}
		VIRTUALIZER_END
}

////////////////////////////////////////////////////////////////////////////////

void PrepareShopPacket(void)
{
	static const char *shop_name = "　";
	unsigned int i;

	// 오목팅 (100메소 이상 소지)
	// F500 10 02 010000 00 03

	VIRTUALIZER1_START
	{
		/* close */
		// 68 de 00 00 00 8d 4c 24 ? e8 ? ? ? ? 6a ? 8d 4c 24 ? c7
		packet[0].size = 3;
		*(unsigned __int16 *) &packet[0].data[0] = (unsigned __int16) S_MINIROOM;
		*(unsigned __int8  *) &packet[0].data[2] = (unsigned __int8) S_MINIROOM_LEAVE;

		/* open */
		// op코드(&data[2])만 참조 -> 68 de 00 00 00 8d 4c 24 ? e8 ? ? ? ? 6a ? 8d 4c 24 ? c7 44 24 ? 06 ? ? ? e8
		packet[1].size = (i = strlen(shop_name)) + 13u;
		*(unsigned __int16 *) &packet[1].data[0] = (unsigned __int16) S_MINIROOM;
		*(unsigned __int8  *) &packet[1].data[2] = (unsigned __int8) S_MINIROOM_CREATE;
		*(unsigned __int8  *) &packet[1].data[3] = 5;
		*(unsigned __int16 *) &packet[1].data[4] = (unsigned __int16) i;
		memcpy(&packet[1].data[6], shop_name, i);
		*(unsigned __int8  *) &packet[1].data[i + 6u] = 0; /* category */
		*(unsigned __int16 *) &packet[1].data[i + 7u] = 1; /* slot */
		*(unsigned __int32 *) &packet[1].data[i + 9u] = 5140000u; /* itemid */

		/* additem */
		// 68 ? ? ? ? 8d 4c 24 ? e8 ? ? ? ff 8b 06
		packet[2].size = 18;
		*(unsigned __int16 *) &packet[2].data[0] = (unsigned __int16) S_MINIROOM;
		*(unsigned __int8  *) &packet[2].data[2] = (unsigned __int8) S_MINIROOM_ADDITEM;
		*(unsigned __int8  *) &packet[2].data[3] = 1; /* category */
		*(unsigned __int16 *) &packet[2].data[4] = 1; /* slot */
		*(unsigned __int16 *) &packet[2].data[6] = 1;
		*(unsigned __int16 *) &packet[2].data[8] = 1;
		*(unsigned __int64 *) &packet[2].data[10] = 9999999999; /* price (9,999,999,999) */

		/* unknown */
		packet[3].size = 5;
		*(unsigned __int16 *) &packet[3].data[0] = (unsigned __int16) S_MINIROOM;
		*(unsigned __int8  *) &packet[3].data[2] = (unsigned __int8) S_MINIROOM_START1;
		*(unsigned __int16 *) &packet[3].data[3] = 0;

		/* start */
		// 68 de 00 00 00 8d 4c 24 ? e8 ? ? ? ? 6a ? 8d 4c 24 ? 89
		packet[4].size = 4;
		*(unsigned __int16 *) &packet[4].data[0] = (unsigned __int16) S_MINIROOM;
		*(unsigned __int8  *) &packet[4].data[2] = (unsigned __int8) S_MINIROOM_START2;
		*(unsigned __int8  *) &packet[4].data[3] = 1;

		/* enter */
		packet[5].size = 9;
		*(unsigned __int16 *) &packet[5].data[0] = (unsigned __int16) S_MINIROOM;
		*(unsigned __int8  *) &packet[5].data[2] = (unsigned __int8) S_MINIROOM_ENTER;
		*(unsigned __int32 *) &packet[5].data[3] = 0;
		*(unsigned __int16 *) &packet[5].data[7] = 0;
	}
		VIRTUALIZER_END
}

void SendShopPacket(int close)
{
	static unsigned int counter;
	unsigned int i;

	/* close */
	if ((i = *(unsigned int *) pt_CUniqueModeless) != 0)
		thiscall2(*(void **) (*(unsigned int *) (i + 4u) + 0u), (void *) (i + 4u), (void *) VK_ESCAPE, (void *) 0);
	else if (close)
		send_packet(packet[0].data, packet[0].size);

	/* open */
	send_packet(packet[1].data, packet[1].size);

	/* additem */
	*(unsigned __int8 *) &packet[2].data[2] = (unsigned __int8) ((++counter & 3) + S_MINIROOM_ADDITEM);
	send_packet(packet[2].data, packet[2].size);

	/* unknown */
	send_packet(packet[3].data, packet[3].size);

	/* start */
	send_packet(packet[4].data, packet[4].size);
}

void KillSignal(HWND hwnd)
{
	VIRTUALIZER1_START
	{
		signal_mode = 0;
		SECURE_API(s_SetPriorityClass)(SECURE_API(s_GetCurrentProcess)(), NORMAL_PRIORITY_CLASS);
		update_title(hwnd);
		msg(10, "%s", "신호 종료");
	}
		VIRTUALIZER_END
}

void DoNearSignal(HWND hwnd)
{
	VIRTUALIZER1_START
	{
		if (signal_mode != 1) {
			signal_mode = 1;
			SECURE_API(s_SetPriorityClass)(SECURE_API(s_GetCurrentProcess)(), HIGH_PRIORITY_CLASS);
			update_title(hwnd);
			msg(10, "%s", "신호 시작 [NEAR]");
		}
		else
			KillSignal(hwnd);
	}
		VIRTUALIZER_END
}

void DoWarpSignal(HWND hwnd)
{
	VIRTUALIZER1_START
	{
		if (signal_mode != 2) {
			signal_mode = 2;
			SECURE_API(s_SetPriorityClass)(SECURE_API(s_GetCurrentProcess)(), HIGH_PRIORITY_CLASS);
			update_title(hwnd);
			msg(10, "%s", "신호 시작 [WARP]");
		}
		else
			KillSignal(hwnd);
	}
		VIRTUALIZER_END
}

VOID CALLBACK OnShopCloseSub2(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	unsigned int i;

	VIRTUALIZER1_START
	{
		SECURE_API(s_KillTimer)(hwnd, idTimer);
		if ((i = *(unsigned int *) pt_CUniqueModeless) != 0 && *(unsigned int *) (i + pt_shoptype) == 0)
			SECURE_API(s_PlaySoundA)(MAKEINTRESOURCE(IDR_WAVE3), module_base, SND_ASYNC | SND_RESOURCE);
	}
		VIRTUALIZER_END
}

VOID CALLBACK OnShopCloseSub1(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	VIRTUALIZER1_START
	{
		SendShopPacket(0);
		SECURE_API(s_KillTimer)(hwnd, idTimer);
	}
		VIRTUALIZER_END
}

void OnShopClose(unsigned int type, unsigned int id) /* 여기서 메이플 내부 함수를 호출함에 있어, 소켓을 끊을만한 짓을 하는 함수를 호출하면 안됨 */
{
	unsigned int i;
	int x, y;
	unsigned char data[16];

	if (signal_mode && shop_search(type, id, &x, &y) == 0)
		if ((i = *(unsigned int *) pt_CUniqueModeless) == 0 || *(unsigned int *) (i + pt_shoptype) != 0) {
			if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
				thiscall1(*(void **) (*(unsigned int *) (i + 4u) + pt_getpos), (void *) (i + 4u), (void *) data);
				if (signal_mode == 2) {
					if (x != *(int *) &data[0] || y != *(int *) &data[4]) {
						VIRTUALIZER1_START
						{
							if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
								i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
								thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) x, (void *) y);
								SECURE_API(s_SetTimer)(MainWnd, kTimerShopCloseSub1, 500, OnShopCloseSub1);
							}
						}
							VIRTUALIZER_END
					}
					else
						SendShopPacket(0);
				}
				else if (y == *(int *) &data[4] && abs(x - *(int *) &data[0]) < 65)
					SendShopPacket(0);
			}
			VIRTUALIZER1_START
			{
				SECURE_API(s_SetTimer)(MainWnd, kTimerShopCloseSub2, 1000, OnShopCloseSub2);
				SECURE_API(s_GetLocalTime)((SYSTEMTIME *) data);
				msg(10, "[%02u:%02u:%02u.%03u] 신호 (%d,%d)", ((SYSTEMTIME *) data)->wHour, ((SYSTEMTIME *) data)->wMinute, ((SYSTEMTIME *) data)->wSecond, ((SYSTEMTIME *) data)->wMilliseconds, x, y);
			}
				VIRTUALIZER_END
		}
		else {
			VIRTUALIZER1_START
			{
				KillSignal(MainWnd);
			}
				VIRTUALIZER_END
		}
}

////////////////////////////////////////////////////////////////////////////////

VOID CALLBACK DoShopSub(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	static DWORD dwLastTime;
	static int warp_x, warp_y;
	unsigned int i;
	unsigned char data[16];

	VIRTUALIZER1_START
	{
		if ((i = *(unsigned int *) pt_CUniqueModeless) != 0)
		if (*(unsigned int *) (i + pt_shoptype) == 0) {
			shop_mode = 0xFFFFFFFFu;
			msg(10, "%s", "상점이 개설됨");
		}
		else
			thiscall2(*(void **) (*(unsigned int *) (i + 4u) + 0u), (void *) (i + 4u), (void *) VK_ESCAPE, (void *) 0);
		if (shop_mode == 0xFFFFFFFFu || *(unsigned int *) pt_CUserLocal == 0) {
			shop_mode = 0;
			SECURE_API(s_KillTimer)(hwnd, idTimer);
			update_title(hwnd);
			msg(10, "%s", "상점 개설 종료");
		}
	}
		VIRTUALIZER_END

		switch (shop_mode) {
		case 1:
			VIRTUALIZER1_START
			{
				if (shop_pos_idx >= shop_pos_num && shop_searchfreepos(0) == 0) {
					shop_mode = 0xFFFFFFFFu;
					msg(10, "%s", "상점을 펼 자리가 없음");
					DoShopSub(hwnd, message, idTimer, dwTime);
				}
				else if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
					thiscall1(*(void **) (*(unsigned int *) (i + 4u) + pt_getpos), (void *) (i + 4u), (void *) data);
					warp_x = shop_pos[shop_pos_idx].v;
					warp_y = shop_pos[shop_pos_idx].c;
					if (*(int *) &data[0] == warp_x && *(int *) &data[4] == warp_y) {
						SendShopPacket(0);
						dwLastTime = dwTime;
						shop_mode = 3;
						msg(10, "%s", "상점 개설 시도");
					}
					else {
						i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
						thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) warp_x, (void *) warp_y);
						dwLastTime = dwTime;
						shop_mode = 2;
						msg(10, "%s", "빈 자리로 이동");
					}
				}
			}
				VIRTUALIZER_END
				break;

		case 2:
			VIRTUALIZER1_START
			{
				if ((int) dwTime - (int) dwLastTime >= 500) {
					SendShopPacket(0);
					dwLastTime = dwTime;
					shop_mode = 3;
					msg(10, "%s", "상점 개설 시도");
				}
			}
				VIRTUALIZER_END
				break;

		case 3:
			VIRTUALIZER1_START
			{
				if ((int) dwTime - (int) dwLastTime >= 400) {
					if (shop_searchfreepos(0)) {
						/* 투명 상점인경우 상점자리 스킵 */
						while (abs(warp_x - shop_pos[shop_pos_idx].v) < 130 && abs(warp_y - shop_pos[shop_pos_idx].c) < 65)
							if (++shop_pos_idx >= shop_pos_num) {
								shop_mode = 0xFFFFFFFFu;
								msg(10, "%s", "상점을 펼 자리가 없음");
								break;
							}
						if (shop_mode != 0xFFFFFFFFu) {
							shop_mode = 1;
							msg(10, "%s", "상점 개설 실패");
						}
					}
					else {
						shop_mode = 0xFFFFFFFFu;
						msg(10, "%s", "상점을 펼 자리가 없음");
					}
					DoShopSub(hwnd, message, idTimer, dwTime);
				}
			}
				VIRTUALIZER_END
				break;
	}
}

void DoShop(HWND hwnd)
{
	VIRTUALIZER1_START
	{
		if (shop_mode == 0) {
			shop_pos_idx = 0;
			shop_pos_num = 0;
			shop_mode = 1;
			SECURE_API(s_SetTimer)(hwnd, kTimerShop, 100, DoShopSub);
			update_title(hwnd);
			msg(10, "%s", "상점 개설 시작");
		}
		else {
			shop_mode = 0xFFFFFFFFu;
			msg(10, "%s", "상점 개설 취소");
		}
	}
		VIRTUALIZER_END
}

////////////////////////////////////////////////////////////////////////////////

VOID CALLBACK DoLoginSub(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
}

void DoLogin(HWND hwnd, int a)
{
	unsigned int i, j, k;

	if ((i = *(unsigned int *) pt_CStage) != 0) {
		// Title ==> 0
		// WorldSelect ==> 1
		// CharacterSelect ==> 2
		// dbg("STEP %u", *(unsigned int *)(i + 396u));
		//	if ((j = *(unsigned int *)(i + pt_worlditem)) != 0) { /* ZArray<CLogin::WORLDITEM> */
		//		dbg("WORLDITEMS %u", *(unsigned int *)(j - 4u));
		//		for (k = 0; k < *(unsigned int *)(j - 4u); ++k) {
		//			l = j + k * 28u;
		//			dbg("IDX[%u] #%u:%s", k, *(unsigned int *)l, *(char **)(l + 4u));
		//		}
		//	}

		if ((j = *(unsigned int *) pt_CUICharSelect) != 0) {
			if (*(unsigned int *) (i + pt_charidx) != 0xFFFFFFFFu)
				if ((k = *(unsigned int *) (i + pt_softkeyboard + 4u)) != 0) {
					keyboardinput2(hwnd, "    ");
					keyboardinput1(hwnd, VK_RETURN, 1);
					*(unsigned int *) (i + pt_charidx) = 0xFFFFFFFFu;
					//thiscall_arg1((void *)k, *(void **)(*(unsigned int *)k + 32u), (void *)1000);
					//thiscall_arg1((void *)k, *(void **)(*(unsigned int *)k + 32u), (void *)1000);
					//thiscall_arg1((void *)k, *(void **)(*(unsigned int *)k + 32u), (void *)1000);
					//thiscall_arg1((void *)k, *(void **)(*(unsigned int *)k + 32u), (void *)1000);
					//thiscall_arg1((void *)k, *(void **)(*(unsigned int *)k + 32u), (void *)1);
					// BtOK => 1
					// BtCancel => 2
					// 0~9 => 1000~1009
					// abc,ABC => 1010,1020
					// def,DEF => 1011,1021
					// ghi,GHI => 1012,1022
					// jkl,JKL => 1013,1023
					// mno,MNO => 1014,1024
					// pqr,PQR => 1015,1025
					// stu,STU => 1016,1026
					// vwx,VWX => 1017,1027
					// yz ,YZ  => 1018,1028
					// BtDel => 1030
					// BtNext => 1031
				}
				else {
					if (a == 0)
						SECURE_API(s_PostMessageW)(hwnd, 0xCA00, 0, 0);
					//*(unsigned int *)(i + pt_charidx) = 0;
					thiscall1(*(void **) (*(unsigned int *) j + 32u), (void *) j, (void *) 1000);
				}
		}
		else if ((j = *(unsigned int *) pt_CUIWorldSelect) != 0) {
			thiscall1(*(void **) (*(unsigned int *) j + 32u), (void *) j, (void *) (1000u + 1u));
			if ((j = *(unsigned int *) pt_CUIChannelSelect) != 0) {
				*(unsigned int *) (j + 256u) = 0;
				thiscall1(*(void **) (*(unsigned int *) j + 32u), (void *) j, (void *) 1000);
			}
		}
		else if ((j = *(unsigned int *) pt_CUITitle) != 0) {

		}
	}

	/*if (login_mode == 0) {
	login_mode = 1;
	SECURE_API(s_SetTimer)(hwnd, kTimerLogin, 100, DoLoginSub);
	update_title(hwnd);
	msg(10, "%s", "로그인 시작");
	} else {
	login_mode = 0;
	SECURE_API(s_KillTimer)(hwnd, kTimerWari);
	update_title(hwnd);
	msg(10, "%s", "로그인 종료");
	}*/
}

////////////////////////////////////////////////////////////////////////////////

unsigned __int64 rdtsc(void) {
	__asm rdtsc;
}

void DlgPacketProcSub(HWND hwnd)
{
	static unsigned int counter;
	unsigned int i, j;
	unsigned char *p, data[65536];

	*data = '\0';

	VIRTUALIZER1_START
	{
		SECURE_API(s_GetDlgItemTextA)(hwnd, IDC_EDIT1, data, sizeof(data));
	}
		VIRTUALIZER_END

		p = data;
	while (*p) {
		i = 0;
		do {
			switch (*p) {
			case '#':
				while (*++p && *p != '\n');
				break;

			case '*':
				++p;
				data[i++] = (unsigned char) rdtsc();
				break;

			case '.':
				++p;
				data[i++] = (unsigned char) (++counter);
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				j = (*p++ - '0');
				if ('0' <= *p && *p <= '9')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - '0'));
				else if ('A' <= *p && *p <= 'F')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - 'A' + 10u));
				else if ('a' <= *p && *p <= 'f')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - 'a' + 10u));
				break;

			case '@':
				++p;
				*(unsigned int *) &data[i] = (unsigned int) SECURE_API(s_GetTickCount)();
				i += 4;
				break;

			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				j = (*p++ - 'A' + 10u);
				if ('0' <= *p && *p <= '9')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - '0'));
				else if ('A' <= *p && *p <= 'F')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - 'A' + 10u));
				else if ('a' <= *p && *p <= 'f')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - 'a' + 10u));
				break;

			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				j = (*p++ - 'a' + 10u);
				if ('0' <= *p && *p <= '9')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - '0'));
				else if ('A' <= *p && *p <= 'F')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - 'A' + 10u));
				else if ('a' <= *p && *p <= 'f')
					data[i++] = (unsigned char) ((j << 4) | (*p++ - 'a' + 10u));
				break;

			default:
				++p;
				break;
			}
		} while (*p && *p != '\n');
		if (i > 1u) {
			VIRTUALIZER1_START
			{
				send_packet(data, i);
			}
				VIRTUALIZER_END
		}
	}
}

VOID CALLBACK PacketTimer(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	VIRTUALIZER1_START
	{
		DlgPacketProcSub(hwnd);
	}
		VIRTUALIZER_END
}

INT_PTR CALLBACK DlgPacketProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	unsigned int i;

	switch (uMsg) {
	case WM_CLOSE:
		VIRTUALIZER1_START
		{
			PacketWnd = NULL;
			SECURE_API(s_DestroyWindow)(hwnd);
			//EndDialog(hwnd, 0);
		}
			VIRTUALIZER_END
			break;

	case WM_INITDIALOG:
		VIRTUALIZER1_START
		{
			PacketWnd = hwnd;
			SECURE_API(s_SetDlgItemInt)(hwnd, IDC_EDIT2, 1000, 0);
			SECURE_API(s_ShowWindow)(hwnd, SW_SHOW);
		}
			VIRTUALIZER_END
			break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			VIRTUALIZER1_START
			{
				DlgPacketProcSub(hwnd);
			}
				VIRTUALIZER_END
				break;

		case IDC_CHECK1:
			VIRTUALIZER1_START
			{
				if (SECURE_API(s_SendDlgItemMessageW)(hwnd, IDC_CHECK1, BM_GETCHECK, 0, 0) != BST_UNCHECKED) {
					if ((i = (unsigned int) SECURE_API(s_GetDlgItemInt)(hwnd, IDC_EDIT2, NULL, 0)) == 0)
						i = 1;
					SECURE_API(s_SetTimer)(hwnd, kTimerPacketSpam, i, PacketTimer);
				}
				else
					SECURE_API(s_KillTimer)(hwnd, kTimerPacketSpam);
			}
				VIRTUALIZER_END
				break;
		}
		break;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

VOID CALLBACK DoEmptyWorkingSet(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	HANDLE process;
	SYSTEMTIME now;
	PROCESS_MEMORY_COUNTERS stat[2];

	VIRTUALIZER1_START
	{
		if ((process = (HANDLE) SECURE_API(s_GetCurrentProcess)()) != NULL) {
			memset(stat, 0, sizeof(stat));
			SECURE_API(s_GetProcessMemoryInfo)(process, &stat[0], sizeof(stat[0]));
			SECURE_API(s_SetProcessWorkingSetSize)(process, -1, -1);
			SECURE_API(s_GetProcessMemoryInfo)(process, &stat[1], sizeof(stat[1]));
			SECURE_API(s_GetLocalTime)(&now);
			msg(10, "[%02u:%02u:%02u] 메모리 정리 %uMB -> %uMB", now.wHour, now.wMinute, now.wSecond, stat[0].WorkingSetSize >> 20, stat[1].WorkingSetSize >> 20);
		}
	}
		VIRTUALIZER_END
}

////////////////////////////////////////////////////////////////////////////////

VOID CALLBACK DoMouseSub(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	unsigned int i;
	int x, y;

	VIRTUALIZER1_START
	{
		if ((i = *(unsigned int *) pt_CInputSystem) != 0 && (i = *(unsigned int *) (i + pt_mousevec)) != 0) {
			x = *(int *) (i + pt_mousex);
			y = *(int *) (i + pt_mousey);
			if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
				i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
				thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) x, (void *) y);
			}
		}
	}
		VIRTUALIZER_END
}

void DoMouse(HWND hwnd)
{
	VIRTUALIZER1_START
	{
		if (mouse_mode == 0) {
			mouse_mode = 1;
			//SECURE_API(s_SetTimer)(hwnd, kTimerMouse, 30, DoMouseSub);
			update_title(hwnd);
		}
		else {
			mouse_mode = 0;
			//SECURE_API(s_KillTimer)(hwnd, kTimerMouse);
			update_title(hwnd);
		}
	}
		VIRTUALIZER1_END
}

////////////////////////////////////////////////////////////////////////////////

VOID CALLBACK DoTimerSub1(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	static const int x1 = -420 + 65;
	static const int x2 = 960 - 65;
	static int x, y, z;
	unsigned int i;

	switch (z) {
	case 0:
		// →↓
		if ((x += 25) > x2) {
			x = x2;
			if ((y += 50) > -100) {
				y = -100;
				z = 3;
			}
			else
				z = 1;
		}
		break;
	case 1: // ←↓
		if ((x -= 25) < x1) {
			x = x1;
			if ((y += 50) > -100) {
				y = -100;
				z = 2;
			}
			else
				z = 0;
		}
		break;
	case 2: // →↑
		if ((x += 25) > x2) {
			x = x2;
			if ((y -= 50) < -700) {
				y = -700;
				z = 1;
			}
			else
				z = 3;
		}
		break;
	case 3: // ←↑
		if ((x -= 25) < x1) {
			x = x1;
			if ((y -= 50) < -700) {
				y = -700;
				z = 0;
			}
			else
				z = 2;
		}
		break;
	}

	VIRTUALIZER1_START
	{
		if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
			i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
			thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) x, (void *) y);
		}
	}
		VIRTUALIZER_END
}

VOID CALLBACK DoTimerSub2(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	unsigned int i, j, k;
	int x, y;

	VIRTUALIZER1_START
	{
		if ((i = *(unsigned int *) pt_CUserPool) != 0) {
			i += 36u; /* ZList<ZRef<USERREMOTE_ENTRY>> */
			for (j = *(unsigned int *) (i + 12u), j = j ? (j - 16u) : 0; j; j = *(unsigned int *) (j + 4u)) {
				k = *(unsigned int *) (*(unsigned int *) (j + 20u) + 16u);
				if (*(unsigned int *) (k + pt_userid) == 0x0122E626u) {
					x = (int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_userx);
					y = (int) *(double *) (*(unsigned int *) (k + pt_uservec) - 12u + 16u + pt_usery);
					if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
						i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
						thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) x, (void *) y);
					}
					break;
				}
			}
		}
	}
		VIRTUALIZER_END
}

void DoTimer(HWND hwnd)
{
	VIRTUALIZER1_START
	{
		if (timer_mode == 0) {
			timer_mode = 1;
			SECURE_API(s_SetTimer)(hwnd, kTimerWarp, 30, DoTimerSub1);
			update_title(hwnd);
		}
		else {
			timer_mode = 0;
			SECURE_API(s_KillTimer)(hwnd, kTimerWarp);
			update_title(hwnd);
		}
	}
		VIRTUALIZER1_END
}

////////////////////////////////////////////////////////////////////////////////

void MyFunc(void)
{
	//	unsigned int i;

	//	for (i = 0; i < 13; ++i)
	//		msg(i, "색%02u 가나다라마바사아자차카타파하", i);

	//	if ((i = *(unsigned int *)pt_CUniqueModeless) != 0)
	//		msg(10, "상점DATA [%p][%p]", *(unsigned int *)(i + 228u), *(unsigned int *)(i + 232u));
	//	else
	//		msg(10, "저기요님아상점이열려있지않은데요;");

	//	if ((i = *(unsigned int *)0x13C22D8) != 0) /* CUIFarm */
	//		thiscall_arg1((void *)(i + 8u), *(void **)(*(unsigned int *)(i + 8u) + 0u), (void *)0);

	//	if ((i = *(unsigned int *)0x13CC748) != 0) /* CUIFarm */
	//		thiscall_arg0((void *)(i + 0u), *(void **)(*(unsigned int *)(i + 0u) + 16u));

	// 미니맵증발
	//	i = *(unsigned int *)0x13B9114;
	//	*(unsigned int *)(i + 22576u) = 0;

	//	i = *(unsigned int *)0x13B9114; /* CWvsContext */
	//	for (j = 0; j < 256; ++j)
	//		thiscall_arg1((void *)i, (void *)0xEB8AD0, (void *)j);
	//	thiscall_arg1((void *)i, (void *)0xEB8AD0, (void *)133);
	//	*(unsigned int *)(i + 22564u) = (unsigned int)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

	//unsigned int i;
	//	unsigned int g_pState = 0x13C2338;
	//	unsigned int CStage = *(unsigned int *)(g_pState + 4u);

	//42D570
	//void __thiscall ZXString_char___Assign(void *this, const char *a2, unsigned int a3)
	// 344 352
	//this + 52
	//i = *(unsigned int *)0x13C0C58; /* CUITitle */
	//thiscall_arg2((void *)(*(unsigned int *)(i + 344u) + 52u), (void *)0x42D570, (void *)"", (void *)-1);
	//thiscall_arg2((void *)(*(unsigned int *)(i + 352u) + 52u), (void *)0x42D570, (void *)"", (void *)-1);
	//thiscall_arg2((void *)(i + 4u), *(void **)(*(unsigned int *)(i + 4u) + 0u), (void *)13, (void *)0);

	//	i = *(unsigned int *)0x13C0C5C; /* CUIWorldSelect */
	//	thiscall_arg1((void *)i, *(void **)(*(unsigned int *)i + 32u), (void *)(((unsigned int)SECURE_API(s_GetTickCount)() % 32) + 1000u));

	//	i = *(unsigned int *)0x13C0C60; /* CUIChannelSelect */
	//	thiscall_arg1((void *)i, (void *)0x72A490, (void *)0);

	//i = *(unsigned int *)0x13B9114; /* CWvsContext */
	//thiscall_arg0((void *)i, (void *)0xED2D80);

	//	if (GetKeyState(VK_SHIFT) & 0x8000)
	//		*(unsigned int *)(CStage + 396u) = (*(unsigned int *)(CStage + 396u) - 1u) % 5;
	//	else
	//		*(unsigned int *)(CStage + 396u) = (*(unsigned int *)(CStage + 396u) + 1u) % 5;
	//	dbg("Step = %u", *(unsigned int *)(CStage + 396u));

	//	*(unsigned int *)(CStage + 392u) = *(unsigned int *)(*(unsigned int *)0x13B948C + 24u);
	//	*(unsigned int *)(CStage + 400u) = *(unsigned int *)(*(unsigned int *)0x13B948C + 24u);

	//((int (__cdecl *)(int, int))0x707E30)(53,0);
}

LRESULT CALLBACK MyMainWndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	unsigned int i;
	HMENU menu;
	unsigned char data[16];

	__try {
		if (Msg != WM_SOCKET)
			if (Msg >= WM_MOUSEFIRST && Msg <= WM_MOUSELAST) {
				if (Msg == WM_MOUSEWHEEL) {
					SECURE_API(s_ClientToScreen)(hwnd, (memset(data, 0, sizeof(POINT)), (POINT *) data));
					mouse_pos = (lParam = MAKELONG((long) LOWORD(lParam) - ((POINT *) data)->x, (long) HIWORD(lParam) - ((POINT *) data)->y));
				}
				else
					mouse_pos = lParam;
			}
			else if (Msg >= WM_KEYFIRST && Msg <= WM_KEYLAST) {
				if (Msg == WM_KEYDOWN || Msg == WM_SYSKEYDOWN) {
					switch (wParam) {
					case VK_RETURN:
						if (SECURE_API(s_GetKeyState)(VK_MENU) & 0x8000)
							__leave;
						break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						if ((SECURE_API(s_GetKeyState)(VK_CONTROL) & 0x8000)) {
							VIRTUALIZER1_START
							{
								if (SECURE_API(s_GetKeyState)(VK_SHIFT) & 0x8000) {
									if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
										thiscall1(*(void **) (*(unsigned int *) (i + 4u) + pt_getpos), (void *) (i + 4u), (void *) &save_pos[wParam - '0']);
										msg(10, "%s #%u (%d,%d)", "좌표 설정", wParam - '0', save_pos[wParam - '0'].v, save_pos[wParam - '0'].c);
									}
								}
								else if ((i = *(unsigned int *) pt_CUserLocal) != 0) {
									i = (unsigned int) thiscall0(*(void **) (*(unsigned int *) (i + 4u) + pt_getvecctrl), (void *) (i + 4u));
									thiscall3(*(void **) (*(unsigned int *) i + pt_rawmove), (void *) i, (void *) 0, (void *) save_pos[wParam - '0'].v, (void *) save_pos[wParam - '0'].c);
									msg(10, "%s #%u (%d,%d)", "좌표 이동", wParam - '0', save_pos[wParam - '0'].v, save_pos[wParam - '0'].c);
								}
							}
								VIRTUALIZER_END
								__leave;
						}
						break;
					case VK_F1:
						//DumpShopObject();
						//MyFunc();
						DoLogin(hwnd, 0);
						break;
					case VK_F2:
						//custom_send_packet("\x6C\x00\x64\x00\x00\x00\x01\x01", 8);
						break;
					case VK_F6:
						VIRTUALIZER1_START
						{
							DoTimer(hwnd);
						}
							VIRTUALIZER_END
							break;
					case VK_F7:
						VIRTUALIZER1_START
						{
							DoMouse(hwnd);
						}
							VIRTUALIZER_END
							break;
					case VK_F8:
						VIRTUALIZER1_START
						{
							DoCSWari(hwnd);
						}
							VIRTUALIZER_END
							break;
					case VK_F9:
						VIRTUALIZER1_START
						{
							if ((lan_mode = !lan_mode) == 0) {
								send_flush();
								MainWndProc(hwnd, WM_SOCKET, socket_handle, FD_READ);
							}
							update_title(hwnd);
						}
							VIRTUALIZER_END
							break;
					case VK_F10:
						VIRTUALIZER1_START
						{
							if (SECURE_API(s_GetKeyState)(VK_SHIFT) & 0x8000)
							shop_warpfreepos(hwnd);
							else
								DoShop(hwnd);
						}
							VIRTUALIZER_END
							break;
					case VK_F11:
						VIRTUALIZER1_START
						{
							if (SECURE_API(s_GetKeyState)(VK_CONTROL) & 0x8000)
							DoWarpSignal(hwnd);
							else if (SECURE_API(s_GetKeyState)(VK_SHIFT) & 0x8000) {
								lan_mode = ~0u;
								SendShopPacket(1);
								lan_mode = 0u;
								send_flush();
								MainWndProc(hwnd, WM_SOCKET, socket_handle, FD_READ);
								update_title(hwnd);
							}
							else
								DoNearSignal(hwnd);
						}
							VIRTUALIZER_END
							break;
					case VK_F12:
						VIRTUALIZER1_START
						{
							if (SECURE_API(s_GetKeyState)(VK_CONTROL) & 0x8000) {
								if (PacketWnd == NULL)
									PacketWnd = (HWND) SECURE_API(s_CreateDialogParamW)(module_base, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DlgPacketProc, 0);
							}
							else if (SECURE_API(s_GetKeyState)(VK_SHIFT) & 0x8000)
								SendShopWariPacket(hwnd);
							else
								DoShopWari(hwnd);
						}
							VIRTUALIZER_END
							break;
					case 0xBC: // [,]
						if (SECURE_API(s_GetKeyState)(VK_CONTROL) & 0x8000) {
							VIRTUALIZER1_START
							{
								if (SECURE_API(s_GetKeyState)(VK_SHIFT) & 0x8000)
								shop_warpnearest(-1);
								else
									shop_warpnearest(-130);
							}
								VIRTUALIZER_END
								__leave;
						}
						break;
					case 0xBE: // [.]
						if (SECURE_API(s_GetKeyState)(VK_CONTROL) & 0x8000) {
							VIRTUALIZER1_START
							{
								if (SECURE_API(s_GetKeyState)(VK_SHIFT) & 0x8000)
								shop_warpnearest(1);
								else
									shop_warpnearest(130);
							}
								VIRTUALIZER_END
								__leave;
						}
						break;

					case 0xBF: // [/]
						if (SECURE_API(s_GetKeyState)(VK_CONTROL) & 0x8000) {
							VIRTUALIZER1_START
							{
								shop_warpnearest(0);
							}
								VIRTUALIZER_END
								__leave;
						}
						break;
					}
				}
				else if (Msg == WM_KEYUP) {
					switch (wParam) {
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						if (SECURE_API(s_GetKeyState)(VK_CONTROL) & 0x8000)
							__leave;
						break;
					}
				}
			}
			else
				switch (Msg) {
				case WM_CREATE:
					VIRTUALIZER1_START
					{
						MainWnd = hwnd;
						if ((menu = (HMENU) SECURE_API(s_GetSystemMenu)(hwnd, 0)) != NULL) {
							SECURE_API(s_InsertMenuW)(menu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
							SECURE_API(s_InsertMenuW)(menu, 0, MF_BYPOSITION | MF_STRING | MF_ENABLED, kTimerEmptyWorkingSet, L"메모리 정리");
						}
						SECURE_API(s_SetTimer)(hwnd, kTimerEmptyWorkingSet, 20 * 60 * 1000, DoEmptyWorkingSet);
					}
						VIRTUALIZER1_END
						break;
				case WM_DESTROY:
					VIRTUALIZER1_START
					{
						SECURE_API(s_TerminateProcess)(SECURE_API(s_GetCurrentProcess)(), 0);
					}
						VIRTUALIZER1_END
						break;
				case WM_COPYDATA:
					if (((COPYDATASTRUCT *) lParam)->dwData == 0x02100713u) {
						VIRTUALIZER1_START
						{
							send_packet(((COPYDATASTRUCT *) lParam)->lpData, ((COPYDATASTRUCT *) lParam)->cbData);
						}
							VIRTUALIZER_END
							__leave;
					}
					break;
				case WM_SYSCOMMAND:
					if (wParam == kTimerEmptyWorkingSet) {
						VIRTUALIZER1_START
						{
							DoEmptyWorkingSet(hwnd, WM_TIMER, kTimerEmptyWorkingSet, (unsigned int) SECURE_API(s_GetTickCount)());
						}
							VIRTUALIZER_END
							__leave;
					}
					break;
				case 0xCA00:
					DoLogin(hwnd, 1);
					__leave;
					break;
			}
			result = MainWndProc(hwnd, Msg, wParam, lParam);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}

	return result;
}

ATOM WINAPI MyRegisterClassExA(WNDCLASSEXA *lpWndClassEx)
{
	static volatile skip;
	char data[16];

	if (skip == 0 && HIWORD(lpWndClassEx->lpszClassName) && memcmp(lpWndClassEx->lpszClassName, (*(int *) &data[0] = 'lpaM', *(int *) &data[4] = 'otSe', *(int *) &data[8] = 'lCyr', *(int *) &data[12] = 'ssa', data), 16) == 0 && _InterlockedExchange(&skip, ~0) == 0) {
		VIRTUALIZER1_START
		{
			MainWndProc = lpWndClassEx->lpfnWndProc;
			lpWndClassEx->lpfnWndProc = MyMainWndProc;
		}
			VIRTUALIZER_END
	}

	return (ATOM) SECURE_API(s_RegisterClassExA)(lpWndClassEx);
}

BOOL WINAPI MyPeekMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	BOOL i;

	if ((i = SECURE_API(s_PeekMessageA)(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg)) != 0) {
		SECURE_API(s_SleepEx)(0, 1);
		return i;
	}

	SECURE_API(s_SleepEx)(16, 1);
	return 0;
}

DWORD WINAPI MyMsgWaitForMultipleObjects(DWORD nCount, const HANDLE *pHandles, BOOL bWaitAll, DWORD dwMilliseconds, DWORD dwWakeMask)
{
	unsigned int i;

	if ((i = *(unsigned int *) pt_CWvsContext) != 0)
		*(unsigned char *) (i + pt_skipfadein) = *(unsigned char *) (i + pt_skipfadeout) = 1;

	if (mouse_mode)
		DoMouseSub(NULL, 0, 0, 0);

	return (DWORD) SECURE_API(s_MsgWaitForMultipleObjectsEx)(nCount, pHandles, dwMilliseconds, dwWakeMask, bWaitAll ? (MWMO_WAITALL | MWMO_ALERTABLE) : MWMO_ALERTABLE);
}

BOOL WINAPI MyGetCursorPos(LPPOINT lpPoint)
{
	lpPoint->x = LOWORD(mouse_pos);
	lpPoint->y = HIWORD(mouse_pos);
	return (BOOL) SECURE_API(s_ClientToScreen)(MainWnd, lpPoint);
}

BOOL WINAPI MyGetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize)
{
	BOOL i;

	VIRTUALIZER2_START
	{
		if ((i = (BOOL) SECURE_API(s_GetVolumeInformationA)(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize)) != 0)
		if (lpVolumeSerialNumber)
			*lpVolumeSerialNumber = (DWORD) SECURE_API(s_GetCurrentProcessId)();
	}
		VIRTUALIZER_END

		return i;
}

VOID WINAPI MySleep2(DWORD dwMilliseconds)
{
	if (mouse_mode)
		DoMouseSub(NULL, 0, 0, 0);

	if (dwMilliseconds < 16u) /* 메인루프에서 1ms 루프를 돈다.. 16ms 로 늘림 */
		dwMilliseconds = 16u; /* 이게 없으면 신호 졸라게 따임 ㅅㅂ */

	SECURE_API(s_SleepEx)(dwMilliseconds, 1);
}

HANDLE WINAPI MyCreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName)
{
	HANDLE hr;
	char data[16];

	VIRTUALIZER2_START
	{
		if ((hr = (HANDLE) SECURE_API(s_CreateMutexA)(lpMutexAttributes, bInitialOwner, lpName)) != NULL)
		if (HIWORD(lpName) && kstrstr(lpName, (*(int *) &data[0] = 'CsvW', *(int *) &data[4] = 'neil', *(int *) &data[8] = 'xtMt', *(int *) &data[12] = 0, data)))
			SECURE_API(s_SetLastError)(0);
	}
		VIRTUALIZER_END

		return hr;
}

void client_loading_done(void)
{
}

void client_hooksub(void)
{
	unsigned int i;

	VIRTUALIZER3_START
	{
		pt_sendpacket = (unsigned int) memscan(client_base, "\x8B\x44\x24\x04\x8B\x0D\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\xC3", sizeof("\x8B\x44\x24\x04\x8B\x0D\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\xC3") - 1);
		//dbg("%s=%p", "pt_sendpacket", pt_sendpacket);

		if ((i = (unsigned int) memscan(client_base, "\x51\x83\x3D\x00\x00\x00\x00\x00\x74", sizeof("\x51\x83\x3D\x00\x00\x00\x00\x00\x74") - 1)) != 0) {
			pt_chatlog = i;
			pt_CUIStatusBar = *(unsigned int *) (i + 3u);
		}
		//dbg("%s=%p", "pt_chatlog", pt_chatlog);

		if ((i = (unsigned int) memscan(client_base, "\xA3\x00\x00\x00\x00\xEB\x0A\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xC7", sizeof("\xA3\x00\x00\x00\x00\xEB\x0A\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xC7") - 1)) != 0)
			pt_CUniqueModeless = *(unsigned int *) (i + 1);
		//dbg("%s=%p", "pt_CUniqueModeless", pt_CUniqueModeless);

		if ((i = (unsigned int) memscan(client_base, "\x89\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x0F\xBE\x00\x89", sizeof("\x89\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x0F\xBE\x00\x89") - 1)) != 0)
			pt_shoptype = *(unsigned int *) (i + 16u);
		//dbg("%s=%p", "pt_shoptype", pt_shoptype);

		if ((i = (unsigned int) memscan(client_base, "\x89\x15\x00\x00\x00\x00\xC7\x00\x00\x00\x00\x00\xC7\x01\x00\x00\x00\x00\x89\x51\x00\xC7", sizeof("\x89\x15\x00\x00\x00\x00\xC7\x00\x00\x00\x00\x00\xC7\x01\x00\x00\x00\x00\x89\x51\x00\xC7") - 1)) != 0)
			pt_CEmployeePool = *(unsigned int *) (i + 2u);
		//dbg("%s=%p", "pt_CEmployeePool", pt_CEmployeePool);

		if ((i = (unsigned int) memscan(client_base, "\x89\x87\x00\x00\x00\x00\x8B\x44\x24\x00\xC7\x07", sizeof("\x89\x87\x00\x00\x00\x00\x8B\x44\x24\x00\xC7\x07") - 1)) != 0)
			pt_eshopid = *(unsigned int *) (i + 2u);
		//dbg("%s=%p", "pt_eshopid", pt_eshopid);

		if ((i = (unsigned int) memscan(client_base, "\x89\x86\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x0F\xBF\xC8\x89\x8E", sizeof("\x89\x86\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x0F\xBF\xC8\x89\x8E") - 1)) != 0) {
			pt_eshopx = *(unsigned int *) (i + 2u);
			pt_eshopy = *(unsigned int *) (i + 16u);
		}
		//dbg("%s=%p", "pt_eshopx", pt_eshopx);
		//dbg("%s=%p", "pt_eshopy", pt_eshopy);

		if ((i = (unsigned int) memscan(client_base, "\xA3\x00\x00\x00\x00\xC7\x06\x00\x00\x00\x00\x89\x44\x24\x00\x89\x41\x00\xC7", sizeof("\xA3\x00\x00\x00\x00\xC7\x06\x00\x00\x00\x00\x89\x44\x24\x00\x89\x41\x00\xC7") - 1)) != 0)
			pt_CUserPool = *(unsigned int *) (i + 1u);
		//dbg("%s=%p", "pt_CUserPool", pt_CUserPool);

		if ((i = (unsigned int) memscan(client_base, "\x89\x86\x00\x00\x00\x00\x8D\x44\x24\x00\x50\x8B\xCF\xE8\x00\x00\x00\x00\x8D", sizeof("\x89\x86\x00\x00\x00\x00\x8D\x44\x24\x00\x50\x8B\xCF\xE8\x00\x00\x00\x00\x8D") - 1)) != 0) {
			pt_ushopid = *(unsigned int *) (i + 2u);
			pt_ushopmode = pt_ushopid - 4u;
		}
		//dbg("%s=%p", "pt_ushopid", pt_ushopid);
		//dbg("%s=%p", "pt_ushopmode", pt_ushopmode);

		//if ((i = (unsigned int)memscan(client_base, "\x8B\x87\x00\x00\x00\x00\x55\x85\xC0\x74\x05", sizeof("\x8B\x87\x00\x00\x00\x00\x55\x85\xC0\x74\x05") - 1)) != 0)
		if ((i = (unsigned int) memscan(client_base, "\x88\x42\x06\xE8\x00\x00\x00\x00\x89\x9E\x00\x00\x00\x00\x8D\x54", sizeof("\x88\x42\x06\xE8\x00\x00\x00\x00\x89\x9E\x00\x00\x00\x00\x8D\x54") - 1)) != 0)
			pt_uservec = *(unsigned int *) (i + 10u);
		//dbg("%s=%p", "pt_uservec", pt_uservec);

		//if ((i = (unsigned int)memscan(client_base, "\x8D\x6B\x00\x89\xB3\x00\x00\x00\x00\xDD\x5D", sizeof("\x8D\x6B\x00\x89\xB3\x00\x00\x00\x00\xDD\x5D") - 1)) != 0) {
		//	pt_userx = *(unsigned char *)(i + 2u);
		//	pt_usery = pt_userx + 8u;
		//}
		//dbg("%s=%p", "pt_userx", pt_userx);
		//dbg("%s=%p", "pt_usery", pt_usery);

		// 이걸로 찾아올라가면됨 [v1.2.196(2)이후로 바뀜]
		// C7 83 ? ? 00 00 07 00 00 00
		pt_userx = 112u; /* FIXME */
		pt_usery = pt_userx + 8u;

		if ((i = (unsigned int) memscan(client_base, "\x74\x08\x39\x86\x00\x00\x00\x00\x74", sizeof("\x74\x08\x39\x86\x00\x00\x00\x00\x74") - 1)) != 0)
			pt_userid = *(unsigned int *) (i + 4u);
		//dbg("%s=%p", "pt_userid", pt_userid);

		if ((i = (unsigned int) memscan(client_base, "\x33\xC0\x39\x05\x00\x00\x00\x00\x0F\x95\xC0\xC3", sizeof("\x33\xC0\x39\x05\x00\x00\x00\x00\x0F\x95\xC0\xC3") - 1)) != 0)
			pt_CUserLocal = *(unsigned int *) (i + 4u);
		//dbg("%s=%p", "pt_CUserLocal", pt_CUserLocal);

		if ((i = (unsigned int) memscan(client_base, "\xA1\x00\x00\x00\x00\x85\xC0\x74\x1B\x8B\xF0\x85\xF6\x74\x17\x80\xBE", sizeof("\xA1\x00\x00\x00\x00\x85\xC0\x74\x1B\x8B\xF0\x85\xF6\x74\x17\x80\xBE") - 1)) != 0) {
			pt_CWvsContext = *(unsigned int *) (i + 1u);
			pt_skipfadeout = *(unsigned int *) (i + 17u);
			pt_skipfadein = *(unsigned int *) (i + 17u) - 1u;
		}

		//dbg("%s=%p", "pt_CWvsContext", pt_CWvsContext);
		//dbg("%s=%p", "pt_skipfadeout", pt_skipfadeout);
		//dbg("%s=%p", "pt_skipfadein", pt_skipfadein);

		if ((i = (unsigned int) memscan(client_base, "\x56\x8B\x35\x00\x00\x00\x00\x85\xF6\x74\x00\x8B\x46", sizeof("\x56\x8B\x35\x00\x00\x00\x00\x85\xF6\x74\x00\x8B\x46") - 1)) != 0)
			pt_CStage = *(unsigned int *) (i + 3u);
		//dbg("%s=%p", "pt_CStage", pt_CStage);

		if ((i = (unsigned int) memscan(client_base, "\xFF\xD2\x8B\x0D\x00\x00\x00\x00\x3B\xCB\x74\x19\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x3B\xC3\x74\x0B\x8D\x48\x08\x8B\x01\x8B\x10\x6A\x01\xFF\xD2\x8B\x0D\x00\x00\x00\x00\x3B\xCB\x74\x19\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x3B\xC3\x74\x0B\x8D\x48\x08\x8B\x01\x8B\x10\x6A\x01\xFF\xD2", sizeof("\xFF\xD2\x8B\x0D\x00\x00\x00\x00\x3B\xCB\x74\x19\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x3B\xC3\x74\x0B\x8D\x48\x08\x8B\x01\x8B\x10\x6A\x01\xFF\xD2\x8B\x0D\x00\x00\x00\x00\x3B\xCB\x74\x19\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x3B\xC3\x74\x0B\x8D\x48\x08\x8B\x01\x8B\x10\x6A\x01\xFF\xD2") - 1)) != 0) {
			pt_CUITitle = *(unsigned int *) (i + 4u);
			pt_CUIWorldSelect = *(unsigned int *) (i + 39u);
			pt_CUIChannelSelect = *(unsigned int *) (i + 74u);
			pt_CUICharSelect = *(unsigned int *) (i + 109u);
		}
		//dbg("%s=%p", "pt_CUITitle", pt_CUITitle);
		//dbg("%s=%p", "pt_CUIWorldSelect", pt_CUIWorldSelect);
		//dbg("%s=%p", "pt_CUIChannelSelect", pt_CUIChannelSelect);
		//dbg("%s=%p", "pt_CUICharSelect", pt_CUICharSelect);

		if ((i = (unsigned int) memscan(client_base, "\x33\xED\x39\xAE\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x8B\x86\x00\x00\x00\x00\x3B\xC5", sizeof("\x33\xED\x39\xAE\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x8B\x86\x00\x00\x00\x00\x3B\xC5") - 1)) != 0) {
			pt_waitresp = *(unsigned int *) (i + 4u);
			pt_charidx = *(unsigned int *) (i + 16u);
		}
		//dbg("%s=%p", "pt_waitresp", pt_waitresp);
		//dbg("%s=%p", "pt_charidx", pt_charidx);

		if ((i = (unsigned int) memscan(client_base, "\x6A\xFF\x8D\x8F\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x00\x8D", sizeof("\x6A\xFF\x8D\x8F\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x00\x8D") - 1)) != 0)
			pt_worlditem = *(unsigned int *) (i + 4u);
		//dbg("%s=%p", "pt_worlditem", pt_worlditem);

		if ((i = (unsigned int) memscan(client_base, "\x8D\x9E\x00\x00\x00\x00\x57\x8B\xCB\xC6", sizeof("\x8D\x9E\x00\x00\x00\x00\x57\x8B\xCB\xC6") - 1)) != 0)
			pt_softkeyboard = *(unsigned int *) (i + 2u);
		//dbg("%s=%p", "pt_softkeyboard", pt_softkeyboard);

		if ((i = (unsigned int) memscan(client_base, "\x8B\x0D\x00\x00\x00\x00\x74\x00\x83\xB9", sizeof("\x8B\x0D\x00\x00\x00\x00\x74\x00\x83\xB9") - 1)) != 0)
			pt_CInputSystem = *(unsigned int *) (i + 2u);
		//dbg("%s=%p", "pt_CInputSystem", pt_CInputSystem);

		if ((i = (unsigned int) memscan(client_base, "\x8B\xB1\x00\x00\x00\x00\x85\xF6\x75\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x4C\x24\x08\x8B\x06\x8B\x90\x00\x00\x00\x00\xF7\xD9", sizeof("\x8B\xB1\x00\x00\x00\x00\x85\xF6\x75\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x4C\x24\x08\x8B\x06\x8B\x90\x00\x00\x00\x00\xF7\xD9") - 1)) != 0)
			pt_mousevec = *(unsigned int *) (i + 2u);
		//dbg("%s=%p", "pt_mousevec", pt_mousevec);

		if ((i = (unsigned int) memscan(client_base, "\x39\x98\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x8B\x5C", sizeof("\x39\x98\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x8B\x5C") - 1)) != 0) {
			pt_mousex = 0x88; //*(unsigned int *)(i + 2u);
			pt_mousey = pt_mousex + 4u;
		}
		//dbg("%s=%p", "pt_mousex", pt_mousex);
		//dbg("%s=%p", "pt_mousey", pt_mousey);

		if ((i = (unsigned int) memscan(client_base, "\x8B\x87\x00\x00\x00\x00\x3B\x86\x00\x00\x00\x00\x0F\x85", sizeof("\x8B\x87\x00\x00\x00\x00\x3B\x86\x00\x00\x00\x00\x0F\x85") - 1)) != 0)
			pt_accid = *(unsigned int *) (i + 2u);
		//dbg("%s=%p", "pt_accid", pt_accid);

		// 8B 87 ? ? ? ? 3B 86 ? ? ? ? 0F 85

		// 8b 08 89 8e ? ? ? ? 8b 50 04 8b 06 89 96
		// v8 = *(int (__thiscall **)(_DWORD, _DWORD))(*((_DWORD *)v5 + 1) + 16)
		pt_getpos = 16; /* CUser::GetPos (IVecCtrlOwner) */

		// 83 be ? ? ? ? ? 89 44 24 ? 0f 84 ? ? ? ? 8b 86 ? ? ? ?
		// v12 = (*(int (__thiscall **)(char *))(v11 + 32))((char *)v3 + 4)
		// (*(void (__thiscall **)(int, _DWORD, _DWORD, _DWORD))(*(_DWORD *)v12 + 116))
		pt_getvecctrl = 32; /* CUser::GetVecCtrl (IVecCtrlOwner) */
		pt_rawmove = 116 + 8; /* CVecCtrl::raw_Move (CVecCtrlUser) */

		//SECURE_API(s_ExitProcess)(0);
	}
		VIRTUALIZER_END
}

void client_hook(void)
{
	static unsigned int addr1, addr2, code[2];
	unsigned int i, j;
	PROC fn;

	VIRTUALIZER3_START
	{
		if ((secure_value_A ^ (init_rand(SECURE_API(s_GetCurrentProcessId)() ^ 0xBADF00Du), gen_rand() ^ gen_rand())) == 0x92041719u)
		for (fn = SECURE_API(s_CreateMutexA); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC) MyCreateMutexA);
		for (fn = SECURE_API(s_Sleep); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC) MySleep2);
		for (fn = SECURE_API(s_GetVolumeInformationA); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC) MyGetVolumeInformationA);
		for (fn = SECURE_API(s_GetCursorPos); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC) MyGetCursorPos);
		for (fn = SECURE_API(s_MsgWaitForMultipleObjects); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC) MyMsgWaitForMultipleObjects);
		for (fn = SECURE_API(s_PeekMessageA); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC) MyPeekMessageA);
		for (fn = SECURE_API(s_RegisterClassExA); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC) MyRegisterClassExA);
		for (fn = SECURE_API(s_SetForegroundWindow); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC)"\xC2\x04\x00");
		for (fn = SECURE_API(s_SetWindowsHookExA); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC)"\x33\xC0\xC2\x10\x00");
		for (fn = SECURE_API(s_UpdateWindow); (i = (unsigned int) memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *) i = (PROC)"\xC2\x04\x00");

		/*{
		dbg("HS %08X", memscan(client_base, "\x8B\x4D\x18\x89\x8D\x5C\xFD\xFF\xFF\xC7\x85\x60\xFD\xFF\xFF", 15));
		ExitProcess(0);
		}*/

		/* prepare packet */
		PrepareShopPacket();

		/* search memory addresses */
		client_hooksub();

		/* skip CLogo */
		if ((i = (unsigned int) memscan(client_base, "\x55\x49\x2F\x4C\x6F\x67\x6F\x2E", 8)) != 0)
		{
			i = *(unsigned int *) (i - 8u);
			addr1 = i; /* CLogo::Init() */
			if ((j = (unsigned int) _memscan((void *) i, 128, "\x74\x00\x8B\x00\xE8\x00\x00\xFF", 8)) != 0) {
				j = (j + 4u) + *(unsigned int *) (j + 5u) + 5u;
				addr2 = j; /* CLogo::LogoEnd() */
				__asm {
					push offset code;
					push offset $SKIP_INTRO_BEGIN;
					push addr1;
					call HookJMP;
					add esp, 12;
				}
			}
		}
	}
		VIRTUALIZER_END

		__asm {
		jmp short $SKIP_INTRO_END;
	$SKIP_INTRO_BEGIN:
		VIRTUALIZER1_START
		{
			pushad
			push offset code;
			push 0;
			push addr1;
			call HookJMP;
			add esp, 12;
			call client_loading_done
				popad
				call addr2;
		}
			VIRTUALIZER_END
			ret 4;
	$SKIP_INTRO_END:
	}
}