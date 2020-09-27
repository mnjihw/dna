#include "common.h"

char version[] = "1.35";

#define INTERNAL_IP "192.168.219.100"
#define EXTERNAL_IP "182.211.75.197"
 
SOCKET g_clntSock = INVALID_SOCKET;
char packet_data[4096];
int packet_size;

extern aes256_context ctx;
unsigned int adapter_sel, breakloop, operating;
unsigned int R_SERVERINFO, R_EMPLOYEE_LEAVE, R_CHARACTER_APPEAR, R_SHOPINFO, R_TYPEOFLOUDSPEAKERS, R_MEGAPHONE;
unsigned int S_CHANGEROOM, S_CHATTING, S_WHISPER;
HWND g_hWnd, g_hList1, g_hList2, g_hList3, g_hList4, g_hList5, g_hList6, g_hButton1, g_hButton2, g_hButton5, g_hMP, g_hChattingDlg;
char INIPath[256], username[128], time_remaning[16];

charinfo_t character_list[200];
employeeinfo_t employee_list[40];
employeeinfo_t notify_list[40];

void ListBox_Modify(HWND hwnd, int sel, char *str)
{
	SendMessage(hwnd, LB_DELETESTRING, sel, 0);
	SendMessage(hwnd, LB_INSERTSTRING, sel, (LPARAM)str);
}

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



__declspec(naked) void nop1()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


void SendPacket(const void *data, unsigned int size)
{
	COPYDATASTRUCT cds;
	

	g_hMP = FindWindow("MapleStoryClass", NULL);
	if (g_hMP)
	{
		cds.dwData = 0x02100713;
		cds.cbData = size;
		cds.lpData = (void *) data;
		SendMessage(g_hMP, WM_COPYDATA, 0, (LPARAM) &cds);
	}
}





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

void SendData(const char *fmt, ...)
{
	int size;
	WSAOVERLAPPED ol;
	WSABUF wsabuf[2];
	char data[2048];

	VIRTUALIZER_SHARK_WHITE_START
	{
		size = wvsprintf(data, fmt, (va_list)((DWORD_PTR)&fmt + sizeof(void *)));
		RFCrypt(size, data, data);

		wsabuf[0].len = 4;
		wsabuf[0].buf = (char *)&size;
		wsabuf[1].len = size;
		wsabuf[1].buf = data;

		ol.hEvent = NULL;
		WSASend(g_clntSock, wsabuf, 2, NULL, 0, &ol, NULL);

		MessagePump();

		while (WSAGetOverlappedResult(g_clntSock, &ol, (DWORD *)&size, FALSE, (DWORD *)&size) == FALSE)
			MessagePump();
	}
	VIRTUALIZER_SHARK_WHITE_END
}




void MessagePump(void)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_CLOSE)
			exit(1);
	}
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


void ProcessMessage(char *packet)
{
	unsigned char aeskey[32];
	unsigned char buf[65];

	VIRTUALIZER_SHARK_WHITE_START
	{

		if (*packet == 'O')
		{
			sscanf(packet, "%*c %s %s", buf, time_remaning);
			if (hex2data(aeskey, buf, 32) == -1)
			{
				MessageBox(GetDesktopWindow(), "서버에서 받아온 AESkey에 오류가 있습니다!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				KillProcess();
			}
			aes256_init(&ctx, aeskey);
		}
		else if (*packet == 'X')
		{
			MessageBox(GetDesktopWindow(), "하드인증 실패!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == 'N')
		{
			MessageBox(GetDesktopWindow(), "기간 만료!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == 'E')
		{
			MessageBox(GetDesktopWindow(), "최신 버전이 아닙니다.\n파일을 새로 받아주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == '?')
		{
			MessageBox(GetDesktopWindow(), "?", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
	}
	VIRTUALIZER_SHARK_WHITE_END
}

bool ReceiveMessage(SOCKET s)
{
	static bool processing;
	int i, j;
	char data[2048];
	bool ret = false;

	VIRTUALIZER_SHARK_WHITE_START
	{
		if ((i = recv(s, &packet_data[packet_size], sizeof(packet_data) - packet_size, 0)) <= 0)
			goto $exit;

		packet_size += i;

		if (processing)
			goto $exit;

		processing = true;

		for (i = 0; packet_size >= i + 4 && packet_size >= i + (j = *(int *)&packet_data[i]) + 4; i += j + 4)
		{
			if (j > sizeof(data) - 1)
			{
				RFCrypt(sizeof(data) - 1, &packet_data[i + 4], data);
				data[sizeof(data) - 1] = '\0';
			}
			else
			{
				RFCrypt(j, &packet_data[i + 4], data);
				data[j] = '\0';
			}
			ProcessMessage(data);
		}

		if (packet_size -= i)
			memcpy(packet_data, &packet_data[i], packet_size);

		processing = false;
		ret = true;
$exit:;
	}
	VIRTUALIZER_SHARK_WHITE_END
	return ret;
}

void auth()
{
	char data[2048];

	getuuid(data);

	SendData("A 상점유틸 %s %s %s", data, username, version);
	
}

__declspec(naked) void nop2()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
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



bool ConnectToServer(const char *host, int port)
{
	SOCKET hSock;
	u_long ipaddr;
	struct hostent *h;
	SOCKADDR_IN servAdr; 
	FD_SET fds; 
	struct timeval tv;
	bool ret = false; 


	VIRTUALIZER_SHARK_WHITE_START
	{
		if ((ipaddr = inet_addr(host)) == INADDR_NONE)
			if ((h = gethostbyname(host)) != NULL)
				ipaddr = *(u_long *)h->h_addr;

		hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		ioctlsocket(hSock, FIONBIO, (u_long*)"\x01\x00\x00");
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = ipaddr;
		servAdr.sin_port = htons((u_short)port);

		FD_ZERO(&fds);
		FD_SET(hSock, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = 300000;

		connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr));

		ioctlsocket(hSock, FIONBIO, (u_long*)"\x00\x00\x00");
		select(0, 0, &fds, 0, &tv);
		if (!FD_ISSET(hSock, &fds))
		{
			closesocket(hSock);
			ret = false;
			goto $exit;
		}

		if (WSAAsyncSelect(hSock, g_hWnd, WM_SOCKET, FD_READ | FD_CLOSE) == SOCKET_ERROR)
		{
			closesocket(hSock);
			ret = false;
			goto $exit;
		}

		g_clntSock = hSock;
		ret = true;
$exit:;
	}
	VIRTUALIZER_SHARK_WHITE_END
	return ret;
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



void KillProcess()
{
	HANDLE hSnapshot, hThread;
	THREADENTRY32 te32;
	DWORD current_processid, current_threadid, oldprotect;

	VIRTUALIZER_SHARK_WHITE_START
	{
		if (g_clntSock != INVALID_SOCKET)
		{
			closesocket(g_clntSock);
			g_clntSock = INVALID_SOCKET;
		}
		VirtualProtect(GetModuleHandle(NULL), 10000000, PAGE_EXECUTE_READWRITE, &oldprotect);
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


__declspec(naked) void nop3()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


VOID CALLBACK FillNOP(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	unsigned char code[NOP_SIZE];
	DWORD oldprotect;
	unsigned int i, j;
	void(*pt_nop[20])() = { 0, };
	pt_nop[0] = nop1;
	pt_nop[1] = nop2;
	pt_nop[2] = nop3;
	pt_nop[3] = nop4;
	pt_nop[4] = nop5;
	pt_nop[5] = nop6;
	pt_nop[6] = nop7;
	pt_nop[7] = nop8;
	pt_nop[8] = nop9;
	pt_nop[9] = nop10;
	pt_nop[10] = nop11;
	pt_nop[11] = nop12;
	pt_nop[12] = nop13;
	pt_nop[13] = nop14;
	pt_nop[14] = nop15;

	VIRTUALIZER_TIGER_BLACK_START
	{
		for (i = 0; i < _countof(pt_nop); ++i)
		{
			if (pt_nop[i])
			{
				for (j = 0; j < sizeof(code) >> 2; j += 4)
					*(unsigned int*)&code[j] = gen_rand();

				VirtualProtect(pt_nop[i], NOP_SIZE, PAGE_EXECUTE_READWRITE, &oldprotect);
				memcpy(pt_nop[i], code, NOP_SIZE);
				VirtualProtect(pt_nop[i], NOP_SIZE, oldprotect, &oldprotect);
			}
		}
	}
	VIRTUALIZER_TIGER_BLACK_END
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	WSADATA wsaData;
	DWORD username_size = sizeof(username) - 1;

	
	VIRTUALIZER_SHARK_WHITE_START
	{
		SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);
 
		if (IsUserAnAdmin() == FALSE)
		{ 
			MessageBox(GetDesktopWindow(), "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR);
			KillProcess();
		} 

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			MessageBox(GetDesktopWindow(), "WSAStartup() error", 0, 0);
			KillProcess();
		}
	
		 
		if (!FileNameCheck("리듬 상점유틸.exe"))
			KillProcess();

		if (FindWindow("#32770", "리듬 상점유틸"))
		{
			SetForegroundWindow(FindWindow("#32770", "리듬 상점유틸"));
			KillProcess();
		}
		
		init_rand(GetCurrentProcessId() ^ 0xBADF00Du);

		FillNOP(0, 0, 0, 0);

		GetUserName(username, &username_size);


		CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
		MessageLoop();

		 
	}
	VIRTUALIZER_SHARK_WHITE_END 

	return 0;
}



__declspec(naked) void nop4()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

	

void packet_handle(unsigned char *data, unsigned int size)
{
	LVITEM li;
	unsigned int channel, room;
	char buf[1024], guild[13];
	unsigned char packet[1024];
	char textData[256], *p;
	unsigned int textSize, packet_size;
	SYSTEMTIME st;
	int i, j, k, l, idx;
	static DWORD tick;
	WINDOWINFO wi;
	LVFINDINFO fi;
	
	 
	if (*(unsigned short*)data == R_SERVERINFO)
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
			wsprintf(buf, "현재 채널: %u세", i);
		else
			wsprintf(buf, "현재 채널: %u채널", i);
		ListBox_Modify(g_hList4, 0, buf);
		memset(&character_list, 0, sizeof(character_list)); //전부 초기화
		memset(&employee_list, 0, sizeof(employee_list));
		SendMessage(g_hList1, LB_RESETCONTENT, 0, 0);
		ListView_DeleteAllItems(g_hList6);
		tick = GetTickCount();
	}
	else if (*(unsigned short*)data == R_EMPLOYEE_LEAVE - 1) //고상 정보 패킷
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
					wsprintf(buf, "%s %02u시%02u분%02u초 %s님이 고상 열음", st.wHour >= 12 ? "오후" : "오전", st.wHour > 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, employee_list[i].nick);
					SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM) buf), 0);
				}
				break;
			}
		}		
	}
	else if (*(unsigned short*) data == R_EMPLOYEE_LEAVE)
	{
		for (i = 0; i < _countof(employee_list); ++i)
		{
			if (*(unsigned int*) &data[2] == employee_list[i].id)
			{
				GetLocalTime(&st);
				SendMessage(g_hList1, LB_DELETESTRING, SendMessage(g_hList1, LB_FINDSTRINGEXACT, -1, (LPARAM) employee_list[i].nick), 0);
				wsprintf(buf, "%s %02u시%02u분%02u초 %s님이 고상 닫음", st.wHour >= 12 ? "오후" : "오전", st.wHour > 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, employee_list[i].nick);
				SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM) SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM) buf), 0);
				memset(&employee_list[i], 0, sizeof(employee_list[i]));
				break;
			}
		}
	}
	else if (*(unsigned short*)data == R_CHARACTER_APPEAR)
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
				PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_RESOURCE | SND_ASYNC);
				wsprintf(buf, "%s %02u시%02u분%02u초 %s님이 맵에 나타남", st.wHour >= 12 ? "오후" : "오전", st.wHour > 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, notify_list[i].nick);
				SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM)buf), 0);
				SendMessage(g_hList3, LB_SETITEMDATA, i, notify_list[i].shopid);
				break;
			}
		}
	}
	else if (*(unsigned short*)data == R_CHARACTER_APPEAR + 1) //캐릭터 사라질 때
	{
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*)&data[2])
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
	else if (*(unsigned short*)data == R_SHOPINFO)
	{
		if (data[2] != 0x14)
			return;
		if (*(unsigned short*)&data[4] == 0x12);
		else if (data[3] == 5 || data[3] == 6)
		{
			if (operating && (*(unsigned short*)&data[5] == 0 || *(unsigned short*)&data[5] == 1))
			{
				operating = 0;
				GetLocalTime(&st);
				//PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_RESOURCE | SND_ASYNC);
				wsprintf(buf, "%s %02u시%02u분%02u초 상점 입장 성공! 광클 종료", st.wHour >= 12 ? "오후" : "오전", st.wHour > 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond);
				SendMessage(g_hList3, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM)buf), 0);
			}
		}
	}
	else if (*(unsigned short*) data == R_TYPEOFLOUDSPEAKERS || *(unsigned short*) data == R_MEGAPHONE)
	{
		if(!IsDlgButtonChecked(g_hWnd, IDC_CHECK1))
			return;
		if (*(unsigned short*) data == R_TYPEOFLOUDSPEAKERS)
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
		p = CharUpper(buf);
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
			p = &p[-1];
			p[2] = '\0';
			channel = atoi(p);
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
		if (channel == 1)
			channel = 0;
		room += 910000000;
		packet_size = 0;
		*(unsigned short*) &packet[packet_size] = S_CHANGEROOM, packet_size += 2;
		packet[packet_size] = channel, ++packet_size;
		*(unsigned int*) &packet[packet_size] = room, packet_size += 4;
		*(unsigned int*) &packet[packet_size] = GetTickCount(), packet_size += 4;
		SendPacket(packet, packet_size);
		PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_RESOURCE | SND_ASYNC);

		wi.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(g_hMP, &wi); 
		if (wi.dwStyle & WS_MINIMIZE)
			PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
		SetForegroundWindow(g_hMP);
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
			s->auth = 0u;
			s->size = 0u;
		}
		else if ((s = session[th->dport]) == NULL)
			return;
		if (i = ih->tot_len, (i = (((i & 0xFF) << 8) | (i >> 8)) - (j = (ih->ihl << 2) + (th->doff << 2))) != 0)
			packet_handler_sub(s, (char *)pkt_data + j + 14u, i);
	
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


__declspec(naked) void nop5()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


INT_PTR CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	unsigned int count, i;
	char name[128], desc[128], buf[128];

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
			wsprintf(buf, "%u", adapter_sel);
			WritePrivateProfileString("SETTING", "adapter_sel", buf, INIPath);
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

	GetModuleFileName(NULL, INIPath, sizeof(INIPath));

	memcpy(&INIPath[strlen(INIPath) - 3], "ini", 3);

	if (!(R_SERVERINFO = GetPrivateProfileInt("PACKET", "R_SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_SERVERINFO", "", INIPath);
	if (!(R_EMPLOYEE_LEAVE = GetPrivateProfileInt("PACKET", "R_EMPLOYEE_LEAVE", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_EMPLOYEE_LEAVE", "", INIPath);
	if (!(R_CHARACTER_APPEAR = GetPrivateProfileInt("PACKET", "R_CHARACTER_APPEAR", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_CHARACTER_APPEAR", "", INIPath);
	if (!(R_SHOPINFO = GetPrivateProfileInt("PACKET", "R_SHOPINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_SHOPINFO", "", INIPath);
	if (!(adapter_sel = GetPrivateProfileInt("SETTING", "adapter_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "adapter_sel", "0", INIPath);
	if (!(R_TYPEOFLOUDSPEAKERS = GetPrivateProfileInt("PACKET", "R_TYPEOFLOUDSPEAKERS", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_TYPEOFLOUDSPEAKERS", "", INIPath);
	if (!(R_MEGAPHONE = GetPrivateProfileInt("PACKET", "R_MEGAPHONE", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_MEGAPHONE", "", INIPath);
	if (!(S_CHANGEROOM = GetPrivateProfileInt("PACKET", "S_CHANGEROOM", 0, INIPath)))
		WritePrivateProfileString("PACKET", "S_CHANGEROOM", "", INIPath);
	if (!(S_CHATTING = GetPrivateProfileInt("PACKET", "S_CHATTING", 0, INIPath)))
		WritePrivateProfileString("PACKET", "S_CHATTING", "", INIPath);
	if (!(S_WHISPER = GetPrivateProfileInt("PACKET", "S_WHISPER", 0, INIPath)))
		WritePrivateProfileString("PACKET", "S_WHISPER", "", INIPath);
}





INT_PTR CALLBACK OnInitDialog(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	LVCOLUMN col;
	char name[128], desc[128];

	g_hWnd = hDlg;
	VIRTUALIZER_SHARK_WHITE_START
	{ 
		if (!ConnectToServer(EXTERNAL_IP, 1818))
		{
			if (!ConnectToServer(INTERNAL_IP, 1818))
			{
				MessageBox(GetDesktopWindow(), "서버 닫힘!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				//KillProcess();
			}
		}
		//auth();
	}
	VIRTUALIZER_SHARK_WHITE_END

	g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
	g_hList2 = GetDlgItem(hDlg, IDC_LIST2);
	g_hList3 = GetDlgItem(hDlg, IDC_LIST3);
	g_hList4 = GetDlgItem(hDlg, IDC_LIST4);
	g_hList5 = GetDlgItem(hDlg, IDC_LIST5);
	g_hList6 = GetDlgItem(hDlg, IDC_LIST6);
	g_hButton1 = GetDlgItem(hDlg, IDC_BUTTON1);
	g_hButton2 = GetDlgItem(hDlg, IDC_BUTTON2);
	g_hButton5 = GetDlgItem(hDlg, IDC_BUTTON5);
	g_hMP = FindWindow("MapleStoryClass", NULL);
	EnableWindow(g_hButton1, FALSE);
	EnableWindow(g_hButton2, FALSE);
	EnableWindow(g_hButton5, FALSE);
	SendMessage(g_hList4, LB_INSERTSTRING, 0, (LPARAM)"현재 채널: NULL");
	SendMessage(g_hList5, LB_INSERTSTRING, 0, (LPARAM)"ID: 0");
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

	if (RegisterHotKey(g_hWnd, 8, MOD_NOREPEAT, VK_F8) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
		MessageBox(GetDesktopWindow(), "핫키 등록 실패!: F8", "알림", MB_ICONERROR);

	ReadINI();
	NpfFindAllDevices();
	NpfGetDeviceInfo(adapter_sel + 1, name, desc);
	NpfSetDevice(name);
	if (NpfStart())
		CloseHandle(CreateThread(0, 0, NpfLoop, 0, 0, 0));
	else
	{
		MessageBox(GetDesktopWindow(), "NpfStart() 실패", "알림", MB_ICONERROR);
		ExitProcess(1);
	}
	SetTimer(hDlg, 1, 5 * 60 * 1000, FillNOP);
	return 0;
}


__declspec(naked) void nop6()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}



int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	char buf[32], buf2[32];
	wchar_t wbuf[32], wbuf2[32];
	
	
	ListView_GetItemText(g_hList6, lParam1, ((sortinfo_t*)lParamSort)->iSubItem, buf, sizeof(buf));
	ListView_GetItemText(g_hList6, lParam2, ((sortinfo_t*)lParamSort)->iSubItem, buf2, sizeof(buf2));

	wsprintfW(wbuf, L"%S", buf);
	wsprintfW(wbuf2, L"%S", buf2);
	
	if (((sortinfo_t*) lParamSort)->iSubItem == 1)
	{
		if (StrCmpLogicalW(wbuf, wbuf2) != 0 && wcscmp(wbuf, L"") == 0)
			return 1;
		else if (StrCmpLogicalW(wbuf, wbuf2) != 0 && wcscmp(wbuf2, L"") == 0)
			return -1;
	}
	return ((sortinfo_t*)lParamSort)->ascending ? StrCmpLogicalW(wbuf, wbuf2) : -StrCmpLogicalW(wbuf, wbuf2);

}

void Clipboard(char *source)
{
	int ok = OpenClipboard(NULL);
	if (!ok)
		return;

	HGLOBAL clipbuffer;
	char * buffer;


	EmptyClipboard();
	int srclen = strlen(source) + 1;
	clipbuffer = GlobalAlloc(GMEM_MOVEABLE, srclen);
	buffer = (char*) GlobalLock(clipbuffer);
	strcpy_s(buffer, srclen, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

}

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (iMessage)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN)
			SendMessage(GetParent(hWnd), WM_COMMAND, IDC_BUTTON1, 0);
		break;
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}


INT_PTR CALLBACK ChattingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	BYTE packet[1024];
	char buf[256], target_nick[32];
	unsigned int size;

	switch (iMessage)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, 70, 0);
		SendDlgItemMessage(hDlg, IDC_EDIT2, EM_LIMITTEXT, 12, 0);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT2), FALSE);
		CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), EditSubclassProc, 0, 0);
		break; 
	case WM_CLOSE:
		RemoveWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), EditSubclassProc, 0);
		g_hChattingDlg = 0;
		DestroyWindow(hDlg);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			GetDlgItemText(hDlg, IDC_EDIT1, buf, sizeof(buf));
			size = 0;
			if(IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
			{
				*(unsigned short*)&packet[size] = S_CHATTING, size += 2;
				*(unsigned int*)&packet[size] = GetTickCount(), size += 4;
				*(unsigned short*)&packet[size] = (unsigned short)strlen(buf), size += 2;
				memcpy(&packet[size], buf, strlen(buf)), size += strlen(buf);
				packet[size] = 0, ++size;
			}
			else
			{
				if (GetDlgItemText(hDlg, IDC_EDIT2, target_nick, sizeof(target_nick)) == 0)
				{
					MessageBox(GetDesktopWindow(), "상대를 입력해주세요", "알림", MB_ICONINFORMATION);
					return 0;
				}
				*(unsigned short*)&packet[size] = S_WHISPER, size += 2;
				packet[size] = 0x6, ++size;
				*(unsigned int*)&packet[size] = GetTickCount(), size += 4;
				*(unsigned short*)&packet[size] = (unsigned short)strlen(target_nick), size += 2;
				memcpy(&packet[size], target_nick, (unsigned short)strlen(target_nick)), size += strlen(target_nick);
				*(unsigned short*)&packet[size] = (unsigned short)strlen(buf), size += 2;
				memcpy(&packet[size], buf, strlen(buf)), size += strlen(buf);

			}
			SendPacket(packet, size);
			if(IsDlgButtonChecked(hDlg, IDC_CHECK1) == BST_CHECKED)
				SetDlgItemText(hDlg, IDC_EDIT1, "");
			break;
		case IDC_RADIO1:
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT2), FALSE);
			break;
		case IDC_RADIO2:
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT2), TRUE);
			break;
		}
		break;
	}

	return 0;
}

DWORD WINAPI ClickFunc(LPVOID arg)
{
	while (operating)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(30);
		PostMessage(g_hMP, WM_CHAR, '/', 0);
		Sleep(30);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(30);
	}
	return 0;
}


__declspec(naked) void nop7()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


DWORD WINAPI ReconnectThread(LPVOID arg)
{
	unsigned int i;

	VIRTUALIZER_TIGER_BLACK_START
	{
		while (1)
		{
			if (ConnectToServer(EXTERNAL_IP, 1818) || ConnectToServer(INTERNAL_IP, 1818))
				break;
			for (i = 0; i < 5; ++i)
				Sleep(1000);
		}
		auth();
	}
	VIRTUALIZER_TIGER_BLACK_END

	return 0;
}
 
LRESULT ProcessSocketMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SOCKET s = (SOCKET)wParam;
	int socket_event;

	VIRTUALIZER_SHARK_WHITE_START
	{
		socket_event = WSAGETSELECTEVENT(lParam);

		if (socket_event == FD_READ)
			ReceiveMessage(s);
		else if (socket_event == FD_CLOSE)
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ReconnectThread, 0, 0, 0));
	}
	VIRTUALIZER_SHARK_WHITE_END
	return 0;
}


INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int i, j, k;
	char buf[128];
	static sortinfo_t si; 
	char year[8] = { 0, }, month[8] = { 0, }, day[8] = { 0, };

	switch (iMessage) 
	{
	case WM_SOCKET:
		return ProcessSocketMessage(hDlg, wParam, lParam);
	case WM_HOTKEY:
		if (wParam == 8)
		{
			operating = !operating;
			if (operating)
				CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ClickFunc, 0, 0, 0));
		}
		break;
	case WM_ACTIVATEAPP:
		if (wParam == WA_ACTIVE)
			SetForegroundWindow(g_hChattingDlg);
		break;
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
		return OnInitDialog(hDlg, iMessage, wParam, lParam);
	case WM_CLOSE:
		breakloop = 1;
		while (breakloop)
			SwitchToThread();
		KillTimer(hDlg, 1);
		if (g_clntSock != INVALID_SOCKET)
		{
			closesocket(g_clntSock);
			g_clntSock = INVALID_SOCKET;
		}
		WSACleanup();
		UnregisterHotKey(g_hWnd, 8);
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
					for (j = 0; j < SendMessage(g_hList1, LB_GETCOUNT, 0, 0); ++j)
					{
						if (strcmp(buf, employee_list[j].nick) == 0)
						{
							if (notify_list[39].id != 0)
							{
								MessageBox(GetDesktopWindow(), "알림 울릴 캐릭터는 40개까지 지정할 수 있습니다.", "알림", 0);
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
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		case IDC_BUTTON5:
			wsprintf(buf, "0x%X", employee_list[SendMessage(g_hList1, LB_GETCURSEL, 0, 0)].id);
			Clipboard(buf);
			wsprintf(buf, "ID: 0x%X", employee_list[SendMessage(g_hList1, LB_GETCURSEL, 0, 0)].id);
			ListBox_Modify(g_hList5, 0, buf);
			break;
		case IDC_BUTTON6:
			if (!g_hChattingDlg)
				g_hChattingDlg = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG3), g_hWnd, ChattingDlgProc);
			else
				SetForegroundWindow(g_hChattingDlg);
			break;
		case IDC_BUTTON7:
			sscanf(time_remaning, "%4s%2s%2s", year, month, day);
			wsprintf(buf, "%s년 %s월 %s일까지 사용 가능합니다!", year, month, day);
			MessageBox(0, buf, "알림", MB_ICONINFORMATION);
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
			//CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			//MessageBox(GetDesktopWindow(), "현재로서는 사용할 수 없습니다", "알림", MB_ICONINFORMATION);
			break;
		}
		break;

	}
	return 0;
}


__declspec(naked) void nop8()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}
