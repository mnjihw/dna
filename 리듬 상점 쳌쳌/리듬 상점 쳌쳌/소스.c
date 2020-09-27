#include "common.h"


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
WNDPROC EditWndProc;
HINSTANCE g_hInst;
LPCTSTR lpszClass = "";
unsigned int adapterSel, startup;
unsigned int SERVERINFO, USER_MINIROOM_BALLON, EMPLOYEE_LEAVE;
HWND g_hWnd, g_hList1, g_hList2, g_hList3;

typedef struct charinfo
{
	unsigned int id;
	char nick[13];
}charinfo_t;

typedef struct session
{
	unsigned int auth, recv_iv, send_iv, ver, size;
	unsigned char data[128 * 1024];
} session_t;
ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];
charinfo_t charinfo[34];

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);





	CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);


	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}





void packet_handle(session_t *s, unsigned char *data, unsigned int size)
{
	static int currentChannel;
	char buf[1024], shoptitle[51];
	int i, j;
	SYSTEMTIME st;

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
		SendMessage(g_hList2, LB_DELETESTRING, 0, 0);
		SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)buf);
	}
	else if (*(unsigned short*)data == USER_MINIROOM_BALLON)
	{
		GetLocalTime(&st);
		if (*(unsigned char*)&data[6] != 0)
		{
			memcpy(shoptitle, &data[13], *(unsigned short*)&data[11]);
			shoptitle[*(unsigned short*)&data[11]] = '\0';
			for (i = 0; i < 34; ++i)
			{
				if (charinfo[i].id == *(unsigned int*)&data[2])
				{
					wsprintf(buf, "%u년%u월%u일 %s %u시%u분%u초 %s(0x%X)님이 [%s]제목으로 일상 폈거나 제목 바꿈", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, charinfo[i].nick, *(unsigned int*)&data[2], shoptitle);
					SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
					return;
				}
			}
			wsprintf(buf, "%u년%u월%u일 %s %u시%u분%u초 0x%X님이 [%s]제목으로 일상 폈거나 제목 바꿈", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, *(unsigned int*)&data[2], shoptitle);
			
		}
		else
			wsprintf(buf, "%u년%u월%u일 %s %u시%u분%u초 0x%X님이 일상 닫음", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, *(unsigned int*)&data[2]);
		SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
	}
	else if (*(unsigned short*)data == EMPLOYEE_LEAVE - 1) //고상 열 때 오는 패킷
	{
		GetLocalTime(&st);
		if (*(unsigned char*)&data[size - 2] == 0x1)
		{
			for (i = 0; i < 34; ++i)
				if (charinfo[i].id == 0)
					break;
			charinfo[i].id = *(unsigned int*)&data[2];
			memcpy(charinfo[i].nick, &data[18], *(unsigned short*)&data[16]);
			if (g_hList3)
			{
				wsprintf(buf, "%s(0x%X)", charinfo[i].nick, charinfo[i].id);
				SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM)buf), 0);
			}
			j = *(unsigned short*)&data[16];
			memcpy(shoptitle, &data[16 + 1 + *(unsigned short*)&data[16] + 8], *(unsigned short*)&data[16 + 1 + j + 6]);
			shoptitle[*(unsigned short*)&data[16 + 1 + j + 6]] = '\0';
			wsprintf(buf, "%u년%u월%u일 %s %u시%u분%u초 %s(0x%X)님이 [%s]제목으로 고상 핌", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, charinfo[i].nick, *(unsigned int*)&data[2], shoptitle);
			SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
		}
	}
	else if (*(unsigned short*)data == EMPLOYEE_LEAVE) //고상 닫힐 때 오는 패킷
	{
		GetLocalTime(&st);
		for (i = 0; i < 34; ++i)
		{
			if (charinfo[i].id == *(unsigned int*)&data[2])
			{
				wsprintf(buf, "%u년%u월%u일 %s %u시%u분%u초 %s(0x%X)님이 고상 닫음", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, charinfo[i].nick, *(unsigned int*)&data[2]);
				SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
				return;
			}
		}
		wsprintf(buf, "%u년%u월%u일 %s %u시%u분%u초 0x%X님이 고상 닫음", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, *(unsigned int*)&data[2]);
		SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
	}
	else if (*(unsigned short*)data == EMPLOYEE_LEAVE + 1)//고상 제목 바꿀 때 오는 패킷
	{
		GetLocalTime(&st);
		if (*(unsigned char*)&data[6])
		{
			for (i = 0; i < 34; ++i)
			{
				if (charinfo[i].id == *(unsigned int*)&data[2])
				{
					memcpy(shoptitle, &data[13], *(unsigned short*)&data[11]);
					shoptitle[*(unsigned short*)&data[11]] = '\0';
					wsprintf(buf, "%u년%u월%u일 %s %u시%u분%u초 %s(0x%X)님이 [%s]으로 고상 제목 바꿈", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, charinfo[i].nick, *(unsigned int*)&data[2], shoptitle);
					SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
					return;
				}
			}
			wsprintf(buf, "%u년%u월%u일 %s %u시%u분%u초 0x%X님이 [%s]으로 고상 제목 바꿈", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, *(unsigned int*)&data[2], shoptitle);
			SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
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

DWORD WINAPI NpfLoop(LPVOID arg)
{
	for (;;)
		NpfCheckMessage();
	NpfStop();

	return 0;
}

INT_PTR CALLBACK NickDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int i;
	char buf[256];

	switch (iMessage)
	{
	case WM_INITDIALOG:
		g_hList3 = GetDlgItem(hDlg, IDC_LIST1);
		for (i = 0; i < 34; ++i)
		{
			if (charinfo[i].id)
			{
				wsprintf(buf, "%s(0x%X)", charinfo[i].nick, charinfo[i].id);
				SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM)buf), 0);
			}
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			if ((i = SendMessage(g_hList3, LB_GETCURSEL, 0, 0)) == LB_ERR)
			{
				MessageBox(0, "삭제할 항목을 선택해주세요", "알림", MB_OK);
				return 0;
			}
			SendMessage(g_hList3, LB_DELETESTRING, (WPARAM)i, 0);
			charinfo[i].id = 0;
			memset(charinfo[i].nick, 0, sizeof(charinfo[i].nick));
			break;
		case IDC_BUTTON2:
			SendMessage(g_hList3, LB_RESETCONTENT, 0, 0);
			memset(charinfo, 0, sizeof(charinfo));
			break;
		}
		break;
	case WM_CLOSE:
		g_hList3 = 0;
		EndDialog(hDlg, 0);
		return 0;
	}
	return 0;
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


void ReadINI()
{
	char INIPath[128];

	GetModuleFileName(NULL, INIPath, sizeof(INIPath));

	INIPath[strlen(INIPath) - 3] = 'i';
	INIPath[strlen(INIPath) - 2] = 'n';
	INIPath[strlen(INIPath) - 1] = 'i';

	if (!(SERVERINFO = GetPrivateProfileInt("PACKET", "SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SERVERINFO", "", INIPath);
	if (!(USER_MINIROOM_BALLON = GetPrivateProfileInt("PACKET", "USER_MINIROOM_BALLON", 0, INIPath)))
		WritePrivateProfileString("PACKET", "USER_MINIROOM_BALLON", "", INIPath);
	if (!(EMPLOYEE_LEAVE = GetPrivateProfileInt("PACKET", "EMPLOYEE_LEAVE", 0, INIPath)))
		WritePrivateProfileString("PACKET", "EMPLOYEE_LEAVE", "", INIPath);
}

INT_PTR CALLBACK OnInitDialog(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	g_hWnd = hDlg;
	g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
	g_hList2 = GetDlgItem(hDlg, IDC_LIST2);
	SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)"현재 채널: NULL");
	ReadINI();
	NpfFindAllDevices();
	if (NpfStart())
		CreateThread(0, 0, NpfLoop, 0, 0, 0);
	else
	{
		MessageBox(0, "NpfStart() 실패", "알림", MB_ICONERROR);
		ExitProcess(1);
	}
	
	return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

	switch (iMessage)
	{
	case WM_INITDIALOG:
		OnInitDialog(hDlg, iMessage, wParam, lParam);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG3), g_hWnd, NickDlgProc);
			break;
		case IDC_BUTTON2:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		}
		break;
		
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{


	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
