#include "common.h"

extern aes256_context ctx;
unsigned int adapter_sel, breakloop;
unsigned int SERVERINFO, EMPLOYEE_LEAVE, CHARACTER_INFO, 확성기류, 메가폰;
HWND g_hWnd, g_hList1, g_hList2, g_hList3, g_hList4, g_hList5, g_hList6, g_hButton1, g_hButton2, g_hButton5;
char version [] = "1.12";

ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];
charinfo_t character_list[200];
employeeinfo_t employee_list[40];
employeeinfo_t notify_list[40];

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{ 
	TerminateProcess(GetCurrentProcess(), 0);
	return EXCEPTION_EXECUTE_HANDLER;
}

void getuuid(char *data)
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		if (*gen_uuid(data))
		{
			*(int *)&data[128] = ~_byteswap_ulong(((int *)data)[0]);
			*(int *)&data[132] = ~_byteswap_ulong(((int *)data)[1]);
			*(int *)&data[136] = ~_byteswap_ulong(((int *)data)[2]);
			*(int *)&data[140] = ~_byteswap_ulong(((int *)data)[3]);
			*(int *)&data[144] = ~_byteswap_ulong(((int *)data)[4]);
			*(int *)&data[148] = ~_byteswap_ulong(((int *)data)[5]);
			*(int *)&data[152] = ~_byteswap_ulong(((int *)data)[6]);
			*(int *)&data[156] = ~_byteswap_ulong(((int *)data)[7]);
			*(int *)&data[160] = ~_byteswap_ulong(((int *)data)[8]);
			*(int *)&data[164] = ~_byteswap_ulong(((int *)data)[9]);
		} 
	}
	VIRTUALIZER_SHARK_WHITE_END
}

void vmemcpy(void *addr, const void *data, unsigned int size)
{
	DWORD protect;

	VIRTUALIZER_TIGER_BLACK_START
	{
		if (VirtualProtect(addr, size, PAGE_READWRITE, &protect)) 
		{
			memcpy(addr, data, size);
			VirtualProtect(addr, size, protect, &protect);
			FlushInstructionCache(GetCurrentProcess(), addr, size);
		}
	}
	VIRTUALIZER_TIGER_BLACK_END
}



#if 0
void SendPacket(const void *data, unsigned int size)
{
	COPYDATASTRUCT cds;

	HWND hwnd;
	hwnd = FindWindow("MapleStoryClass", NULL);
	if (hwnd)
	{
		cds.dwData = 0x02100713;
		cds.cbData = size;
		cds.lpData = (void *) data;
		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM) &cds);
	}
}

void _SendPacket(const void *data, unsigned int size)
{
	COPYDATASTRUCT cds;

	HWND hwnd;
	hwnd = FindWindow("MapleStoryClass", NULL);
	if (hwnd)
	{
		cds.dwData = 0x123;
		cds.cbData = size;
		cds.lpData = (void *) data;
		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM) &cds);
	}
}
#endif




void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey [] = "제시카ㅎㅎ";

	VIRTUALIZER_TIGER_BLACK_START
	{
		for (int i = 0; i != size; ++i)
		((char *) out)[i] = ((char *) in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
	}
	VIRTUALIZER_TIGER_BLACK_END
}

u_long host2ip(const char *host)
{
	u_long ipaddr;
	struct hostent *h;

	if ((ipaddr = inet_addr(host)) == INADDR_NONE)
		if ((h = gethostbyname(host)) != NULL)
			ipaddr = *(u_long *) h->h_addr;


	return ipaddr;
}



SOCKET ConnectToServer()
{
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	FD_SET fds;
	struct timeval tv;

	VIRTUALIZER_TIGER_BLACK_START
	{
		hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		ioctlsocket(hSock, FIONBIO, (u_long *)"\x01\x00\x00\x00");
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = inet_addr("49.174.170.30");
		servAdr.sin_port = htons(atoi("6969"));

		FD_ZERO(&fds);
		FD_SET(hSock, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 300000;
		connect(hSock, (SOCKADDR*) &servAdr, sizeof(servAdr));

		ioctlsocket(hSock, FIONBIO, (u_long *)"\x00\x00\x00\x00");
		select(0, 0, &fds, 0, &tv);
		if (!FD_ISSET(hSock, &fds))
		{
			closesocket(hSock);
			hSock = INVALID_SOCKET;
		}

	}
	VIRTUALIZER_TIGER_BLACK_END
	return hSock;

}


SOCKET _ConnectToServer()
{
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	FD_SET fds;
	struct timeval tv;

	VIRTUALIZER_TIGER_BLACK_START
	{
		hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		ioctlsocket(hSock, FIONBIO, (u_long *)"\x01\x00\x00\x00");
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = inet_addr("192.168.219.196");
		servAdr.sin_port = htons(atoi("6969"));

		FD_ZERO(&fds);
		FD_SET(hSock, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 300000;
		connect(hSock, (SOCKADDR*) &servAdr, sizeof(servAdr));

		ioctlsocket(hSock, FIONBIO, (u_long *)"\x00\x00\x00\x00");
		select(0, 0, &fds, 0, &tv);
		if (!FD_ISSET(hSock, &fds))
		{
			closesocket(hSock);
			hSock = INVALID_SOCKET;
		}

	}
		VIRTUALIZER_TIGER_BLACK_END
		return hSock;

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


int hex2data(unsigned char *data, const unsigned char *hexstring, unsigned int len)
{
	unsigned const char *pos = hexstring;
	char *endptr;
	size_t count = 0;

	if ((hexstring[0] == '\0') || (strlen(hexstring) % 2))
		return -1;

	for (count = 0; count < len; count++)
	{
		char buf[5] = { '0', 'x', pos[0], pos[1], 0 };
		data[count] = (unsigned char)strtol(buf, &endptr, 0);
		pos += 2 * sizeof(char);

		if (endptr[0] != '\0'); //return -1(non-hexadecimal chararcter encountered)
		
	}

	return 0;
}

int FileNameCheck(char *fileName)
{
	int i;
	char myFileName[MAX_PATH];

	GetModuleFileName(NULL, myFileName, sizeof(myFileName));
	for (i = strlen(myFileName); myFileName[i] != '\\'; --i);
	if (lstrcmpi(&myFileName[i + 1], fileName))
	{
		DeleteFile(fileName);
		MoveFile(&myFileName[i + 1], fileName);
		return FALSE;
	}


	return TRUE;

}


#pragma optimize("", off)

unsigned int rand_mix(unsigned int a, unsigned int b)
{
	unsigned int ret;

	VIRTUALIZER_SHARK_WHITE_START
	{
		ret = a ^ b ^ 0xBADF00Du;
	}
	VIRTUALIZER_SHARK_WHITE_END
	return ret;
}
#pragma optimize("", on)



void KillProcess()
{
	HANDLE hSnapshot, hThread;
	THREADENTRY32 te32;
	DWORD current_processid, current_threadid;

	VIRTUALIZER_SHARK_WHITE_START
	{
		VirtualProtect(GetModuleHandle(NULL), 10000000, PAGE_EXECUTE_READWRITE, 0);
		memset(GetModuleHandle(NULL), 0, 10000000);
		if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)) != INVALID_HANDLE_VALUE)
		{
			current_processid = GetCurrentProcessId();
			current_threadid = GetCurrentThreadId();
			te32.dwSize = sizeof(THREADENTRY32);
			if (Thread32First(hSnapshot, &te32))
			{
				do
				{
					if (te32.th32OwnerProcessID == current_processid)
					{
						if (te32.th32ThreadID != current_threadid)
						{
							TerminateThread((hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID)), 0);
							CloseHandle(hThread);
						}
					}
				} while (Thread32Next(hSnapshot, &te32));
			}
			CloseHandle(hSnapshot);
		}
		TerminateProcess(GetCurrentProcess(), 0);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	unsigned int i;
	UINT rand1, rand2;
	SOCKET hSock; 
	char data[2048];
	char username[128];
	DWORD username_size = sizeof(username) - 1;
	WSADATA wsadata;
	int size;
	unsigned char aeskey[32];
	char message[2048]; 

	
	VIRTUALIZER_SHARK_WHITE_START
	{
		SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);
 
		if (IsUserAnAdmin() == FALSE)
		{ 
			MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR);
			KillProcess();
		} 

		if (WSAStartup(MAKEWORD(2, 2), &wsadata))
		{
			MessageBox(0, "WSAStartup() error", 0, 0);
			KillProcess();
		}
	
		 
		if (!FileNameCheck("리듬 상점유틸.exe"))
			KillProcess();

		if ((CreateMutex(NULL, FALSE, "ShopUtilMultiMtx")) == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			SetForegroundWindow(FindWindow("#32770", "리듬 상점유틸"));
			KillProcess();
		}
		 
	
		hSock = ConnectToServer();
		if (hSock == INVALID_SOCKET)
		{
			hSock = _ConnectToServer();
			if (hSock == INVALID_SOCKET)
			{
				MessageBox(0, "서버 닫힘!", "알림", MB_ICONERROR);
				KillProcess();
			}
		}
		getuuid(data);

		init_rand(GetCurrentProcessId() ^ 0xBADF00Du);

		rand1 = gen_rand();
		rand2 = gen_rand();

		GetUserName(username, &username_size);
		size = wsprintf(message, "RU %u %u %s %s %s", rand1, rand2, data, username, version);
		RFCrypt(size, message, message);
		send(hSock, message, size, 0);

		size = recv(hSock, message, sizeof(message) - 1, 0);
		RFCrypt(size, message, message);
		closesocket(hSock);

		if (size != SOCKET_ERROR)
		{
			message[size] = '\0';
			if (*message == 'O')
			{
				sscanf(message, "%*s %u %s", &i, data);
				if (rand_mix(rand1, rand2) != i)
					KillProcess();


				if (hex2data(aeskey, data, 32) == -1)
				{
					MessageBox(0, "서버에서 받아온 AESkey에 오류가 있습니다!", "알림", MB_ICONERROR);
					KillProcess();
				}
				aes256_init(&ctx, aeskey);
				CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
				MessageLoop();
			}
			else
			{
				if (*message == 'X')
					MessageBox(0, "하드인증 실패!", "알림", MB_ICONERROR);
				else if (*message == 'N')
					MessageBox(0, "기간 만료!", "알림", MB_ICONERROR);
				else if (*message == 'E')
					MessageBox(0, "최신 버전이 아닙니다.\n파일을 새로 받아주세요.", "알림", MB_ICONERROR);
				else if (*message == '?')
					MessageBox(0, "?", "알림", MB_ICONERROR);
				KillProcess();
			}
		}
		else
		{
			MessageBox(0, "recv() error!", "알림", MB_ICONERROR);
			KillProcess();
		}

	
	}
	VIRTUALIZER_SHARK_WHITE_END 

	return 0;
}



	

void packet_handle(unsigned char *data, unsigned int size)
{
	LVITEM li;
	int channel, room;
	char buf[1024], guild[13];
	unsigned char packet[1024];
	char textData[256], *p, *q;
	unsigned int textSize, packet_size;
	SYSTEMTIME st;
	int i, j, k, l, idx;
	static DWORD tick;
	LVFINDINFO fi;
	
	 
	if (*(unsigned short*)data == SERVERINFO)
	{
		if (data[2] > 19)
			return;
		else if (data[2] == 0)
			i = 1;
		else if (data[2] == 1)
			i = 20;
		else
			i = data[2];
		if (i == 20)
			wsprintf(buf, "현재 채널 : %u세", i);
		else
			wsprintf(buf, "현재 채널 : %u채널", i);
		SendMessage(g_hList4, LB_DELETESTRING, 0, 0);
		SendMessage(g_hList4, LB_INSERTSTRING, 0, (LPARAM)buf);
		memset(&character_list, 0, sizeof(character_list)); //전부 초기화
		memset(&employee_list, 0, sizeof(employee_list));
		SendMessage(g_hList1, LB_RESETCONTENT, 0, 0);
		ListView_DeleteAllItems(g_hList6);
		tick = GetTickCount();
	}
	else if (*(unsigned short*)data == EMPLOYEE_LEAVE - 1) //고상 정보 패킷
	{
		if (employee_list[_countof(employee_list) - 1].id != 0) //끝까지 비어있지 않으면
		{
			memset(&employee_list, 0, sizeof(employee_list)); //전부 초기화
			SendMessage(g_hList1, LB_RESETCONTENT, 0, 0);
		}
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (employee_list[i].id == 0) //비어있는 곳에서 break
			{
				employee_list[i].id = *(unsigned int*) &data[2];
				j = *(unsigned short*) &data[16]; //닉네임 사이즈
				memcpy(employee_list[i].nick, &data[18], j);
				employee_list[i].nick[j] = '\0';
				employee_list[i].shopid = *(unsigned int*) &data[16 + 1 + j + 2];
				wsprintf(buf, "%s", employee_list[i].nick);
				SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM) buf), 0);
				if (GetTickCount() - tick > 1000)
				{
					GetLocalTime(&st);
					wsprintf(buf, "%s %02u시%02u분%02u초 %s님이 고상 열음", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, employee_list[i].nick);
					SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM) buf), 0);
				}
				break;
			}
		}		
	}
	else if (*(unsigned short*) data == EMPLOYEE_LEAVE)
	{
		for (i = 0; i < _countof(employee_list); ++i)
		{
			if (*(unsigned int*) &data[2] == employee_list[i].id)
			{
				GetLocalTime(&st);
				SendMessage(g_hList1, LB_DELETESTRING, SendMessage(g_hList1, LB_FINDSTRINGEXACT, -1, (LPARAM) employee_list[i].nick), 0);
				wsprintf(buf, "%s %02u시%02u분%02u초 %s님이 고상 닫음", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, employee_list[i].nick);
				SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM) buf), 0);
				memset(&employee_list[i], 0, sizeof(employee_list[i]));
				break;
			}
		}
	}
	else if (*(unsigned short*)data == CHARACTER_INFO)
	{
		if (character_list[_countof(character_list) - 1].id != 0)
			memset(&character_list, 0, sizeof(character_list));
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == 0)
			{
				character_list[i].id = *(unsigned int*) &data[2];
				j = *(unsigned short*) &data[7];
				memcpy(character_list[i].nick, &data[9], j);
				character_list[i].nick[j] = '\0';

				idx = ListView_GetItemCount(g_hList6);
				li.mask = LVIF_TEXT;
				li.iItem = idx;
				li.pszText = character_list[i].nick;
				li.cchTextMax = strlen(character_list[i].nick);
				ListView_InsertItem(g_hList6, &li);

				k = 7 + j + 4;
				if ((j = *(unsigned short*) &data[k]) != 0)
				{
					if (j > 12)
					{
						l = 0;
						while (j < 4 || j > 12)
							j = *(unsigned short*) &data[k + ++l];
						memcpy(guild, &data[k + l + 1], j);
					}
					else
						memcpy(guild, &data[k + 2], j);
					guild[j] = '\0';
					ListView_SetItemText(g_hList6, idx, 1, guild);
				}
				else
					ListView_SetItemText(g_hList6, idx, 1, "");
				break;
			}
		}
		for (i = 0; i < _countof(notify_list); ++i)
		{

			if (*(unsigned int*) &data[2] == notify_list[i].id)
			{
				GetLocalTime(&st);
				PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
				wsprintf(buf, "%s %02u시%02u분%02u초 %s님이 맵에 나타남", st.wHour >= 12 ? "오후" : "오전", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, notify_list[i].nick);
				SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM)buf), 0);
				SendMessage(g_hList3, LB_SETITEMDATA, i, notify_list[i].shopid);
				break;
			}
		}
	}
	else if (*(unsigned short*) data == CHARACTER_INFO + 1) //캐릭터 사라질 때
	{
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*) &data[2])
			{
				fi.flags = LVFI_STRING | LVFI_SUBSTRING;
				fi.psz = character_list[i].nick;
				fi.vkDirection = VK_DOWN;
				j = ListView_FindItem(g_hList6, -1, &fi);
				if (j != -1) 
					ListView_DeleteItem(g_hList6, j);
				memset(&character_list[i], 0, sizeof(character_list[i]));
				break;
			}
		}
	}
	else if (*(unsigned short*) data == 확성기류 || *(unsigned short*) data == 메가폰)
	{
		if (!SendDlgItemMessage(g_hWnd, IDC_CHECK1, BM_GETCHECK, BST_CHECKED, 0))
			return;
		if (*(unsigned short*) data == 확성기류)
		{
			if (data[2] != 0x2/*확성기*/ && data[2] != 0x3/*고성능 확성기*/ && data[2] != 0x8/*아이템 확성기*/)
				return;
			textSize = *(unsigned short*) &data[3];
			memcpy(textData, &data[5], textSize);
		}
		else
		{
			j = 6;
			textSize = 0;
			while (1)
			{
				i = *(unsigned short*) &data[j];
				if (data[j + 2] == 0)
					break;
				memcpy(buf, &data[j + 2], i);
				buf[i] = '\0';
				strcat(textData, buf);
				j = i + j + 2;
				textSize += i;
			}
		}
		textData[textSize] = '\0';
		strcpy(buf, textData);
		p = _strupr(buf);
		if (!strstr(p, "GOGO"))
			return;
		p = strchr(buf, '-') - 1;
		if (p == NULL)
			return;
		i = 0;
		while (p[i] == ' ')
			--i;
		p = &p[i];
		if (isdigit(p[-1]))
		{
			q = &p[-1];
			q[2] = '\0';
			channel = atoi(q);
		}
		else
		{
			p[1] = '\0';
			channel = atoi(p);
		}
		if (channel < 1 || channel >= 20)
			return;
		strcpy(buf, textData);
		p = strrchr(buf, '-') + 1;
		i = 0;
		while (p[i] == ' ')
			++i;
		p = &p[i];
		if (isdigit(p[1]))
		{
			p[2] = '\0';
			room = atoi(p);
		}
		else
		{
			p[1] = '\0';
			room = atoi(p);
		}
		if (room < 1 || 12 < room)
			return;
		SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM) textData), 0);
		packet_size = 0;
		if (channel == 1)
			channel = 0;
		i = 910000000;
		i += room;
		*(unsigned short*) &packet[packet_size] = 0x141, packet_size += 2;
		packet[packet_size] = channel, ++packet_size;
		*(unsigned int*) &packet[packet_size] = i, packet_size += 4;
		*(unsigned int*) &packet[packet_size] = GetTickCount(), packet_size += 4;
		//SendPacket(packet, packet_size);
		PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		PostMessage(FindWindow("MapleStoryClass", NULL), WM_SYSCOMMAND, SC_RESTORE, 0);
		SetForegroundWindow(FindWindow("MapleStoryClass", NULL));
		
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
	}
	else
		i = 0;

	k = s->recv_iv;

	for (; (size - i) >= (j = *(unsigned __int16 *)&data[i] ^ *(unsigned __int16 *)&data[i + 2u]) + 4u; i += j + 4u) 
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

	for (i = 1, name = npf_devicelist; *name; ++i) 
	{
		if (strcmp(name, device_name) == 0) 
		{
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
		{
			if (PacketSetBpf(adapter, &bpf_code) != FALSE)
			{
				if (PacketSetBuff(adapter, 1048576u) != FALSE)
				{
					if (PacketSetHwFilter(adapter, 1/*NDIS_PACKET_TYPE_DIRECTED*/) != FALSE)
					{
						npf_adapter = adapter;
						return 1u;
					}
					PacketCloseAdapter(adapter);
				}
			}
		}
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
	{
		if (WaitForSingleObject(adapter->ReadEvent, 1) == WAIT_OBJECT_0)
		{
			if (ReadFile(adapter->hFile, pkt_data, sizeof(pkt_data), &pkt_size, NULL))
			{
				for (i = 0, j = pkt_size; i < j; i += (((struct bpf_hdr *)&pkt_data[i])->bh_caplen + ((struct bpf_hdr *)&pkt_data[i])->bh_hdrlen + 3u) & ~3)
					packet_handler(NULL, (struct pcap_pkthdr *)&pkt_data[i], &pkt_data[i + ((struct bpf_hdr *)&pkt_data[i])->bh_hdrlen]);
			}
		}
	}
}

DWORD WINAPI NpfLoop(LPVOID arg)
{
	while (!breakloop)
		NpfCheckMessage();
	NpfStop();

	breakloop = 0;
	return 0;
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
			SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)desc);
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


void ReadINI()
{
	char INIPath[128];

	GetModuleFileName(NULL, INIPath, sizeof(INIPath));

	memcpy(&INIPath[strlen(INIPath) - 3], "ini", 3);

	if (!(SERVERINFO = GetPrivateProfileInt("PACKET", "SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SERVERINFO", "", INIPath);
	if (!(EMPLOYEE_LEAVE = GetPrivateProfileInt("PACKET", "EMPLOYEE_LEAVE", 0, INIPath)))
		WritePrivateProfileString("PACKET", "EMPLOYEE_LEAVE", "", INIPath);
	if (!(CHARACTER_INFO = GetPrivateProfileInt("PACKET", "CHARACTER_INFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "CHARACTER_INFO", "", INIPath);
	//if (!(확성기류 = GetPrivateProfileInt("PACKET", "확성기류", 0, INIPath)))
		//WritePrivateProfileString("PACKET", "확성기류", "", INIPath);
	//if (!(메가폰 = GetPrivateProfileInt("PACKET", "메가폰", 0, INIPath)))
		//WritePrivateProfileString("PACKET", "메가폰", "", INIPath);
}







INT_PTR CALLBACK OnInitDialog(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	LVCOLUMN col;
	char name[128], desc[128];

	g_hWnd = hDlg;
	g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
	g_hList2 = GetDlgItem(hDlg, IDC_LIST2);
	g_hList3 = GetDlgItem(hDlg, IDC_LIST3);
	g_hList4 = GetDlgItem(hDlg, IDC_LIST4);
	g_hList5 = GetDlgItem(hDlg, IDC_LIST5);
	g_hList6 = GetDlgItem(hDlg, IDC_LIST6);
	g_hButton1 = GetDlgItem(hDlg, IDC_BUTTON1);
	g_hButton2 = GetDlgItem(hDlg, IDC_BUTTON2);
	g_hButton5 = GetDlgItem(hDlg, IDC_BUTTON5);
	EnableWindow(g_hButton1, FALSE);
	EnableWindow(g_hButton2, FALSE);
	EnableWindow(g_hButton5, FALSE);
	SendMessage(g_hList4, LB_INSERTSTRING, 0, (LPARAM)"현재 채널 : NULL");
	SendMessage(g_hList5, LB_INSERTSTRING, 0, (LPARAM)"ID : 0");
	SendMessage(g_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL, IDI_APPLICATION));

	ListView_SetExtendedListViewStyle(g_hList6, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;
	col.cx = 110;
	col.pszText = "닉네임";
	col.cchTextMax = strlen("닉네임");
	col.iSubItem = 0;
	SendMessage(g_hList6, LVM_INSERTCOLUMN, 0, (LPARAM) &col);

	col.cx = 110;
	col.pszText = "길드";
	col.cchTextMax = strlen("길드");
	SendMessage(g_hList6, LVM_INSERTCOLUMN, 1, (LPARAM) &col);


	ReadINI();
	NpfFindAllDevices();
	NpfGetDeviceInfo(adapter_sel + 1, name, desc);
	NpfSetDevice(name);
	if (NpfStart())
		CreateThread(0, 0, NpfLoop, 0, 0, 0);
	else
	{
		MessageBox(0, "NpfStart() 실패", "알림", MB_ICONERROR);
		ExitProcess(1);
	}
	return 0;
}




int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{ 
	LVITEMW li;
	wchar_t buf[16], buf2[16];
	
	li.iSubItem = ((sortinfo_t*)lParamSort)->iSubItem;
	li.cchTextMax = sizeof(buf);
	li.pszText = buf;
	
	SendMessageW(g_hList6, LVM_GETITEMTEXTW, lParam1, (LPARAM)&li);
	

	li.iSubItem = ((sortinfo_t*) lParamSort)->iSubItem;
	li.cchTextMax = sizeof(buf2);
	li.pszText = buf2;

	SendMessageW(g_hList6, LVM_GETITEMTEXTW, lParam2, (LPARAM) &li);
	
	if (((sortinfo_t*) lParamSort)->iSubItem == 1)
	{
		if (StrCmpLogicalW(buf, buf2) != 0 && wcscmp(buf, L"") == 0)
			return 1;
		else if (StrCmpLogicalW(buf, buf2) != 0 && wcscmp(buf2, L"") == 0)
			return -1;
	}
	return ((sortinfo_t*)lParamSort)->ascending ? StrCmpLogicalW(buf, buf2) : -StrCmpLogicalW(buf, buf2);

}



INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	unsigned int i, j, k;
	char buf[32];
	static sortinfo_t si;

	switch (iMessage)
	{
	case WM_NOTIFY:
		if (((LPNMHDR) lParam)->hwndFrom == g_hList6)
		{
			if (((LPNMLISTVIEW) lParam)->hdr.code == LVN_COLUMNCLICK)
			{
				si.ascending = !si.ascending;
				si.iSubItem = ((LPNMLISTVIEW) lParam)->iSubItem;
				ListView_SortItemsEx(g_hList6, (PFNLVCOMPARE) CompareFunc, (LPARAM) &si);
				return TRUE;
			}
		}	
		break;
	case WM_LBUTTONDOWN:
		EnableWindow(g_hButton1, FALSE);
		EnableWindow(g_hButton2, FALSE);
		EnableWindow(g_hButton5, FALSE);
		SendMessage(g_hList1, LB_SELITEMRANGE, 0, SendMessage(g_hList1, LB_GETCOUNT, 0, 0));
		SendMessage(g_hList2, LB_SELITEMRANGE, 0, SendMessage(g_hList2, LB_GETCOUNT, 0, 0));
		break;
	case WM_INITDIALOG:
		OnInitDialog(hDlg, iMessage, wParam, lParam);
		return 0;
	case WM_CLOSE:
		breakloop = 1;
		while (breakloop)
			SwitchToThread();
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1: //추가
			for (i = 0; i < _countof(employee_list); ++i) //항목 개수만큼 반복
			{
				if (SendMessage(g_hList1, LB_GETSEL, i, 0)) //i번째 인덱스가 선택돼있다면
				{
					SendMessage(g_hList1, LB_GETTEXT, i, (LPARAM)buf);
					for (j = 0; j < (unsigned int)SendMessage(g_hList1, LB_GETCOUNT, 0, 0); ++j)
					{
						if (strcmp(buf, employee_list[j].nick) == 0)
						{
							if (notify_list[39].id != 0)
							{
								MessageBox(0, "알림 울릴 캐릭터는 40개까지 지정할 수 있습니다.", "알림", 0);
								return 0;
							}
							for (k = 0; k < _countof(employee_list); ++k)
								if (notify_list[k].id == employee_list[j].id) // 이미 있는 id라면
									break;
							if (notify_list[k].id == employee_list[j].id) // 이미 있는 id라면
								break;
							for (k = 0; k < _countof(notify_list); ++k)
								if (notify_list[k].id == 0)
									break;
							notify_list[k].id = employee_list[j].id;
							notify_list[k].shopid = employee_list[j].shopid;
							strcpy(notify_list[k].nick, employee_list[j].nick);
							SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);
							break;
						}
					}
				}
			}
			SendMessage(g_hList1, LB_SELITEMRANGE, 0, SendMessage(g_hList1, LB_GETCOUNT, 0, 0));
			break;
		case IDC_BUTTON2: //제거
			k = SendMessage(g_hList2, LB_GETCOUNT, 0, 0);
			for (i = k - 1;i>=0;--i)
			{
				if (SendMessage(g_hList2, LB_GETSEL, i, 0))
				{
					SendMessage(g_hList2, LB_GETTEXT, i, (LPARAM)buf);
					SendMessage(g_hList2, LB_DELETESTRING, i, 0);
					for (j = 0; j < _countof(notify_list); ++j)
					{
						if (strcmp(buf, notify_list[j].nick) == 0)
						{
							memset(&notify_list[j], 0, sizeof(notify_list[j]));
							break;
						}
					}
					
				}
			}
			break;
		case IDC_BUTTON3:
			SendMessage(g_hList2, LB_RESETCONTENT, 0, 0);
			memset(&notify_list, 0, sizeof(notify_list));
			break;
		case IDC_BUTTON4:
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		case IDC_BUTTON5:
			wsprintf(buf, "ID : 0x%X", employee_list[SendMessage(g_hList1, LB_GETCURSEL, 0, 0)].id);
			SendMessage(g_hList5, LB_DELETESTRING, 0, 0);
			SendMessage(g_hList5, LB_INSERTSTRING, 0, (LPARAM)buf);
			break;
		case IDC_LIST1:
			if (SendMessage(g_hList1, LB_GETSELCOUNT, 0, 0))
			{ 
				if (SendMessage(g_hList1, LB_GETSELCOUNT, 0, 0) == 1)
					EnableWindow(g_hButton5, TRUE);
				else
					EnableWindow(g_hButton5, FALSE);
				EnableWindow(g_hButton1, TRUE);
				switch (HIWORD(wParam))
				{
				case LBN_DBLCLK:
					/*SendMessage(g_hList1, LB_GETTEXT, (WPARAM)SendMessage(g_hList1, LB_GETCURSEL, 0, 0), (LPARAM)buf);
					for (i = 0; i < 40; ++i)
						if (strcmp(buf, employee_list[i].nick) == 0)
							break;
					_itoa(employee_list[i].shopid, buf, 10);*/
					//SendMessage(FindWindow("MapleStoryClass", NULL), WM_USER + 7, 0, 0);
					//_SendPacket(buf, strlen(buf));
					break;
				}
			}
			break;
		case IDC_LIST2:
			if (SendMessage(g_hList2, LB_GETSELCOUNT, 0, 0))
			{
				EnableWindow(g_hButton2, TRUE);
				switch (HIWORD(wParam))
				{
				case LBN_DBLCLK:
					/*SendMessage(g_hList2, LB_GETTEXT, (WPARAM)SendMessage(g_hList2, LB_GETCURSEL, 0, 0), (LPARAM)buf);
					for (i = 0; i < 40; ++i)
						if (strcmp(buf, notify_list[i].nick) == 0)
							break;*/
					//_itoa(notify_list[i].shopid, buf, 10);
					//SendMessage(FindWindow("MapleStoryClass", NULL), WM_USER + 7, 0, 0);
					//_SendPacket(buf, strlen(buf));
					break;
				}
			}
			break;
		case IDC_LIST3:
			if (SendMessage(g_hList3, LB_GETSELCOUNT, 0, 0))
			{
				switch (HIWORD(wParam))
				{
				case LBN_DBLCLK:
					//_itoa(SendMessage(g_hList3, LB_GETITEMDATA, (WPARAM)SendMessage(g_hList3, LB_GETCURSEL, 0, 0), 0), buf, 10);
					//SendMessage(FindWindow("MapleStoryClass", NULL), WM_USER + 7, 0, 0);
					//_SendPacket(buf, strlen(buf));
					break;
				}
			}
			break;
		case IDC_CHECK1:
			CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			MessageBox(hDlg, "현재로서는 사용할 수 없습니다", "알림", MB_ICONINFORMATION);
			break;
		}
		break;

	}
	return 0;
}
