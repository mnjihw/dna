#include "common.h"

typedef struct session
{
	unsigned int auth, recv_iv, send_iv, ver, size;
	unsigned char data[128 * 1024];
} session_t;

extern aes256_context ctx;
const unsigned char *aeskey = "\x48\x00\x00\x00\xE6\x00\x00\x00\xE5\x00\x00\x00\x78\x00\x00\x00\x0E\x00\x00\x00\xD4\x00\x00\x00\x7B\x00\x00\x00\x3B\x00\x00\x00";
ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];
unsigned int adapterSel, breakloop, blacking, 해상도, limitsecond, limitcount;
int operating = 1, start;
HANDLE hEvent;
DWORD target_pid;
image_t *블랙리스트, *블랙리스트2, *추가, *없음, *삭제, *캐럿;
HINSTANCE g_hInst;
HWND g_hWnd, g_hButton, g_hList1, g_hList2, g_hMP, g_hEdit1, g_hEdit2, target_window;


struct list
{
	char data[41];
	int islimit;
};

void SuspendProcess(DWORD pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess) GetProcAddress(GetModuleHandle("ntdll.dll"), "NtSuspendProcess");

	pfnNtSuspendProcess(hProcess);
	CloseHandle(hProcess);
}

void ResumeProcess(DWORD pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	NtResumeProcess pfnNtResumeProcess = (NtResumeProcess) GetProcAddress(GetModuleHandle("ntdll.dll"), "NtResumeProcess");

	pfnNtResumeProcess(hProcess);
	CloseHandle(hProcess);
}

int auth(const char *data)
{
	int check = 0, i = 0;
	struct list list[50] = { 0, };
	HANDLE hFile;
	HKEY hkey = 0;
	DWORD cbData = 255, dwType;
	char MaplePath[256], buf[256];

	VIRTUALIZER2_START
	{
		for (i = 0; i < _countof(list); ++i)
			list[i].islimit = 1;

		list[0].islimit = 0;
		list[1].islimit = 0;

		wsprintf(list[0].data, "6628FAC83923D1C364DB1C20F5E866EE9C1C7E6F");//나 무제한
		wsprintf(list[1].data, "8FC2BA0A6AC027A64AA1C3DD18E920B45FB18D0D");//최준영 무제한
		//wsprintf(list[2].data, "CBA6485AEBB1C76CDAE95CC5552B04355D90FDE0");//최재운 나이스 15년1월5일 1달
		for (i = 0; i < _countof(list); ++i)
		{
			if (strcmp(list[i].data, data) == 0)
			{
				check = 1;
				break;
			}
		}
		RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wizet\\MapleStory", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY | KEY_ENUMERATE_SUB_KEYS, NULL, &hkey, NULL);

		if (RegQueryValueEx(hkey, "ExecPath", NULL, &dwType, (LPBYTE) MaplePath, &cbData) != ERROR_SUCCESS)
		{
			wsprintf(buf, "%s() %u", "RegQueryValueEx", GetLastError());
			MessageBox(0, buf, 0, MB_ICONERROR);
			check = 0;
		}
		RegCloseKey(hkey);

		wsprintf(buf, "%s\\MapleStory.exe", MaplePath);

		if (check && list[i].islimit) //인증 돼있으면서 무제한 아닌경우
		{
			hFile = CreateFile(buf, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
			if (hFile == INVALID_HANDLE_VALUE || GetFileSize(hFile, 0) != 11772672)//크기
				check = 0;

			CloseHandle(hFile);
		}
	}
	VIRTUALIZER_END

	return check;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	char data[2048];
	MSG Message;
	g_hInst = hInstance;
	int check;
	
	 
	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR);
		return 0;
	}

	VIRTUALIZER2_START
	{
		getuuid(data);
		check = auth(data);
		if (check == 0)
			ExitProcess(0);
	}
	VIRTUALIZER_END
	

	CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);


	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

void getuuid(char *data)
{
	VIRTUALIZER3_START
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
	VIRTUALIZER_END
}


void Clipboard(char *source)
{
	HGLOBAL clipbuffer;
	char * buffer;
	int srclen, ok;

	ok = OpenClipboard(NULL);
	if (!ok)
		return;
	EmptyClipboard();
	srclen = strlen(source) + 1;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, srclen);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy_s(buffer, srclen, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

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

void MouseMove(int x, int y)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
}

void AddBlacklist(char *nick)
{
	int i;

	g_hMP = FindWindow("MapleStoryClass", NULL);
	if (g_hMP)
	{
		SetForegroundWindow(g_hMP);
		Sleep(300);

		if ((i = Global_ImageSearch(블랙리스트)) != -1 || (i = Global_ImageSearch(블랙리스트2)) != -1)
		{
			if (target_pid)
				SuspendProcess(target_pid);
			Sleep(100);
			MouseMove(30, 30);
			Clipboard(nick);
			if (해상도 == 910)
				MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
			else
				MouseClick(i & 65535, (i >> 16), 1);
			Sleep(150);
			i = Global_ImageSearch(추가);
			if (해상도 == 910)
				MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
			else
				MouseClick(i & 65535, (i >> 16), 1);
			Sleep(150);
			do
			{
				keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
				Sleep(50);
				PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
				Sleep(50);
				keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				Sleep(100);
			} while (Global_ImageSearch(캐럿) != -1);
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			Sleep(100);
			if (target_pid)
				ResumeProcess(target_pid);
			
		}
		else
		{
			if (target_pid)
				SuspendProcess(target_pid);
			Sleep(150);
			Clipboard(nick);
			MouseClick(177, 37, 1);
			Sleep(150);
			MouseClick(106, 344, 1);
			Sleep(150);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
			Sleep(50);
			PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
			Sleep(50);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(100);
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			Sleep(150);
			if (target_pid)
				ResumeProcess(target_pid);

		}

	}
}

VOID CALLBACK DeleteAllBlacklist(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	int i;

	if (!operating)
		return;
	if (start)
		while (start)
			SwitchToThread();
	
	if (blacking)
		return;
	blacking = 1;

	g_hMP = FindWindow("MapleStoryClass", NULL);
	if (g_hMP)
	{
		SetForegroundWindow(g_hMP);
		Sleep(100);

		if ((i = Global_ImageSearch(블랙리스트)) != -1 || (i = Global_ImageSearch(블랙리스트2)) != -1)
		{
			if (target_pid)
				SuspendProcess(target_pid);
			MouseMove(30, 30);
			Sleep(100);
			if (해상도 == 910)
				MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
			else
				MouseClick(i & 65535, (i >> 16), 1);
			Sleep(100);
			if ((i = Global_ImageSearch(삭제)) != -1)
			{
				do
				{
					if (해상도 == 910)
						MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
					else
						MouseClick(i & 65535, (i >> 16), 1);
					Sleep(50);
					PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
					Sleep(50);
				} while (Global_ImageSearch(없음) == -1);
			}
			if (target_pid)
				ResumeProcess(target_pid);
		}
		else
		{
			if (target_pid)
				SuspendProcess(target_pid);
			MouseMove(30, 30);
			Sleep(100);
			switch (해상도)
			{
			case 800:
			case 1024:
			case 1366:
				MouseClick(168, 39, 1);
				break;
			case 910:
				MouseClick(116, 26, 1);
				break;
			}
			Sleep(100);
			for (i = 0; i < 30; ++i)
			{
				switch (해상도)
				{
				case 800:
				case 1024:
				case 1366:
					MouseClick(163, 341, 1);
					break;
				case 910:
					MouseClick(111, 230, 1);
					break;
				}

				Sleep(50);
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
				Sleep(50);
			}
			if (target_pid)
				ResumeProcess(target_pid);
		}
	}
	
	blacking = 0;
}

typedef struct
{
	int isblacked;
	char nick[13];
	DWORD first, last;
	unsigned int count;

}list_t;

list_t list[26];



void packet_handle(session_t *s, unsigned char *data, unsigned int size)
{
	char nick[13];
	SYSTEMTIME st;
	static unsigned int blackcount;
	int nicksize, i, check = 0;
	char buf[128];

	if (operating)
	{
		if (blacking)
			while (blacking)
				SwitchToThread();
		if (*(unsigned short*)data == 0x3BA && *(unsigned char*)&data[2] == 0x15) //교환 패킷
		{
			start = 1;
			nicksize = *(unsigned short*)&data[4];
			memcpy(nick, &data[6], nicksize);
			nick[nicksize] = '\0';
		$start:
			for (i = 0; i < _countof(list); ++i)
			{
				if (list[i].nick[0] == 0) //비어있으면 닉이랑 시간 넣음
				{
					strcpy(list[i].nick, nick);
					list[i].nick[nicksize] = '\0';
					list[i].last = list[i].first = GetTickCount();
					++list[i].count;
					check = 1;
					break;
				}
				if (strcmp(list[i].nick, nick) == 0) 
				{
					check = 1;
					if (list[i].isblacked == 1)
					{
						GetLocalTime(&st);
						wsprintf(buf, "%s %02u시%02u분%02u초 %s 이미 블추돼있음", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, nick);
						SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM) buf), 0);
						return;
					}
					if (list[i].first == 0)
						list[i].first = GetTickCount();
					list[i].last = GetTickCount();
					++list[i].count;

					limitsecond = GetDlgItemInt(g_hWnd, IDC_EDIT1, 0, 0);
					limitcount = GetDlgItemInt(g_hWnd, IDC_EDIT2, 0, 0);
					if (limitsecond == 0)
						limitsecond = 60;
					if (limitcount == 0)
						limitcount = 5;
					if (list[i].count == limitcount)
					{
						if ((list[i].last - list[i].first) / 1000 <= limitsecond) 
						{
							GetLocalTime(&st);
							wsprintf(buf, "%s %02u시%02u분%02u초 %s 블추함", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, nick);
							SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM) buf), 0);
							AddBlacklist(list[i].nick);
							list[i].isblacked = 1;
							++blackcount;
							list[i].count = list[i].last = list[i].first = 0;
							//if (blackcount == _countof(list))
							if (blackcount == 10)
							{
								GetLocalTime(&st);
								wsprintf(buf, "%s %02u시%02u분%02u초 리스트가 꽉 차서 초기화함", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond);
								SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM) buf), 0);
								start = 0;
								DeleteAllBlacklist(g_hWnd, WM_TIMER, 1, GetTickCount());
								start = 1;
								memset(&list, 0, sizeof(list));
								blackcount = 0;
							}
							return;
						}
						else
						{
							list[i].count = 1;
							list[i].last = list[i].first = GetTickCount();
						}

					}
					break;
				}
			}
			if (check == 0)
			{
				GetLocalTime(&st);
				wsprintf(buf, "%s %02u시%02u분%02u초 리스트가 꽉 차서 초기화함", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond);
				SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM) buf), 0);
				memset(&list, 0, sizeof(list));
				goto $start;
			}
			GetLocalTime(&st);
			wsprintf(buf, "%s %02u시%02u분%02u초 %s님이 교환걸음",st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, nick);
			SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
			start = 0;

		}
		else if (*(unsigned short*)data == 0x130)
		{
			if (*(unsigned char*)&data[2] > 19)
				return;
			else if (*(unsigned char*)&data[2] == 0)
				i = 1;
			else if (*(unsigned char*)&data[2] == 1)
				i = 20;
			else
				i = *(unsigned char*)&data[2];
			if (i == 20)
				wsprintf(buf, "현재 채널: %d세", i);
			else
				wsprintf(buf, "현재 채널: %d채널", i);
			SendMessage(g_hList2, LB_DELETESTRING, 0, 0);
			SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)buf);
		}
	}
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

		wzcrypt_decrypt(j, k, &data[i + 4u], pkt);
		packet_handle(s, pkt, j);
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

INT_PTR CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int count, i;
	char name[128], desc[128];


	switch (iMessage)
	{
	case WM_INITDIALOG:
		count = NpfFindAllDevices();
		for (i = 1; i <= count; ++i)
		{
			NpfGetDeviceInfo(i, name, desc);
			SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)desc);
		}
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, adapterSel, 0);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			adapterSel = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
			NpfGetDeviceInfo(adapterSel + 1, name, desc);
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
	RECT rect;
	int ys, xs;
	HBITMAP hbitmap;
	HDC hdc, hmemdc;


	if (!GetClientRect(hwnd, &rect))
	{
		MessageBox(0, "메이플 찾지 못함!", "메세지박스", MB_OK);
		return 0;
	}

	hdc = GetDC(g_hMP);

	ys = rect.bottom - rect.top;
	xs = rect.right - rect.left;
	
	

	if (xs == 800 && ys == 600)
		해상도 = 800;
	else if (xs == 1024 && ys == 768)
		해상도 = 1024;
	else if (xs == 1366 && ys == 768)
		해상도 = 1366;
	else if (xs == 910 && ys == 512)
		해상도 = 910;
	
	

	hbitmap = CreateCompatibleBitmap(hdc, xs, ys);
	hmemdc = CreateCompatibleDC(hdc);
	SelectObject(hmemdc, hbitmap);
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
DWORD WINAPI NpfLoop(LPVOID arg)
{

	while (!breakloop)
		NpfCheckMessage();
	NpfStop();

	breakloop = 0;

	return 0;
}



INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	MSG message;
	char buf[128];


	switch (iMessage)
	{
	case WM_HOTKEY:
		if (wParam == 1)
		{
			if (SendDlgItemMessage(hDlg, IDC_CHECK1, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
				SendDlgItemMessage(hDlg, IDC_CHECK1, BM_SETCHECK, BST_CHECKED, 0);
			else
				SendDlgItemMessage(hDlg, IDC_CHECK1, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(g_hWnd, WM_COMMAND, LOWORD(IDC_CHECK1), 0);
		}
		break;
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		g_hButton = GetDlgItem(hDlg, IDC_BUTTON1);
		g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
		g_hList2 = GetDlgItem(hDlg, IDC_LIST2);
		g_hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		g_hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		g_hMP = FindWindow("MapleStoryClass", NULL);
		SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)"현재 채널: NULL");
		SendDlgItemMessage(hDlg, IDC_CHECK1, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(g_hEdit1, EM_LIMITTEXT, 3, 0);
		SendMessage(g_hEdit2, EM_LIMITTEXT, 2, 0);
		SetWindowText(g_hEdit1, "60");
		SetWindowText(g_hEdit2, "5");
		limitsecond = 60;
		limitcount = 5;
		블랙리스트 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1)));
		블랙리스트2 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2)));
		추가 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3)));
		없음 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP4)));
		삭제 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP5)));
		캐럿 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP6)));
		SetTimer(hDlg, 1, 60 * 1000 * 10, DeleteAllBlacklist);
		
		aes256_init(&ctx, aeskey);
		RegisterHotKey(hDlg, 1, MOD_NOREPEAT, VK_F1);
		NpfFindAllDevices();
		if (NpfStart())
			CreateThread(0, 0, NpfLoop, 0, 0, 0);
		else
		{
			MessageBox(0, "NpfStart() 실패", "알림", MB_ICONERROR);
			ExitProcess(1);
		}
		SetFocus(g_hWnd);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			SetWindowText(g_hButton, "장매 윈도우에 우클릭");
			EnableWindow(g_hButton, 0);
			while (1)
			{
				if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
						exit(0);
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				else
				{
					Sleep(10);
					if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
						break;
				}

			}
			EnableWindow(g_hButton, 1);
			target_window = GetForegroundWindow();
			SetWindowText(g_hButton, "장매 핸들 얻어오기");
			if (target_window == g_hWnd)
			{
				MessageBox(0, "무교방어 윈도우 핸들은 얻어올 수 없습니다.", "알림", MB_OK);
				return 0;
			}
			wsprintf(buf, "%s(핸들: O)", "리듬 무교방어");
			SetWindowText(g_hWnd, buf);
			GetWindowThreadProcessId(target_window, &target_pid);
			break;
		case IDC_BUTTON2:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		case IDC_CHECK1:
			operating = SendDlgItemMessage(hDlg, IDC_CHECK1, BM_GETCHECK, 0, 0);
			break;
		}
		break;
	case WM_CLOSE:
		breakloop = 1;
		while (breakloop)
			SwitchToThread();
		DeleteObject(블랙리스트);
		DeleteObject(블랙리스트2);
		DeleteObject(추가);
		DeleteObject(없음);
		DeleteObject(삭제);
		DeleteObject(캐럿);
		UnregisterHotKey(hDlg, 1);
		PostQuitMessage(0);
		return 0;

	}
	return 0;
}

