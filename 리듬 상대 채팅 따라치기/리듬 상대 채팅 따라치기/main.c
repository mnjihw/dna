#include "common.h"


#define USERINFO 0x5C
#define SERVERINFO 0x157
#define CHARACTER_APPEAR 0x19F
#define CHATTING 0x1A1

#define S_CHATTING 0xB7

const unsigned char aeskey[] = "\x48\x00\x00\x00\xE6\x00\x00\x00\xE5\x00\x00\x00\x78\x00\x00\x00\x0E\x00\x00\x00\xD4\x00\x00\x00\x7B\x00\x00\x00\x3B\x00\x00\x00";
extern aes256_context ctx;
HWND g_hWnd, g_hList1, g_hEdit1, g_hMP;
unsigned int adapter_sel, target_id, operating, promiscuous_mode, startup, breakloop;

charinfo_t character_list[256];
DWORD offset, len, origin_crc, pid;
HANDLE hThread;
 
typedef struct {
	unsigned int size;
	unsigned char data[60];
} packet_t;

packet_t packet;



void Clipboard(char *source)
{
	HGLOBAL clipbuffer;
	char * buffer;
	int srclen;
	int ok = OpenClipboard(NULL);
	if (!ok) 
		return;

	EmptyClipboard();
	srclen = strlen(source) + 1;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, srclen);
	buffer = (char*) GlobalLock(clipbuffer);
	strcpy(buffer, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
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
		cds.lpData = (void *) data;
		SendMessage(g_hMP, WM_COPYDATA, 0, (LPARAM) &cds);
	}
}


void packet_handle(unsigned char *data, unsigned int size)
{
	char buf[128];
	static unsigned int lastid, myid;
	static int i, j;
	static DWORD tick;

	switch (*(unsigned short*) data)
	{
	case USERINFO:
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*) &data[2])
			{
				target_id = character_list[i].id;
				wsprintf(buf, "���: %s", character_list[i].nick);
				SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
				SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM) buf);
				break;
			}
		}
		break;
	case SERVERINFO: //serverinfo
		if (data[2] > 19)
			return;
		else if (data[2] == 0)
			i = 1;
		else if (data[2] == 1)
			i = 20;
		else
			i = data[2];
		if (i == 20)
			wsprintf(buf, "���� ä��: %u��", i);
		else
			wsprintf(buf, "���� ä��: %uä��", i);
		SendMessage(g_hList1, LB_DELETESTRING, 0, 0);
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM) buf);
		memset(&character_list, 0, sizeof(character_list)); //���� �ʱ�ȭ	
		if (*(unsigned short*) &data[16] == 1 && size >= 62)
			myid = *(unsigned int*) &data[58];
		break;
	case CHARACTER_APPEAR: //ĳ���� ����
		if (character_list[_countof(character_list) - 1].id != 0) //������ ������� ������
			memset(&character_list, 0, sizeof(character_list)); //���� �ʱ�ȭ
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == 0)
			{
				character_list[i].id = *(unsigned int*) &data[2];
				j = *(unsigned short*) &data[7];
				memcpy(character_list[i].nick, &data[9], j);
				character_list[i].nick[j] = '\0';
				break;
			}
		}
		break;
	case CHARACTER_APPEAR + 1: //ĳ���� ����� ��
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*) &data[2])
			{
				memset(&character_list[i], 0, sizeof(character_list[i]));
				break;
			}
		}
		break;
	case CHATTING: //��ο��� ä�� op�ڵ�
		if (!operating)
			return;
		if (promiscuous_mode == 1) //������� ���� ���� �ֱ׷���;
		{
			if (*(unsigned int*) &data[2] == myid)
				return; 
			else if (*(unsigned int*) &data[2] == lastid || GetTickCount() - tick <= 1200)
				return;
		}
		else if (*(unsigned int*) &data[2] != target_id)
			return;
		
#if 0
		if (FindWindow("#32770", "foo"))
		{
			*(unsigned short*) &packet.data[0] = S_CHATTING;
			*(unsigned int*) &packet.data[2] = GetTickCount();
			i = *(unsigned short*) &data[7];
			*(unsigned short*) &packet.data[6] = i;
			memcpy(&packet.data[8], &data[9], i);
			*(unsigned char*)&packet.data[8 + i + 1] = 0;
			packet.size = 8 + i + 1;
			SendPacket(packet.data, packet.size);
		}
#endif
		//else
		{
			i = *(unsigned short*) &data[7];
			memcpy(buf, &data[9], i);
			buf[i] = '\0';
			Clipboard(buf);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
			Sleep(50);
			PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
			Sleep(50);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(100);
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		}
		tick = GetTickCount();
		lastid = *(unsigned int*) &data[2];
		Sleep(100);
		break;
	}
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
	else {
		if (th->syn != 0u) {
			if ((s = session[th->dport]) == NULL) {
				if ((s = HeapAlloc(GetProcessHeap(), 0u, sizeof(session_t))) == NULL)
					__asm int 3;
				session[th->dport] = s;
			}
			startup = 1;
			s->auth = 0u;
			s->size = 0u;
		}
		else if ((s = session[th->dport]) == NULL)
			return;
		if (i = ih->tot_len, (i = (((i & 0xFF) << 8) | (i >> 8)) - (j = (ih->ihl << 2) + (th->doff << 2))) != 0)
			packet_handler_sub(s, (char *) pkt_data + j + 14u, i);
	}
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

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
	TerminateProcess(GetCurrentProcess(), 0);
	return EXCEPTION_EXECUTE_HANDLER;
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

/*
VOID CALLBACK func(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	DWORD crc;
	
	crc = GetProcessChecksum(pid, offset, len);
	if (crc != origin_crc)
	{
		crc /= 0;
		dbg("%d", crc);
	}
	dbg("crc: %X", crc);
}*/

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char buf[128], name[128], desc[128];
	char nick[13];

	switch (iMessage)
	{
	case WM_HOTKEY:
		if (wParam == 1)
		{
			if (IsDlgButtonChecked(hDlg ,IDC_CHECK1) == BST_UNCHECKED)
				CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			SendMessage(hDlg, WM_COMMAND, IDC_CHECK1, 0);
		}
		else if (wParam == 2)
		{
			if (IsDlgButtonChecked(hDlg, IDC_CHECK2) == BST_UNCHECKED)
				CheckDlgButton(hDlg, IDC_CHECK2, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
			SendMessage(hDlg, WM_COMMAND, IDC_CHECK2, 0);
		}
		break;
	case WM_INITDIALOG:
		//dbg("�ּ� %p", &promiscuous_mode);
		g_hWnd = hDlg; 
		g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
		g_hEdit1 = GetDlgItem(hDlg, IDC_EDIT1); 
		g_hMP = FindWindow("MapleStoryClass", NULL); 
		SendMessage(g_hEdit1, EM_LIMITTEXT, 12, 0);
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM)"���� ä��: NULL");
		SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"���: NULL");
		if (RegisterHotKey(g_hWnd, 1, MOD_ALT | MOD_NOREPEAT, VK_F1) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: ALT + F1", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 2, MOD_ALT | MOD_NOREPEAT, VK_F2) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: ALT + F2", "�˸�", MB_ICONERROR);
		NpfFindAllDevices();
		NpfGetDeviceInfo(adapter_sel + 1, name, desc);
		NpfSetDevice(name);
		if (NpfStart())
			CloseHandle(CreateThread(0, 0, NpfLoop, 0, 0, 0));
		else
		{
			MessageBox(GetDesktopWindow(), "NpfStart() ����", "�˸�", MB_ICONERROR);
			ExitProcess(1);
		}
		//SetTimer(g_hWnd, 1, 1000, func);
		break;
	case WM_CLOSE:
		UnregisterHotKey(g_hWnd, 1);
		UnregisterHotKey(g_hWnd, 2);
		KillTimer(g_hWnd, 1);
		breakloop = 1;
		while (breakloop)
			SwitchToThread();
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK1:
			operating = SendDlgItemMessage(hDlg, IDC_CHECK1, BM_GETCHECK, 0, 0);
			break;
		case IDC_CHECK2:
			promiscuous_mode = SendDlgItemMessage(hDlg, IDC_CHECK2, BM_GETCHECK, 0, 0);
			break;
		case IDC_BUTTON1: //����
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		case IDC_BUTTON2:
			if (!startup)
			{
				MessageBox(GetDesktopWindow(), "ĳ�ü��� ���ٿͼ� ����ä���� ����� �� �Ŀ� �ٽ� �õ����ּ���.", "�˸�", MB_ICONINFORMATION);
				return 0;
			}
			if (GetWindowTextLength(g_hEdit1) == 0)
			{
				MessageBox(GetDesktopWindow(), "��� �г����� �Է����ּ���.", "�˸�", MB_ICONINFORMATION);
				return 0;
			}
			GetWindowText(g_hEdit1, nick, sizeof(nick));
			for (int i = 0; i < _countof(character_list); ++i)
			{
				if (lstrcmpi(nick, character_list[i].nick) == 0)
				{
					target_id = character_list[i].id;
					wsprintf(buf, "���: %s", character_list[i].nick);
					SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
					SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM) buf);
					break;
				}
				if (i == _countof(character_list) - 1)
				{
					MessageBox(GetDesktopWindow(), "�ش� ĳ���Ͱ� �ʿ� �����ϴ�", "�˸�", MB_ICONINFORMATION);
					return 0;
				}
			}
			break;
		}
		break;
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


/*DWORD GetProcessChecksum(DWORD pid, DWORD CodeSectionStart, DWORD CodeSectionLen)
{

	BYTE *Buf_CodeSection = NULL;
	DWORD *pCodeSectionAddress = NULL;
	DWORD checksum = 0;
	DWORD i = 0;

	Buf_CodeSection = (BYTE *) malloc(sizeof(BYTE) * CodeSectionLen);   //�ڵ念�����̰� �󸶳� ���� �𸣹Ƿ� �����Ҵ�.

	pCodeSectionAddress = (DWORD *) CodeSectionStart;

	//�ڵ念�� ���۰����� �ڵ念�� ��ü ���̿� ���� ���� Buf_CodeSection �� ����
	Toolhelp32ReadProcessMemory(pid, pCodeSectionAddress, Buf_CodeSection, CodeSectionLen, NULL);

	checksum = crcsum32(0xBADF00Du, Buf_CodeSection, CodeSectionLen);


	free(Buf_CodeSection);
	return checksum;
}

int ParsePEFormat(const char* filename, DWORD *offset, DWORD *len)
{
	HANDLE	m_hImgFile = NULL;
	HANDLE	m_hImgMap = NULL;
	LPBYTE	m_pImgView = NULL;
	WORD	NumberOfSection = 0;
	DWORD   OptionalHeader_SizeOfCode = 0;
	DWORD OptionalHeader_VirtualAddress = 0;
	DWORD textSection_Offset = 0;
	DWORD textSection_len = 0;


	m_hImgFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hImgFile == NULL)
	{
		MessageBox(0, "CreateFile() ����!", 0, 0);
		return -1;
	}

	m_hImgMap = CreateFileMapping(m_hImgFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (m_hImgMap == NULL)
	{
		MessageBox(0, "CreateFileMapping() ����!", 0, 0);
		return -1;
	}
	m_pImgView = (LPBYTE) MapViewOfFile(m_hImgMap, FILE_MAP_READ, 0, 0, 0);
	if (m_pImgView == NULL)
	{
		MessageBox(0, "MapViewOfFile() ����!", 0, 0);
		return -1;
	}



	PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER) m_pImgView;
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE) return -4;

	PIMAGE_NT_HEADERS pINH = (PIMAGE_NT_HEADERS) (m_pImgView + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
	{
		MessageBox(0, "IMAGE_NT_SIGNATURE ����!", 0, 0);
		return -1;
	}


	PIMAGE_OPTIONAL_HEADER32 pIOH = (PIMAGE_OPTIONAL_HEADER32) (m_pImgView + pIDH->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER));
	OptionalHeader_SizeOfCode = pIOH->SizeOfCode;				//.text section ã������ 

	PIMAGE_FILE_HEADER pIFH = (PIMAGE_FILE_HEADER) (m_pImgView + pIDH->e_lfanew + sizeof(DWORD));

	NumberOfSection = pIFH->NumberOfSections;


	//ù��° Section Header�� �����ּ� = OptionalHeader + OptionalHeader ũ��. 
	PIMAGE_SECTION_HEADER pISH = (PIMAGE_SECTION_HEADER) ((char *) pIOH + sizeof(IMAGE_OPTIONAL_HEADER32));


	if (OptionalHeader_SizeOfCode == pISH->SizeOfRawData)    // ù��° SectionHeader�� SizeofRawData�� OptionalHeader�� sizeofCode�� 
	{														//���� ���ٸ� �� SectionHeader�� .text ������ ����̴�. 
		// .textbss ���ǰ��� ���еǰ� ã�ƾ� �Ѵ�. 

		textSection_Offset = pISH->VirtualAddress;			//.text ���������� ����޸𸮻��� ImageBase�κ����� offset�� VirtualAddress�� ����.
		textSection_len = pISH->Misc.VirtualSize;			// .text ���������� .text������ ũ�� ����. 
	}
	else
	{
		for (int i = 0; i<NumberOfSection - 1; i++)				//�ٸ��ٸ� �ι�°,����° ���������� SectionHeader�� �Ѿ��. �Ʊ� ���س��� ���ǰ�����ŭ
		{
			pISH = (PIMAGE_SECTION_HEADER) ((char *) pISH + sizeof(IMAGE_SECTION_HEADER));  //SectionaHeader��ŭ �����ּҸ� �ű��. ����SectionHeader�� ����Ű����

			if (OptionalHeader_SizeOfCode == pISH->SizeOfRawData) //���� �����ش��� SizeOfRawData���� OptionalHeader�� SizeofCode���� ���Ѵ�
			{
				textSection_Offset = pISH->VirtualAddress;  //.text ���������� ����޸𸮻��� ImageBase�κ����� offset�� VirtualAddress�� ����.
				textSection_len = pISH->Misc.VirtualSize;	// .text ���������� .text������ ũ�� ����.
				break;
			}

		}
	}



	*offset = pIOH->ImageBase + textSection_Offset;// ����޸𸮻󿡸��εǾ����� .text section�� ���� offset
	*len = textSection_len;   //����޸𸮻� ���εǾ����� .text section ũ��
	//	*offset = pIOH->ImageBase + pIOH->BaseOfCode;   //ù��° �ڵ念���� ���� offset ����׸�忡�� .textbss section�� �ɼ����ִ�. 
	//	*len = pIOH->SizeOfCode;   // �ڵ念���� ũ��

	UnmapViewOfFile(m_pImgView);
	CloseHandle(m_hImgMap);
	CloseHandle(m_hImgFile);


	return 0;
}

*/

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

	
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);


	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(GetDesktopWindow(), "������ ������ �ƴմϴ�.\n������ �������� �ٽ� �������ּ���.", "�˸�", MB_ICONERROR);
		return -1;
	}

	if (FindWindow("#32770", "���� ��� ä�� ����ġ��"))
	{
		SetForegroundWindow(FindWindow("#32770", "���� ��� ä�� ����ġ��"));
		return 0;
	}


	/*GetModuleFileName(GetModuleHandle(0), buf, sizeof(buf));
	if (ParsePEFormat(buf, &offset, &len) == -1)
		return -1; 
	pid = GetCurrentProcessId();

	origin_crc = GetProcessChecksum(pid, offset, len);
	dbg("������: %X", origin_crc);
	dbg("����: %X %X", offset, len);
	*/
	aes256_init(&ctx, (unsigned char*) aeskey);
	CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();
	
	return 0;
}