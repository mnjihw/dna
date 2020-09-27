#include "common.h"

const char version[] = "1.84";

#define INTERNAL_IP "192.168.219.101"
#define EXTERNAL_IP "182.211.75.197"

SOCKET g_clntSock = INVALID_SOCKET;
char packet_data[4096];
int packet_size;

unsigned int send_iv, recv_iv;
unsigned int channel;
char nick[13];
char serverName[16];

pos_t save_pos;
server_t server[11];
warp_t warp[10];
int warp_prepare_sel;

unsigned int cswari_mode;
extern aes256_context ctx;
HANDLE hPcapThread;

unsigned int breakloop, userid, target_userid, wari_shop_idx, decrypt_sel, miniroom_count;
HWND PacketWnd, g_hWnd, g_hList1, g_hList2, g_hList3, g_hSettingWnd, g_hButton4, g_hSnifferWnd, g_hWarpWnd, g_hFindWnd, g_hWarpList[10];
HWND MainWnd;
WNDPROC MainWndProc;
unsigned int operating, 해상도, target_sel, adapter_sel, only_response, minimize_sel, shop_sel, response_sel, wari_state, startup, warp_sel, miniroom_sel, open_slowly, sniffing;
unsigned int R_USER_MINIROOM_BALLON, R_EMPLOYEE_LEAVE, R_SERVERINFO, R_USERINFO, R_SHOPINFO, R_CHARACTER_APPEAR, R_PING;
unsigned int S_MINIROOM, S_CHARACTER_MOVE;
char INIPath[MAX_PATH], title[51], time_remaning[16];
unsigned int pt_CUniqueModeless, pt_sendpacket, pt_CUserLocal, pt_getpos, pt_chatlog, pt_CUIStatusBar;

void *client_base, *module_base;
packet_t packet[8];
static void *engine;
static unsigned int lan_flag;
employeeinfo_t employee_list[40];
charinfo_t character_list[64];
unsigned char pktbuf[65536];
unsigned int pktbuf_size, listidx;
char username[128];

#define SendPacket(wParam, lParam) SendMessage(MainWnd, WM_USER + 5, wParam, lParam);



void server_init(server_t *s)
{
	strcpy(s[0].name, "스카니아");
	strcpy(s[1].name, "베라");
	strcpy(s[2].name, "루나");
	strcpy(s[3].name, "제니스");
	strcpy(s[4].name, "크로아");
	strcpy(s[5].name, "유니온");
	strcpy(s[6].name, "엘리시움");
	strcpy(s[7].name, "이노시스");
	strcpy(s[8].name, "레드");
	strcpy(s[9].name, "오로라");
	strcpy(s[10].name, "리부트");

	s[0].value = 0;
	s[1].value = 0x1;
	s[2].value = 0x3;
	s[3].value = 0x4;
	s[4].value = 0x5;
	s[5].value = 0xA;
	s[6].value = 0x10;
	s[7].value = 0x1D;
	s[8].value = 0x2B;
	s[9].value = 0x2C;
	s[10].value = 0x2D;
}


unsigned int isserver(unsigned char value)
{
	unsigned int i;

	for (i = 0; i < _countof(server); ++i)
	{
		if (server[i].value == value)
			return 1;
	}
	return 0;
}

unsigned int isnickname(char *str, unsigned int count)
{
	unsigned int i;

	for (i = 0; i < count; ++i)
	{
		if ((str[i] & 0x80) != 0x80 && !isalnum(str[i]))
			return 0;
	}
	return 1;

}

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

	wvsprintf(data, fmt, (va_list)((unsigned int)&fmt + sizeof(void *)));
	((void(__cdecl *)(const char **, int))pt_chatlog)((p = data, &p), type);

}

void ListBox_Modify(HWND hwnd, int sel, char *str)
{
	SendMessage(hwnd, LB_DELETESTRING, sel, 0);
	SendMessage(hwnd, LB_INSERTSTRING, sel, (LPARAM)str);
}


__declspec(naked) void nop1()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
		VIRTUALIZER_SHARK_WHITE_END
}

void KillProcess()
{
	HANDLE hSnapshot, hThread;
	THREADENTRY32 te32;
	DWORD current_processid, current_threadid, oldprotect;
	unsigned int i = 0;

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
							if (++i == 20)
								break;
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

void packet_disallow()
{
	FWPM_FILTER0 filter;

	if (lan_flag)
		return;
	VIRTUALIZER_TIGER_BLACK_START
	{
		lan_flag = 1;

		if (engine)
		{
			memset(&filter, 0, sizeof(filter));
			filter.displayData.name = L"<~@X0)5Dg$#?E<~>"; /* Base85 "by movntq" */
			memcpy(&filter.layerKey, "\xBF\xD1\x6F\xC8\xCD\x21\x7E\x49\xA0\xBB\x17\x42\x5C\x88\x5C\x58", 16); /* FWPM_LAYER_INBOUND_IPPACKET_V4 */
			filter.numFilterConditions = 0;
			filter.filterCondition = NULL;
			filter.action.type = 0x00001001; /* FWP_ACTION_BLOCK */
			memcpy(&filter.filterKey, "mbc-cafe-movntq1", 16);
			FwpmFilterAdd0(engine, &filter, NULL, NULL);
			memset(&filter, 0, sizeof(filter));
			filter.displayData.name = L"<~@X0)5Dg$#?E<~>"; /* Base85 "by movntq" */
			memcpy(&filter.layerKey, "\xAE\x9F\x5C\x1E\x84\x8A\x35\x41\xA3\x31\x95\x0B\x54\x22\x9E\xCD", 16); /* FWPM_LAYER_OUTBOUND_IPPACKET_V4 */
			filter.numFilterConditions = 0;
			filter.filterCondition = NULL;
			filter.action.type = 0x00001001; /* FWP_ACTION_BLOCK */
			memcpy(&filter.filterKey, "mbc-cafe-movntq2", 16);
			FwpmFilterAdd0(engine, &filter, NULL, NULL);
		}
	}
		VIRTUALIZER_TIGER_BLACK_END
}

void packet_allow()
{
	GUID guid;

	if (!lan_flag)
		return;
	VIRTUALIZER_TIGER_BLACK_START
	{
		lan_flag = 0;

		if (engine)
		{
			FwpmFilterDeleteByKey0(engine, (GUID *)memcpy(&guid, "mbc-cafe-movntq1", 16));
			FwpmFilterDeleteByKey0(engine, (GUID *)memcpy(&guid, "mbc-cafe-movntq2", 16));
		}
	}
		VIRTUALIZER_TIGER_BLACK_END
}

void packet_init()
{
	FWPM_SESSION0 session;

	VIRTUALIZER_TIGER_BLACK_START
	{
		memset(&session, 0, sizeof(session));
		session.flags = 1; /* FWPM_SESSION_FLAG_DYNAMIC */

		FwpmEngineOpen0(NULL, 0xFFFFFFFF, NULL, &session, &engine);
	}
		VIRTUALIZER_TIGER_BLACK_END
}

void packet_term()
{
	VIRTUALIZER_TIGER_BLACK_START
	{
		if (engine)
		{
			FwpmEngineClose0(engine);
			engine = NULL;
		}
	}
		VIRTUALIZER_TIGER_BLACK_END
}



void PrepareShopPacket()
{
	unsigned int i;

	// 오목팅 (100메소 이상 소지)
	// F500 10 02 010000 00 03

	/* enter */
	packet[0].size = 9;
	*(unsigned __int16 *)&packet[0].data[0] = (unsigned __int16)S_MINIROOM;
	*(unsigned __int8  *)&packet[0].data[2] = (unsigned __int8)0x13; //S_MINIROOM_ENTER
	*(unsigned __int32 *)&packet[0].data[3] = 0;
	*(unsigned __int16 *)&packet[0].data[7] = 0;

	/* open */
	// op코드(&data[2])만 참조 -> 68 de 00 00 00 8d 4c 24 ? e8 ? ? ? ? 6a ? 8d 4c 24 ? c7 44 24 ? 06 ? ? ? e8
	if (miniroom_sel == 5)
		packet[1].size = (i = strlen(title)) + 8u;
	else
		packet[1].size = (i = strlen(title)) + 13u;
	*(unsigned __int16 *)&packet[1].data[0] = (unsigned __int16)S_MINIROOM;
	*(unsigned __int8  *)&packet[1].data[2] = (unsigned __int8)0x10; //S_MINIROOM_CREATE
	if (miniroom_sel == 5)
		*(unsigned __int8  *)&packet[1].data[3] = 2;
	else
		*(unsigned __int8  *)&packet[1].data[3] = 5;
	*(unsigned __int16 *)&packet[1].data[4] = (unsigned __int16)i;
	memcpy(&packet[1].data[6], title, i);
	if (miniroom_sel == 5)
		*(unsigned __int16*)&packet[1].data[i + 6u] = 0;
	else
	{
		*(unsigned __int8  *)&packet[1].data[i + 6u] = 0; /* category */
		*(unsigned __int16 *)&packet[1].data[i + 7u] = 1; /* slot */
		*(unsigned __int32 *)&packet[1].data[i + 9u] = 5140000 + miniroom_sel; /* itemid */
	}


	/* unknown */
	packet[2].size = 5;
	*(unsigned __int16 *)&packet[2].data[0] = (unsigned __int16)S_MINIROOM;
	*(unsigned __int8  *)&packet[2].data[2] = (unsigned __int8)0x50; //S_MINIROOM_START1
	*(unsigned __int16 *)&packet[2].data[3] = 0;

	/* start */
	// 68 de 00 00 00 8d 4c 24 ? e8 ? ? ? ? 6a ? 8d 4c 24 ? 89
	packet[3].size = 4;
	*(unsigned __int16 *)&packet[3].data[0] = (unsigned __int16)S_MINIROOM;
	*(unsigned __int8  *)&packet[3].data[2] = (unsigned __int8)0x1A; //S_MINIROOM_START2
	*(unsigned __int8  *)&packet[3].data[3] = 1;


}

__declspec(naked) void nop2()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
		VIRTUALIZER_SHARK_WHITE_END
}



void client_loading_done(void)
{
}


void HookJMP(void *addr, void *fn, void *code)
{
	DWORD protect;

	VIRTUALIZER_TIGER_BLACK_START
	{
		if (VirtualProtect(addr, 5, PAGE_READWRITE, &protect))
		{
			if (fn)
			{
				if (code)
					memcpy(code, addr, 5);
				*(unsigned char *)addr = 0xE9; /* JMP */
				*(unsigned int  *)((unsigned int)addr + 1u) = (unsigned int)fn - (unsigned int)addr - 5u;
			}
			else if (code)
				memcpy(addr, code, 5);
			VirtualProtect(addr, 5, protect, &protect);
			FlushInstructionCache(GetCurrentProcess(), addr, 5);
		}
	}
		VIRTUALIZER_TIGER_BLACK_END
}


unsigned __int64 rdtsc(void)
{
	__asm rdtsc;
}


int DlgPacketProcSub(HWND hwnd, char *buf, unsigned char *dest)
{
	static unsigned int counter;
	unsigned int i, j;
	unsigned char *p, data[65536];


	*data = '\0';

	if (hwnd && buf == 0 && dest == 0)
	{
		VIRTUALIZER_TIGER_BLACK_START
		{
			GetDlgItemText(hwnd, IDC_EDIT1, data, sizeof(data));
		}
			VIRTUALIZER_TIGER_BLACK_END

			p = data;

		while (*p)
		{
			i = 0;
			do
			{
				switch (*p)
				{
				case '#': //무시
					while (*++p && *p != '\n');
					break;

				case '*': //랜덤
					++p;
					data[i++] = (unsigned char)rdtsc();
					break;

				case '.': //0 ~ FF까지 카운터
					++p;
					data[i++] = (unsigned char)(++counter);
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
						data[i++] = (unsigned char)((j << 4) | (*p++ - '0'));
					else if ('A' <= *p && *p <= 'F')
						data[i++] = (unsigned char)((j << 4) | (*p++ - 'A' + 10u));
					else if ('a' <= *p && *p <= 'f')
						data[i++] = (unsigned char)((j << 4) | (*p++ - 'a' + 10u));
					break;

				case '@':
					++p;
					*(unsigned int *)&data[i] = (unsigned int)GetTickCount();
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
						data[i++] = (unsigned char)((j << 4) | (*p++ - '0'));
					else if ('A' <= *p && *p <= 'F')
						data[i++] = (unsigned char)((j << 4) | (*p++ - 'A' + 10u));
					else if ('a' <= *p && *p <= 'f')
						data[i++] = (unsigned char)((j << 4) | (*p++ - 'a' + 10u));
					break;

				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
					j = (*p++ - 'a' + 10u);
					if ('0' <= *p && *p <= '9')
						data[i++] = (unsigned char)((j << 4) | (*p++ - '0'));
					else if ('A' <= *p && *p <= 'F')
						data[i++] = (unsigned char)((j << 4) | (*p++ - 'A' + 10u));
					else if ('a' <= *p && *p <= 'f')
						data[i++] = (unsigned char)((j << 4) | (*p++ - 'a' + 10u));
					break;

				default:
					++p;
					break;
				}
			} while (*p && *p != '\n');
			if (i > 1u)
			{
				memcpy(pktbuf, data, i);
				pktbuf_size = i;
				SendPacket(5, 0)
					//			VIRTUALIZER_TIGER_BLACK_START
				{
					//send_packet(data, i);
				}
				//		VIRTUALIZER_TIGER_BLACK_END
			}
		}
		/*memcpy(pktbuf, data, i);
		pktbuf_size = i;
		SendPacket(5, 0)
		return 0;*/ //이거 왜 이리해뒀냐 ㅋㅋ
	}
	else
	{
		p = buf;

		while (*p)
		{
			i = 0;
			do
			{
				switch (*p)
				{
				case '#':
					while (*++p && *p != '\n');
					break;

				case '*':
					++p;
					dest[i++] = (unsigned char)rdtsc();
					break;

				case '.':
					++p;
					dest[i++] = (unsigned char)(++counter);
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
						dest[i++] = (unsigned char)((j << 4) | (*p++ - '0'));
					else if ('A' <= *p && *p <= 'F')
						dest[i++] = (unsigned char)((j << 4) | (*p++ - 'A' + 10u));
					else if ('a' <= *p && *p <= 'f')
						dest[i++] = (unsigned char)((j << 4) | (*p++ - 'a' + 10u));
					break;

				case '@':
					++p;
					*(unsigned int *)&dest[i] = (unsigned int)GetTickCount();
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
						dest[i++] = (unsigned char)((j << 4) | (*p++ - '0'));
					else if ('A' <= *p && *p <= 'F')
						dest[i++] = (unsigned char)((j << 4) | (*p++ - 'A' + 10u));
					else if ('a' <= *p && *p <= 'f')
						dest[i++] = (unsigned char)((j << 4) | (*p++ - 'a' + 10u));
					break;

				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
					j = (*p++ - 'a' + 10u);
					if ('0' <= *p && *p <= '9')
						dest[i++] = (unsigned char)((j << 4) | (*p++ - '0'));
					else if ('A' <= *p && *p <= 'F')
						dest[i++] = (unsigned char)((j << 4) | (*p++ - 'A' + 10u));
					else if ('a' <= *p && *p <= 'f')
						dest[i++] = (unsigned char)((j << 4) | (*p++ - 'a' + 10u));
					break;

				default:
					++p;
					break;
				}
			} while (*p && *p != '\n');
			return i;
		}
	}
	return 0;
}



void MessagePump(void)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			PostQuitMessage(msg.wParam);
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
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

__declspec(naked) void nop3()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
		VIRTUALIZER_SHARK_WHITE_END
}


DWORD WINAPI func(LPVOID arg)
{
	unsigned int i, j;
	static unsigned int addr1, addr2, code[2];

	VIRTUALIZER_TIGER_BLACK_START
	{
		Sleep(1500);


		if ((i = (unsigned int)memscan(client_base, "\x33\xC0\x39\x05\x00\x00\x00\x00\x0F\x95\xC0\xC3", sizeof("\x33\xC0\x39\x05\x00\x00\x00\x00\x0F\x95\xC0\xC3") - 1)) != 0)
			pt_CUserLocal = *(unsigned int *)(i + 4u);


		if ((i = (unsigned int)memscan(client_base, "\x51\x83\x3D\x00\x00\x00\x00\x00\x74", sizeof("\x51\x83\x3D\x00\x00\x00\x00\x00\x74") - 1)) != 0)
		{
			pt_chatlog = i;
			pt_CUIStatusBar = *(unsigned int *)(i + 3u);
		}

		pt_getpos = 16; /* CUser::GetPos (IVecCtrlOwner) */

		pt_sendpacket = (unsigned int)memscan(client_base, "\x8B\x00\x24\x04\x00\x8B\x00\x8B\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\xFF\xC7", sizeof("\x8B\x00\x24\x04\x00\x8B\x00\x8B\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\xFF\xC7") - 1);
		//pt_sendpacket = (unsigned int)memscan(client_base, "\x8B\x44\x24\x08\x50\xE8\x00\x00\x00\xFF\xC7\x86\x00\x03\x00\x00\x01\x00\x00\x00", sizeof("\x8B\x44\x24\x08\x50\xE8\x00\x00\x00\xFF\xC7\x86\x00\x03\x00\x00\x01\x00\x00\x00") - 1);
		//if ((i = (unsigned int) memscan(client_base, "\x51\x83\x3D\x00\x00\x00\x00\x00\x74", sizeof("\x51\x83\x3D\x00\x00\x00\x00\x00\x74") - 1)) != 0) 
			//pt_chatlog = i;
		dbg("%p", pt_sendpacket);

		if ((i = (unsigned int)memscan(client_base, "\xA3\x00\x00\x00\x00\xEB\x0A\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xC7", sizeof("\xA3\xFC\x9C\xB7\x01\xEB\x0A\xC7\x05\xFC\x9C\xB7\x01\x00\x00\x00\x00\xC7") - 1)) != 0)
			pt_CUniqueModeless = *(unsigned int*)(i + 1);


		if ((i = (unsigned int)memscan(client_base, "\x55\x49\x2F\x4C\x6F\x67\x6F\x2E", 8)) != 0)
		{
			i = *(unsigned int *)(i - 8u);
			addr1 = i;
			if ((j = (unsigned int)_memscan((void *)i, 128, "\x74\x00\x8B\x00\xE8\x00\x00\xFF", 8)) != 0)
			{
				j = (j + 4u) + *(unsigned int *)(j + 5u) + 5u;
				addr2 = j;
				__asm
				{
					push offset code;
					push offset $SKIP_INTRO_BEGIN;
					push addr1;
					call HookJMP;
					add esp, 12;
				}
			}
		}
	}
		VIRTUALIZER_TIGER_BLACK_END
		__asm
	{
		jmp short $SKIP_INTRO_END;
	$SKIP_INTRO_BEGIN:
		VIRTUALIZER_TIGER_BLACK_START
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
			VIRTUALIZER_TIGER_BLACK_END
			ret 4;
	$SKIP_INTRO_END:
	}


	VIRTUALIZER_TIGER_BLACK_START
	{
		CreateDialog(module_base, MAKEINTRESOURCE(IDD_DIALOG2), 0, DlgProc);
		MessageLoop();
	}
		VIRTUALIZER_TIGER_BLACK_END
		return 0;
}

int __stdcall _connect(SOCKET s, const struct sockaddr *name, int namelen)
{

	VIRTUALIZER_TIGER_BLACK_START
	{
		if (namelen == sizeof(struct sockaddr_in))
		{
			if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, "\x01\x00\x00", 4) == 0)
				if ((connect(s, name, namelen) == 0 || WSAGetLastError() == WSAEWOULDBLOCK))
					WSASetLastError(WSAEWOULDBLOCK);
		}
		else
			WSASetLastError(WSAEWOULDBLOCK);

	}
		VIRTUALIZER_TIGER_BLACK_END

		return -1;
}


void send_packet(const void *data, unsigned int size)
{
	struct
	{
		unsigned int zero;
		const void *data;
		unsigned int size;
	} param;

	/*
	어떤 쓰래드에서 보냈나 체크를 하기 때문에 막 보내면 영정익스프레스임
	[send함수를 dump해서 쓰든지, CRC처리를 하고 직접 수정을 가하든지, 메인쓰래드에서 패킷을 보내든지]
	내가 패킷 보낼 때는 메인쓰래드를 통해서 보내니까 상관없는듯ㅋ
	*/

	param.zero = 0;
	param.data = data;
	param.size = size;

	((void(__cdecl *)(void *))pt_sendpacket)(&param);
}

__declspec(naked) void nop4()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
		VIRTUALIZER_SHARK_WHITE_END
}

void move_character(pos_t dest_pos, pos_t current_pos, unsigned int detail)
{
	unsigned int i;

	if ((i = *(unsigned int*)pt_CUserLocal) != 0)
	{
		if (dest_pos.v != current_pos.v)
		{
			if (dest_pos.v < current_pos.v)
				keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), 0, 0);
			else
				keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
			while (abs(dest_pos.v - current_pos.v) >= 20)
			{
				thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
				Sleep(10);
			}
			keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(210);

			if (detail)
			{
				thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
				if (dest_pos.v != current_pos.v)
				{
					while (1)
					{
						if ((detail == 1 && abs(dest_pos.v - current_pos.v) <= 3) || (detail == 2 && abs(dest_pos.v - current_pos.v) <= 1))
							break;

						if (dest_pos.v < current_pos.v)
						{
							keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), 0, 0);
							Sleep(30);
							keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						}
						else if (dest_pos.v > current_pos.v)
						{
							keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
							Sleep(30);
							keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						}
						Sleep(100);
						thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					}
				}
			}
			//			dbg("와리끝!");
		}
	}
}

DWORD WINAPI CSWariThread(LPVOID arg)
{
	unsigned int i;
	pos_t current_pos, temp;

	Sleep(500);
	//cswari_mode = 0; 이거갖고 와리 여러번 못하게 어케 해보기
	if (warp_sel == 0)
	{
		if ((i = *(unsigned int*)pt_CUserLocal) != 0)
		{
			if (save_pos.c == -146 || save_pos.c == -206 || save_pos.c == -416 || save_pos.c == 34 || save_pos.c == -26 || save_pos.c == -86)
			{
				while (1)
				{
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					Sleep(300);
					if (current_pos.c == -146 || current_pos.c == -206 || current_pos.c == -416 || current_pos.c == 34)
						break;
				}
				if (save_pos.c == 34) //1층의 높이
				{
					if (current_pos.c != 34)
					{
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);

						while (current_pos.c != 34)
						{
							keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
							keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							Sleep(1000);
							thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
						}
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					}
					move_character(save_pos, current_pos, 2);
				}
				else  if (save_pos.c == -206)//2층의 높이
				{
					if (current_pos.c == -146) //언덕젠됐으면 1층으로 감
					{
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);

						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(200);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						current_pos.c = 34;
					}
					switch (current_pos.c)
					{
					case 34: //1층
						temp.v = -271;
						temp.c = 34;
						move_character(temp, current_pos, 1);
						Sleep(100);
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						break;
					case -416: //3층
						if (current_pos.v > 417) //왼쪽으로 가야함
						{
							temp.v = 417;
							temp.c = -416;
							move_character(temp, current_pos, 0);
							Sleep(100);
						}
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(51);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);

						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						break;
					}
					//여긴 2층이야
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					move_character(save_pos, current_pos, 2);

				}
				else if (save_pos.c == -416) //3층의 높이
				{
					if (current_pos.c == -146)
					{
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);

						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(200);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						current_pos.c = 34;
					}
					switch (current_pos.c)
					{
					case 34: //1층
						temp.v = -271;
						temp.c = 34;
						move_character(temp, current_pos, 1);
						Sleep(100);
						while (current_pos.c != -416)
						{
							keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
							Sleep(50);
							keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							Sleep(50);
							thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
							Sleep(100);
						}
						Sleep(50);
						break;
					case -206: //2층
						temp.v = -248;
						temp.c = -206;
						move_character(temp, current_pos, 1);
						Sleep(100);
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						break;
					}
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					move_character(save_pos, current_pos, 2);
				}
				else if (save_pos.c == -26) //첫번째 계단의 높이
				{
					temp.v = 559;
					temp.c = 34;
					move_character(temp, current_pos, 0);
					Sleep(100);
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					if (current_pos.c < -26)
					{
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(1500);
					}
					else if (current_pos.c > -26)
					{
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(200);
					}
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					move_character(save_pos, current_pos, 2);

				}
				else if (save_pos.c == -86) //두번째 계단의 높이
				{
					temp.v = 559;
					temp.c = 34;
					move_character(temp, current_pos, 0);
					Sleep(100);
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					if (current_pos.c < -26)
					{
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(1500);
					}
					else if (current_pos.c > -26)
					{
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(200);
					}
					keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
					Sleep(500);
					keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
					Sleep(100);
					keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					Sleep(300);
					keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					Sleep(100);
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					move_character(save_pos, current_pos, 2);
				}
				else if (save_pos.c == -146) //언덕의 높이
				{
					switch (current_pos.c)
					{
					case 34:
						temp.v = 559;
						temp.c = 34;
						move_character(temp, current_pos, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(200);
						keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(500);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(1000);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(300);
						keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(100);
						break;
					case -206:
						temp.v = 559;
						temp.c = -26;
						move_character(temp, current_pos, 0);
						Sleep(100);
						keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(500);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(1000);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(100);
						keybd_event(VK_MENU, MapVirtualKey(VK_MENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(300);
						keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(100);
						break;
					case -416:
						keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
						Sleep(50);
						while (current_pos.c != -146)
						{
							thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
							Sleep(100);
						}
						keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(100);
						break;
					}
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&current_pos);
					move_character(save_pos, current_pos, 2);
				}
			}// 이 이외 높이에선 와리 안함

		}
	}
	Sleep(50);
	keybd_event('i', 0x17, 0, 0);
	keybd_event('i', 0x17, KEYEVENTF_KEYUP, 0);
	Sleep(150);

	if (shop_sel) //자동일고랜
	{
		switch (해상도)
		{
		case 800:
			MouseClick(776, 38, 1);
			break;
		case 1024:
			MouseClick(999, 37, 1);
			break;
		case 1366:
		case 910:
			MouseClick(1340, 41, 1);
			break;
		}
		Sleep(150);
		switch (해상도)
		{
		case 800:
			MouseClick(693, 63, 2);
			break;
		case 1024:
			MouseClick(917, 67, 2);
			break;
		case 1366:
		case 910:
			MouseClick(1259, 62, 2);
			break;
		}
		Sleep(200);

	}

	if (minimize_sel)
	{
		PostMessage(MainWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		Sleep(200);
	}
	cswari_mode = 0;
	operating = 1;
	return 0;
}




VOID CALLBACK DoCSWariSub(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	static DWORD dwLastTime;
	unsigned int i;

	if (cswari_mode == 0xFFFFFFFFu)
	{
		KillTimer(hwnd, idTimer);
		CloseHandle(CreateThread(0, 0, CSWariThread, 0, 0, 0));
	}
	else
		switch (cswari_mode)
		{
		case 1:
			operating = 0;
			if ((i = *(unsigned int *)pt_CUIStatusBar) != 0)
			{

				if (shop_sel)
				{
					switch (해상도)
					{
					case 800:
						MouseClick(488, 362, 1);
						break;
					case 1024:
						MouseClick(602, 445, 1);
						break;
					case 1366:
					case 910:
						MouseClick(773, 450, 1);
						break;
					}
					Sleep(150);
				}
				cswari_mode = 2;
				dwLastTime = dwTime;
				thiscall1(*(void **)(*(unsigned int *)i + 32u), (void *)i, (void *)1000);
				if ((i = *(unsigned int *)pt_CUserLocal) != 0)
					thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&save_pos);
			}
			else
			{
				cswari_mode = 3;
				DoCSWariSub(hwnd, message, idTimer, dwTime);
			}

			break;

		case 2:
			if ((i = *(unsigned int *)pt_CUIStatusBar) != 0)
			{
				if ((int)dwTime - (int)dwLastTime >= 500)
				{
					cswari_mode = 1;
					DoCSWariSub(hwnd, message, idTimer, dwTime);
				}
			}
			else
			{
				cswari_mode = 3;
				DoCSWariSub(hwnd, message, idTimer, dwTime);
			}

			break;

		case 3:
			if ((i = *(unsigned int *)pt_CUserLocal) != 0)
			{
				cswari_mode = 0xFFFFFFFFu;
				DoCSWariSub(hwnd, message, idTimer, dwTime);
			}
			else
				MouseClick(5, 728, 1);
			break;
		}
}

void DoCSWari(HWND hwnd)
{
	WINDOWINFO wi;

	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(MainWnd, &wi);
	if (wi.dwStyle & WS_MINIMIZE)
	{
		PostMessage(MainWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		Sleep(300);
	}
	if (GetForegroundWindow() != MainWnd)
	{
		SetForegroundWindow(MainWnd);
		Sleep(100);
	}

	if (cswari_mode == 0)
	{
		cswari_mode = 1;
		SetTimer(hwnd, 123, 100, DoCSWariSub);

	}
	else
		cswari_mode = 0xFFFFFFFFu;

}

__declspec(naked) void nop5()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
		VIRTUALIZER_SHARK_WHITE_END
}

LRESULT CALLBACK MyMainWndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result;
	unsigned int i;


	__try
	{
		dbg("%d", Msg);
		if (Msg == WM_USER + 5)
		{
			switch (wParam)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				send_packet(packet[wParam].data, packet[wParam].size);
				break;
			case 4:
				send_packet(warp[lParam].data, warp[lParam].size);
				break;
			case 5:
				send_packet(pktbuf, pktbuf_size);
				break;
			}
		}
		else if (Msg == WM_USER + 7)
		{
			if ((i = *(unsigned int*)pt_CUniqueModeless) != 0)
				ReplyMessage(1);
			else
				ReplyMessage(0);
		}
		else if (Msg >= WM_KEYFIRST && Msg <= WM_KEYLAST)
		{
			if (Msg == WM_KEYDOWN || Msg == WM_SYSKEYDOWN)
			{
				switch (wParam)
				{
				case VK_F2:
					if ((i = *(unsigned int *)pt_CUniqueModeless) != 0)
						thiscall2(*(void **)(*(unsigned int *)(i + 4u) + 0u), (void *)(i + 4u), (void *)VK_ESCAPE, (void *)0);
					break;
				case VK_F3:
					if (*(unsigned int *)pt_CUniqueModeless == 0)
						send_packet(packet[1].data, packet[1].size);
					break;
				case VK_F9:
					if ((i = *(unsigned int*)pt_CUserLocal) != 0)
					{
						thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&save_pos);
						msg(10, "좌표: %d, %d", save_pos.v, save_pos.c);
					}
					break;
				}
			}
		}
		else
		{
			switch (Msg)
			{
			case WM_CREATE:
				VIRTUALIZER_TIGER_BLACK_START
				{
					MainWnd = hwnd;
				}
					VIRTUALIZER_TIGER_BLACK_END
					break;
			case WM_CLOSE:
			case WM_DESTROY:
				VIRTUALIZER_TIGER_BLACK_START
				{
					TerminateProcess(GetCurrentProcess(), 0);
				}
					VIRTUALIZER_TIGER_BLACK_END
					break;
			case WM_COPYDATA:
				if (((COPYDATASTRUCT *)lParam)->dwData == 0x02100713u)
				{
					VIRTUALIZER_TIGER_BLACK_START
					{
						send_packet(((COPYDATASTRUCT *)lParam)->lpData, ((COPYDATASTRUCT *)lParam)->cbData);
					}
						VIRTUALIZER_TIGER_BLACK_END
						__leave;
				}
				break;
			}
		}


		result = MainWndProc(hwnd, Msg, wParam, lParam);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return result;
}





ATOM __stdcall MyRegisterClassExA(WNDCLASSEX *lpWndClassEx)
{
	static volatile skip;
	char data[16];

	if (skip == 0 && HIWORD(lpWndClassEx->lpszClassName) && memcmp(lpWndClassEx->lpszClassName, (*(int *)&data[0] = 'lpaM', *(int *)&data[4] = 'otSe', *(int *)&data[8] = 'lCyr', *(int *)&data[12] = 'ssa', data), 16) == 0 && _InterlockedExchange(&skip, ~0) == 0)
	{
		MainWndProc = lpWndClassEx->lpfnWndProc;
		lpWndClassEx->lpfnWndProc = MyMainWndProc;
	}
	return (ATOM)(g_pfnOldRegisterClassExA)(lpWndClassEx);
}

const unsigned char* findvalue(const unsigned char *data, unsigned int size, unsigned int value)
{
	for (unsigned int i = size; i != 0; --i)
		if (*(unsigned int*)&data[i] == value)
			return &data[i];
	return NULL;
}

DWORD WINAPI _상점개설(LPVOID arg)
{
	unsigned int i;

	BlockInput(1);

	SetForegroundWindow(MainWnd);

	Sleep(101);


	switch (해상도)
	{
	case 800:
		MouseClick(658, 37, 1);
		break;
	case 1024:
		MouseClick(875, 38, 1);
		break;
	case 1366:
	case 910:
		MouseClick(1217, 37, 1);
		break;
	}

	Sleep(100);
	PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);
	MouseMove(30, 30);
	Sleep(50);

	switch (해상도)
	{
	case 800:
		MouseClick(658, 37, 2);
		break;
	case 1024:
		MouseClick(875, 38, 2);
		break;
	case 1366:
	case 910:
		MouseClick(1217, 37, 2);
		break;
	}

	Sleep(200);

	if (shop_sel == 0)
	{
		switch (해상도)
		{
		case 800:
			MouseClick(655, 58, 1);
			break;
		case 1024:
			MouseClick(875, 65, 1);
			break;
		case 1366:
		case 910:
			MouseClick(1226, 65, 1);
			break;
		}
	}
	else
	{
		switch (해상도)
		{
		case 800:
			MouseClick(688, 63, 1);
			break;
		case 1024:
			MouseClick(912, 66, 1);
			break;
		case 1366:
		case 910:
			MouseClick(1253, 63, 1);
			break;
		}
	}

	Sleep(150);

	switch (해상도)
	{
	case 800:
		MouseClick(323, 253, 1);
		break;
	case 1024:
		MouseClick(444, 337, 1);
		break;
	case 1366:
	case 910:
		MouseClick(610, 385, 1);
		break;
	}

	Sleep(150);

	for (i = 10; i != 0; --i)
		PostMessage(MainWnd, WM_CHAR, '9', 0);
	Sleep(100);
	PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);

	if (open_slowly)
		Sleep(1500);
	else
		Sleep(100);

	SendPacket(2, 0)
		SendPacket(3, 0)

		if (shop_sel)
		{
			Sleep(101);
			PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
			Sleep(100);
			switch (해상도)
			{
			case 800:
				MouseClick(324, 121, 1);
				break;
			case 1024:
				MouseClick(436, 207, 1);
				break;
			case 1366:
			case 910:
				MouseClick(605, 205, 1);
				break;
			}
		}
	BlockInput(0);

	return 0;
}

void 상점개설()
{
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_상점개설, 0, 0, 0));
}


DWORD WINAPI _일고랜(LPVOID arg)
{

	packet_disallow();

	Sleep(200);

	PostMessage(MainWnd, WM_KEYDOWN, VK_ESCAPE, 0);

	Sleep(201);
	switch (해상도)
	{
	case 800:
		MouseClick(311, 328, 1);
		break;
	case 1024:
		MouseClick(421, 414, 1);
		break;
	case 1366:
	case 910:
		MouseClick(596, 412, 1);
		break;
	}

	Sleep(200);

	Clipboard(title);
	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
	Sleep(100);
	PostMessage(MainWnd, WM_KEYDOWN, VK_INSERT, 0);
	Sleep(100);
	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	Sleep(200);
	PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(300);
	PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(302);


	packet_allow();
	Sleep(300);
	while (*(unsigned int *)pt_CUniqueModeless == 0)
		Sleep(200);
	Sleep(200);

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

void 일고랜()
{
	HANDLE hThread;

	if ((hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_일고랜, 0, 0, 0)))
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

}


char *GetServerName(char value, char *str)
{
	unsigned int i;

	for (i = 0; i < _countof(server); ++i)
	{
		if (server[i].value == value)
		{
			strcpy(str, server[i].name);
			break;
		}
	}
	return str;
}



void packet_handle_inbound(unsigned char *data, unsigned int size)
{
	static DWORD dwLastPing;
	static unsigned int employee_count, last_room;
	unsigned int i;
	int j;
	char buf[128];
	WINDOWINFO wi;

	if (dwLastPing == 0)
		dwLastPing = GetTickCount();

	if (operating && GetTickCount() - dwLastPing >= 20000)
	{
		//decrypt_sel = 0;
		if ((i = *(unsigned int*)pt_CUserLocal) != 0)
		{
			thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&save_pos);
			if (warp_sel == 0 && save_pos.c == 102 || save_pos.c == -108 || save_pos.c == -318)
				decrypt_sel = 0;
			else
				DoCSWari(MainWnd);
		}
	}

	if (*(unsigned short*)data == R_USER_MINIROOM_BALLON)
	{
		if (data[6] != 0 || !operating || response_sel == 2)// || !decrypt_sel)
			return;
		if (warp_sel)
		{
			for (i = 0, j = -1; i < _countof(warp); ++i)
			{
				if (*(unsigned int*)&data[2] == warp[i].id)
				{
					SendPacket(4, i);
					j = i;
					break;
				}
			}
			if (j == -1)
				return; //못찾았단소리
			Sleep(100);
		}
		else if (!target_sel && (*(unsigned int*)&data[2] != userid || target_userid))
			return;

		SendPacket(1, 0)


			Sleep(300);
		wsprintf(buf, "일상 반응횟수: %u회", ++miniroom_count);
		ListBox_Modify(g_hList2, 0, buf);


		if (*(unsigned int *)pt_CUniqueModeless != 0)
		{
			if (only_response || miniroom_sel == 5)
				SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
			else
			{
				wi.cbSize = sizeof(WINDOWINFO);
				GetWindowInfo(MainWnd, &wi);
				if (wi.dwStyle & WS_MINIMIZE)
				{
					PostMessage(MainWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
					Sleep(300);
				}
				if (GetForegroundWindow() != MainWnd)
				{
					SetForegroundWindow(MainWnd);
					Sleep(100);
				}
				if (shop_sel)
					일고랜();
				상점개설();
				SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
			}
		}
		else
			PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
	}
	else if (*(unsigned short*)data == R_EMPLOYEE_LEAVE)
	{
		if (!operating || response_sel == 1)// || !decrypt_sel)
			goto $end;
		if (warp_sel)
		{
			for (i = 0, j = -1; i < _countof(warp); ++i)
			{
				if (*(unsigned int*)&data[2] == warp[i].id)
				{
					SendPacket(4, i);
					j = i;
					break;
				}
			}
			if (j == -1)
				goto $end; //못찾았단소리
			Sleep(100);
		}
		else if (!target_sel)
		{
			if (*(unsigned int*)&data[2] != target_userid)
				goto $end;
		}


		SendPacket(1, 0)


			Sleep(300);
		wsprintf(buf, "고상 반응횟수: %u회", ++employee_count);
		ListBox_Modify(g_hList2, 1, buf);

		if (*(unsigned int *)pt_CUniqueModeless != 0)
		{
			if (only_response)
				SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
			else
			{
				wi.cbSize = sizeof(WINDOWINFO);
				GetWindowInfo(MainWnd, &wi);
				if (wi.dwStyle & WS_MINIMIZE)
				{
					PostMessage(MainWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
					Sleep(301);
				}
				if (GetForegroundWindow() != MainWnd)
				{
					SetForegroundWindow(MainWnd);
					Sleep(105);
				}
				if (shop_sel)
					일고랜();
				상점개설();
				SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
			}
		}
		else
		{
			PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);

			if (target_sel && target_userid)
			{
				ListBox_Modify(g_hList1, 2, "지정 고상: NULL");
				target_sel = 0;
				target_userid = 0;
			}
		}


	$end:;
		for (i = 0; i < _countof(employee_list); ++i)
		{
			if (*(unsigned int*)&data[2] == employee_list[i].id)
			{
				for (j = 0; j < SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_GETCOUNT, 0, 0); ++j)
				{
					SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_GETLBTEXT, j, (LPARAM)buf);
					if (strcmp(buf, employee_list[i].nick) == 0)
					{
						SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_DELETESTRING, j, 0);
						break;
					}
				}
				memset(&employee_list[i], 0, sizeof(employee_list[i]));
				return;
			}
		}
	}
	else if (*(unsigned short*)data == R_EMPLOYEE_LEAVE - 1) //고상 정보 패킷
	{
		if (!operating)// && decrypt_sel)
		{
			if (employee_list[_countof(employee_list) - 1].id != 0) //끝까지 비어있지 않으면
				memset(&employee_list, 0, sizeof(employee_list)); //전부 초기화
			for (i = 0; i < _countof(employee_list); ++i)
				if (employee_list[i].id == 0) //비어있는 곳에서 break
					break;
			employee_list[i].id = *(unsigned int*)&data[2];
			j = *(unsigned short*)&data[16]; //닉네임 사이즈
			memcpy(employee_list[i].nick, &data[18], j);
			employee_list[i].nick[j] = '\0';
			employee_list[i].shopid = *(unsigned int*)&data[16 + 1 + j + 2];
			if (g_hWarpWnd)
				SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)employee_list[i].nick);
		}
	}
	else if (*(unsigned short*)data == R_SHOPINFO)
	{
		if (/*decrypt_sel && */operating && size == 4 && data[2] == 0x1C && data[3] == 0)
		{
			PostMessage(MainWnd, WM_KEYDOWN, VK_ESCAPE, 0);
			Sleep(10);

			SendPacket(1, 0)


				Sleep(300);
			wsprintf(buf, "일상 반응횟수: %u회", ++miniroom_count);
			ListBox_Modify(g_hList2, 0, buf);


			if (*(unsigned int *)pt_CUniqueModeless != 0)
			{
				if (only_response || miniroom_sel == 5)
					SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
				else
				{
					wi.cbSize = sizeof(WINDOWINFO);
					GetWindowInfo(MainWnd, &wi);
					if (wi.dwStyle & WS_MINIMIZE)
					{
						PostMessage(MainWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
						Sleep(300);
					}
					if (GetForegroundWindow() != MainWnd)
					{
						SetForegroundWindow(MainWnd);
						Sleep(100);
					}
					if (shop_sel)
						일고랜();
					상점개설();
					SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
				}
			}
			else
				PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
		}
		else if (data[2] == 0x14 && data[3] == 6)
		{
			if (!operating && GetAsyncKeyState(VK_F1) & 0x8000)
			{
				for (i = 0; i < _countof(employee_list); ++i)
				{
					if (employee_list[i].id == 0)
						continue;
					unsigned char *s;
					if ((s = (unsigned char*)findvalue(data, size, employee_list[i].shopid)))
					{
						for (j = 0; s[j] != 0; --j);
						++j;
						if (s[j - 2] >= 0x4 && s[j - 2] <= 0xC && s[j - 2] == -j)
						{
							wsprintf(buf, "지정 고상: %s", employee_list[i].nick);
							ListBox_Modify(g_hList1, 2, buf);
							target_userid = employee_list[i].id;
							return;
						}
					}
				}
			}
		}
	}
	else if (*(unsigned short*)data == R_PING)
	{
		//if (size == 10)
		{
			dwLastPing = GetTickCount();
			decrypt_sel = 1;
			//dbg("핑옴");
		}
	}
	else if (*(unsigned short*)data == R_SERVERINFO)
	{
		if (data[2] > 19)
			return;
		else if (data[2] == 0)
			channel = 1;
		else if (data[2] == 1)
			channel = 20;
		else
			channel = data[2];
		if (channel == 20)
			wsprintf(buf, "현재 채널: %u세", channel);
		else
			wsprintf(buf, "현재 채널: %u채널", channel);
		ListBox_Modify(g_hList1, 0, buf);
		memset(&employee_list, 0, sizeof(employee_list)); //전부 초기화
		memset(&character_list, 0, sizeof(character_list));
		if (g_hWarpWnd)
		{
			SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_RESETCONTENT, 0, 0);
			SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_INSERTSTRING, 0, (LPARAM)"[초기화]");
		}
		wari_shop_idx = 0;
		decrypt_sel = 1;

		dwLastPing = GetTickCount();
		if (operating)
			SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);


		if (!operating && size >= 100)
		{

			for (i = 0; i < size - 7; ++i)
			{
				if (isserver(data[i]) && memcmp("\x00\x00\x00", &data[i + 1], 3) == 0 && data[i + 4] != 0 && isnickname(&data[i + 4], 4))
				{
					GetServerName(data[i], serverName);
					strcpy(nick, &data[i + 4]);
					SendData("N %s %s", serverName, nick);
					break;
				}
			}

		}

	}
	else if (*(unsigned short*)data == R_USERINFO)
	{
		if (!userid)// && decrypt_sel)
		{
			userid = *(unsigned int*)&data[2];
			wsprintf(buf, "캐릭터 고유 ID: 0x%X", userid);
			ListBox_Modify(g_hList1, 1, buf);
		}
	}
	else if (*(unsigned short*)data == R_CHARACTER_APPEAR)
	{
		if (operating)
			return;
		if (character_list[_countof(character_list) - 1].id != 0)
			memset(&character_list, 0, sizeof(character_list));
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == 0)
			{
				character_list[i].id = *(unsigned int*)&data[2];
				j = *(unsigned short*)&data[7];
				memcpy(character_list[i].nick, &data[9], j);
				character_list[i].nick[j] = '\0';
				if (g_hWarpWnd)
					SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)character_list[i].nick);
				break;
			}
		}
	}
	else if (*(unsigned short*)data == R_CHARACTER_APPEAR + 1)
	{
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*)&data[2])
			{
				for (j = 0; j < SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_GETCOUNT, 0, 0); ++j)
				{
					SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_GETLBTEXT, j, (LPARAM)buf);
					if (strcmp(buf, character_list[i].nick) == 0)
						SendDlgItemMessage(g_hWarpWnd, IDC_COMBO1, CB_DELETESTRING, j, 0);
					memset(&character_list[i], 0, sizeof(character_list[i]));
				}
			}
		}
	}

}

void packet_handle_outbound(unsigned char *data, unsigned int size)
{
	if (*(unsigned short*)data == S_CHARACTER_MOVE)
	{
		if (!operating && GetAsyncKeyState(VK_F5) & 0x8000 && ~warp_prepare_sel && size < sizeof(warp[0].data)) //!= -1
		{
			memcpy(warp[warp_prepare_sel].data, data, size);
			warp[warp_prepare_sel].size = size;
		}
	}
}

__declspec(naked) void nop7()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
		VIRTUALIZER_SHARK_WHITE_END
}

void Clipboard(char *source)
{
	int srclen;
	HGLOBAL clipbuffer;
	char * buffer;

	if (!OpenClipboard(NULL))
		return;

	EmptyClipboard();
	srclen = strlen(source) + 1;
	clipbuffer = GlobalAlloc(GMEM_MOVEABLE, srclen);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy_s(buffer, srclen, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
}

void LOG(unsigned char *data, unsigned int size, unsigned int inbound)
{
	char buf[128];
	int sel;
	LVITEM li;

	if (g_hSnifferWnd && sniffing)
	{
		sel = SendDlgItemMessage(g_hSnifferWnd, IDC_COMBO1, CB_GETCURSEL, 0, 0);
		if ((sel == 1 && inbound) || sel == 2 && !inbound)
			return;
		listidx = ListView_GetItemCount(g_hList3);

		li.mask = LVIF_TEXT;
		li.pszText = inbound ? "Inbound" : "Outbound";
		li.iItem = listidx;
		li.iSubItem = 0;
		SendMessage(g_hList3, LVM_INSERTITEM, 0, (LPARAM)&li);

		wsprintf(buf, "0x%X", size);
		CharUpper(buf);
		buf[1] = 'x';
		ListView_SetItemText(g_hList3, listidx, 1, buf);

		DisplayPacket(data, size, g_hList3, listidx);
	}
}


unsigned int packet_parse(session_t *s, unsigned char *data, unsigned int size, unsigned int inbound)
{
	unsigned int i, j, k;
	unsigned char pkt[65536];

	if (inbound && s->auth == 0u)
	{
		if ((j = *(unsigned __int16 *)data) + 2u > size)
			return 0;
		i = j + 2u;
		j = *(unsigned __int16 *)&data[4];
		send_iv = *(unsigned __int32 *)&data[j + 6u];
		recv_iv = *(unsigned __int32 *)&data[j + 10u];
		s->ver = atoi32((data[j + 6u] = 0u, (char *)&data[6]));
		s->auth = ~0u;

	}
	else
		i = 0;
	if (inbound)
		k = recv_iv;
	else
		k = send_iv;

#if 0
	if (!decrypt_sel && operating && size == 11)
	{

		SendPacket(1, 0)


			Sleep(300);
		wsprintf(buf, "일상 반응횟수: %u회", ++miniroom_count);
		ListBox_Modify(g_hList2, 0, buf);


		if (*(unsigned int *)pt_CUniqueModeless != 0)
		{
			if (only_response || miniroom_sel == 5)
				SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
			else
			{
				wi.cbSize = sizeof(WINDOWINFO);
				GetWindowInfo(MainWnd, &wi);
				if (wi.dwStyle & WS_MINIMIZE)
				{
					PostMessage(MainWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
					Sleep(300);
				}
				if (GetForegroundWindow() != MainWnd)
				{
					SetForegroundWindow(MainWnd);
					Sleep(100);
				}
				if (shop_sel)
					일고랜();
				상점개설();
				SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
			}
		}
		else
			PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
	}
#endif
	for (; (size - i) >= (j = *(unsigned __int16 *)&data[i] ^ *(unsigned __int16 *)&data[i + 2u]) + 4u; i += j + 4u)
	{
		if (operating)
			wzcrypt_decrypt(7, k, &data[i + 4u], pkt);
		else
			wzcrypt_decrypt(j, k, &data[i + 4u], pkt);
		if (inbound)
			packet_handle_inbound(pkt, j);
		else
			packet_handle_outbound(pkt, j);
		LOG(pkt, j, inbound);
		k = wzcrypt_nextiv(k);
	}
	if (inbound)
		recv_iv = k;
	else
		send_iv = k;
	return i;
}

void packet_handler_sub(session_t *s, unsigned char *data, unsigned int size, unsigned int inbound)
{
	unsigned int i;

	if (s->size)
	{ /* fragmented */
		if (size > sizeof(s->data) - s->size)
			__asm int 3;
		memcpy(&s->data[s->size], data, size);
		if ((i = packet_parse(s, s->data, s->size += size, inbound)) != 0)
			if ((s->size -= i) != 0)
				memcpy(s->data, &s->data[i], s->size);
	}
	else if ((i = packet_parse(s, data, size, inbound)) != size)
		memcpy(s->data, &data[i], s->size = size - i);
}




void packet_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
	unsigned int i, j;
	session_t *s;
	iphdr_t *ih;
	tcphdr_t *th;
	unsigned char *src;

	ih = (iphdr_t *)(pkt_data + 14u);
	th = (tcphdr_t *)(pkt_data + 14u + (ih->ihl << 2));
	src = (unsigned char*)&ih->saddr;

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
			startup = 1;
			s->auth = 0u;
			s->size = 0u;
		}
		else if ((s = session[th->dport]) == NULL)
			return;
		if (i = ih->tot_len, (i = (((i & 0xFF) << 8) | (i >> 8)) - (j = (ih->ihl << 2) + (th->doff << 2))) != 0)
			if (!lan_flag)
				packet_handler_sub(s, (unsigned char *)pkt_data + j + 14u, i, (src[0] == 175 && src[1] == 207 && src[2] == 0)); //패킷 방향 체크(메이플 서버 아이피: 175.207.0.xx)
	}
}


__declspec(naked) void nop8()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
		VIRTUALIZER_SHARK_WHITE_END
}

DWORD WINAPI NpfLoop(LPVOID arg)
{

	while (!breakloop)
		NpfCheckMessage();
	NpfStop();

	breakloop = 0;

	return 0;
}



void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey[] = "제시카ㅎㅎ";

	VIRTUALIZER_SHARK_WHITE_START
	{
		for (unsigned int i = 0; i != size; ++i)
			((char *)out)[i] = ((char *)in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
	}
		VIRTUALIZER_SHARK_WHITE_END
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


DWORD WINAPI ClickFunc(LPVOID *arg)
{
	while (operating)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(30);
		PostMessage(MainWnd, WM_CHAR, '/', 0);
		Sleep(30);
		PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(30);
	}
	return 0;
}

void ProcessMessage(char *packet)
{
	unsigned char aeskey[32];
	unsigned char buf[65];
	static unsigned int check;

	VIRTUALIZER_SHARK_WHITE_START
	{

		if (*packet == 'O')
		{
			if (check++)
				return;
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
		else if (*packet == 'M')
			msg(10, "%s", &packet[2]);
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
	SendData("A foo %s %s %s", data, username, version);
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
		if (channel)
			SendData("N %s %s", serverName, nick);
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


void getuuid(char *data)
{
	VIRTUALIZER_TIGER_BLACK_START
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
		VIRTUALIZER_TIGER_BLACK_END
}



LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
	TerminateProcess(GetCurrentProcess(), 0);
	return EXCEPTION_EXECUTE_HANDLER;
}

__declspec(naked) void nop9()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
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


int hex2data(unsigned char *data, const unsigned char *hexstring, unsigned int len)
{
	unsigned const char *pos = hexstring;
	char *endptr;
	size_t count = 0;

	if ((hexstring[0] == '\0') || (strlen(hexstring) & 1))
		return -1;

	for (count = 0; count < len; ++count)
	{
		char buf[5] = { '0', 'x', pos[0], pos[1], 0 };
		data[count] = (unsigned char)strtol(buf, &endptr, 0);
		pos += 2 * sizeof(char);

		if (endptr[0] != '\0'); //return -1(non-hexadecimal character encountered)
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


void init()
{
	WSADATA wsaData;
	DWORD username_size = sizeof(username) - 1;

	VIRTUALIZER_SHARK_WHITE_START
	{
		SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);


		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			MessageBox(GetDesktopWindow(), "WSAStartup() error", 0, 0);
			KillProcess();
		}

		GetUserName(username, &username_size);

	}
		VIRTUALIZER_SHARK_WHITE_END

}

__declspec(naked) void nop10()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
		VIRTUALIZER_SHARK_WHITE_END
}


BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		VIRTUALIZER_SHARK_WHITE_START
		{
			DisableThreadLibraryCalls(hModule);
			module_base = hModule;
			client_base = GetModule(0xEC755B21u);

			init_rand(GetCurrentProcessId() ^ 0xBADF00Du);
			server_init(server);
			init();

			FillNOP(0, 0, 0, 0);

			HookExportedProc(GetModuleHandle("ws2_32.dll"), "connect", (LPVOID*)&g_pfnOldconnect, _connect);
			HookExportedProc(GetModuleHandle("user32.dll"), "RegisterClassExA", (LPVOID*)&g_pfnOldRegisterClassExA, MyRegisterClassExA);

			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, 0, 0, 0));

		}
			VIRTUALIZER_SHARK_WHITE_END
			break;
	}
	return TRUE;
}



void ReadINI()
{
	GetModuleFileName(module_base, INIPath, sizeof(INIPath) - 1);

	memcpy(&INIPath[strlen(INIPath) - 3], "ini", 3);

	if (!(R_SERVERINFO = GetPrivateProfileInt("PACKET", "R_SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_SERVERINFO", "", INIPath);
	if (!(R_USER_MINIROOM_BALLON = GetPrivateProfileInt("PACKET", "R_USER_MINIROOM_BALLON", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_USER_MINIROOM_BALLON", "", INIPath);
	if (!(R_EMPLOYEE_LEAVE = GetPrivateProfileInt("PACKET", "R_EMPLOYEE_LEAVE", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_EMPLOYEE_LEAVE", "", INIPath);
	if (!(R_SHOPINFO = GetPrivateProfileInt("PACKET", "R_SHOPINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_SHOPINFO", "", INIPath);
	if (!(R_USERINFO = GetPrivateProfileInt("PACKET", "R_USERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_USERINFO", "", INIPath);
	if (!(R_CHARACTER_APPEAR = GetPrivateProfileInt("PACKET", "R_CHARACTER_APPEAR", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_CHARACTER_APPEAR", "", INIPath);
	if (!(R_PING = GetPrivateProfileInt("PACKET", "R_PING", 0, INIPath)))
		WritePrivateProfileString("PACKET", "R_PING", "", INIPath);

	if (!(S_MINIROOM = GetPrivateProfileInt("PACKET", "S_MINIROOM", 0, INIPath)))
		WritePrivateProfileString("PACKET", "S_MINIROOM", "", INIPath);
	if (!(S_CHARACTER_MOVE = GetPrivateProfileInt("PACKET", "S_CHARACTER_MOVE", 0, INIPath)))
		WritePrivateProfileString("PACKET", "S_CHARACTER_MOVE", "", INIPath);

	if (!GetPrivateProfileString("SETTING", "제목", "", title, sizeof(title) - 1, INIPath))
		WritePrivateProfileString("SETTING", "제목", "foo", INIPath);
	if (!(target_sel = GetPrivateProfileInt("SETTING", "target_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "target_sel", "0", INIPath);
	if (!(adapter_sel = GetPrivateProfileInt("SETTING", "adapter_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "adapter_sel", "0", INIPath);
	if (!(only_response = GetPrivateProfileInt("SETTING", "only_response", 0, INIPath)))
		WritePrivateProfileString("SETTING", "only_response", "0", INIPath);
	if (!(minimize_sel = GetPrivateProfileInt("SETTING", "minimize_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "minimize_sel", "0", INIPath);
	if (!(open_slowly = GetPrivateProfileInt("SETTING", "open_slowly", 0, INIPath)))
		WritePrivateProfileString("SETTING", "open_slowly", "0", INIPath);
	if (!(shop_sel = GetPrivateProfileInt("SETTING", "shop_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "shop_sel", "0", INIPath);
	if (!(response_sel = GetPrivateProfileInt("SETTING", "response_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "response_sel", "0", INIPath);
	if (!(miniroom_sel = GetPrivateProfileInt("SETTING", "miniroom_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "miniroom_sel", "0", INIPath);

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

VOID CALLBACK DoEmptyWorkingSet(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	HANDLE process;
	PROCESS_MEMORY_COUNTERS stat[2];
	SYSTEMTIME now;

	VIRTUALIZER_TIGER_BLACK_START
	{
		if ((process = GetCurrentProcess()) != NULL)
		{
			memset(stat, 0, sizeof(stat));
			GetProcessMemoryInfo(process, &stat[0], sizeof(stat[0]));
			SetProcessWorkingSetSize(process, -1, -1);
			GetProcessMemoryInfo(process, &stat[1], sizeof(stat[1]));
			GetLocalTime(&now);
		}
	}
		VIRTUALIZER_TIGER_BLACK_END

}


VOID WaitUntil(int(*func)(DWORD[]), DWORD param[], DWORD dwMilliseconds)
{
	DWORD start;
	MSG msg;

	start = GetTickCount();

	while (dwMilliseconds ? (GetTickCount() - start < dwMilliseconds) : 1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				PostQuitMessage(msg.wParam);
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (func && func(param))
				break;
			Sleep(10);
		}
	}
}

int check_func(DWORD param[])
{
	static int check;

	if (GetAsyncKeyState(VK_F5) & 0x8001)
		check = 1;
	if (check && GetAsyncKeyState(VK_F5) == 0)
		return 1;
	else
		return 0;
}


BOOL CALLBACK WarpDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	unsigned int i, j, idx;
	char buf[128];

	switch (iMessage)
	{
	case WM_INITDIALOG:
		g_hWarpWnd = hDlg;
		g_hWarpList[0] = GetDlgItem(hDlg, IDC_LIST1);
		g_hWarpList[1] = GetDlgItem(hDlg, IDC_LIST3);
		g_hWarpList[2] = GetDlgItem(hDlg, IDC_LIST4);
		g_hWarpList[3] = GetDlgItem(hDlg, IDC_LIST5);
		g_hWarpList[4] = GetDlgItem(hDlg, IDC_LIST6);
		g_hWarpList[5] = GetDlgItem(hDlg, IDC_LIST7);
		g_hWarpList[6] = GetDlgItem(hDlg, IDC_LIST8);
		g_hWarpList[7] = GetDlgItem(hDlg, IDC_LIST9);
		g_hWarpList[8] = GetDlgItem(hDlg, IDC_LIST10);
		g_hWarpList[9] = GetDlgItem(hDlg, IDC_LIST11);
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, 2, 0);

		for (i = 0; i < _countof(warp); ++i)
		{
			if (warp[i].id != 0)
				SendMessage(g_hWarpList[i], LB_INSERTSTRING, 0, (LPARAM)warp[i].nick);
			if (*warp[i].data)
			{
				switch (i)
				{
				case 0:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON7, "세팅 완료");
					break;
				case 1:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON8, "세팅 완료");
					break;
				case 2:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON9, "세팅 완료");
					break;
				case 3:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON10, "세팅 완료");
					break;
				case 4:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON11, "세팅 완료");
					break;
				case 5:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON12, "세팅 완료");
					break;
				case 6:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON13, "세팅 완료");
					break;
				case 7:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON14, "세팅 완료");
					break;
				case 8:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON15, "세팅 완료");
					break;
				case 9:
					SetDlgItemText(g_hWarpWnd, IDC_BUTTON16, "세팅 완료");
					break;
				}
			}
		}
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_INSERTSTRING, 0, (LPARAM)"[초기화]");
		for (i = 0; i < _countof(employee_list); ++i)
		{
			if (employee_list[i].id != 0)
				SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)employee_list[i].nick);
		}
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id != 0)
				SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)character_list[i].nick);
		}
		CheckDlgButton(hDlg, IDC_CHECK1, warp_sel);
		if (IsDlgButtonChecked(hDlg, IDC_CHECK1) == BST_UNCHECKED)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON7), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON8), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON9), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON10), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON11), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON12), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON13), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON14), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON15), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON16), FALSE);
		}
		break;
	case WM_CLOSE:
		g_hWarpWnd = 0;
		DestroyWindow(hDlg);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			if (GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT1)) == 0)
			{
				MessageBox(0, "몇 번째에 지정할지 적어주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}
			if ((i = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0)) == CB_ERR)
			{
				MessageBox(0, "지정할 상점을 선택해주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}

			SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETLBTEXT, i, (LPARAM)buf);
			idx = GetDlgItemInt(hDlg, IDC_EDIT1, 0, 0);
			if (!(1 <= idx && idx <= 10))
			{
				MessageBox(0, "1 ~ 10 사이로 적어주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}
			--idx;
			if (strcmp(buf, "[초기화]") == 0)
			{
				strcpy(warp[idx].nick, "");
				warp[idx].id = 0;
				ListBox_Modify(g_hWarpList[idx], 0, "");
			}
			else
			{
				j = 0;
				for (i = 0; i < _countof(employee_list); ++i)
				{
					if (strcmp(buf, employee_list[i].nick) == 0)
					{
						strcpy(warp[idx].nick, employee_list[i].nick);
						warp[idx].id = employee_list[i].id;
						ListBox_Modify(g_hWarpList[idx], 0, warp[idx].nick);
						j = 777;
						break;
					}
				}
				if (j != 777)
				{
					for (i = 0; i < _countof(character_list); ++i)
					{
						if (strcmp(buf, character_list[i].nick) == 0)
						{
							strcpy(warp[idx].nick, character_list[i].nick);
							warp[idx].id = character_list[i].id;
							ListBox_Modify(g_hWarpList[idx], 0, warp[idx].nick);
							break;
						}
					}
				}
			}
			break;
		case IDC_BUTTON7:
		case IDC_BUTTON8:
		case IDC_BUTTON9:
		case IDC_BUTTON10:
		case IDC_BUTTON11:
		case IDC_BUTTON12:
		case IDC_BUTTON13:
		case IDC_BUTTON14:
		case IDC_BUTTON15:
		case IDC_BUTTON16:
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON7:
				warp_prepare_sel = 0;
				break;
			case IDC_BUTTON8:
				warp_prepare_sel = 1;
				break;
			case IDC_BUTTON9:
				warp_prepare_sel = 2;
				break;
			case IDC_BUTTON10:
				warp_prepare_sel = 3;
				break;
			case IDC_BUTTON11:
				warp_prepare_sel = 4;
				break;
			case IDC_BUTTON12:
				warp_prepare_sel = 5;
				break;
			case IDC_BUTTON13:
				warp_prepare_sel = 6;
				break;
			case IDC_BUTTON14:
				warp_prepare_sel = 7;
				break;
			case IDC_BUTTON15:
				warp_prepare_sel = 8;
				break;
			case IDC_BUTTON16:
				warp_prepare_sel = 9;
				break;
			}
			SetDlgItemText(hDlg, LOWORD(wParam), "위치 세팅중");
			EnableWindow(GetDlgItem(hDlg, LOWORD(wParam)), FALSE);
			i = 0;
			WaitUntil(check_func, 0, 0);
			
			if (!*warp[warp_prepare_sel].data)
			{
				SetDlgItemText(hDlg, LOWORD(wParam), "위치 세팅");
				MessageBox(0, "F5를 누른 상태로 캐릭터를 워프할 위치로 움직인 후 떼 주세요", "알림", MB_ICONINFORMATION);
			}
			else
				SetDlgItemText(hDlg, LOWORD(wParam), "세팅 완료");
			warp_prepare_sel = -1;
			EnableWindow(GetDlgItem(hDlg, LOWORD(wParam)), TRUE);

			break;
		case IDC_CHECK1:
			if (IsDlgButtonChecked(hDlg, IDC_CHECK1) == BST_CHECKED)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON7), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON8), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON9), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON10), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON11), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON12), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON13), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON14), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON15), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON16), TRUE);
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON7), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON8), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON9), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON10), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON11), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON12), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON13), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON14), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON15), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_BUTTON16), FALSE);
			}
			break;
		case IDOK:
			warp_sel = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			if (shop_sel && warp_sel)
				MessageBox(GetDesktopWindow(), "워프 후 자동일고랜을 하면 삑이 날 수 있습니다", "알림", MB_ICONINFORMATION);
			g_hWarpWnd = 0;
			DestroyWindow(hDlg);
			break;
		case IDCANCEL:
			g_hWarpWnd = 0;
			DestroyWindow(hDlg);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


BOOL CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	unsigned int count, i;
	char name[128], desc[128], buf[128];
	char year[8] = { 0, }, month[8] = { 0, }, day[8] = { 0, };

	switch (iMessage)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_EDIT2, EM_LIMITTEXT, 50, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"ON");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"OFF");
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_ADDSTRING, 0, (LPARAM)"일반상점");
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_ADDSTRING, 0, (LPARAM)"일반상점 + 자동일고랜");
		SendDlgItemMessage(hDlg, IDC_COMBO5, CB_ADDSTRING, 0, (LPARAM)"모두 반응");
		SendDlgItemMessage(hDlg, IDC_COMBO5, CB_ADDSTRING, 0, (LPARAM)"일반상점");
		SendDlgItemMessage(hDlg, IDC_COMBO5, CB_ADDSTRING, 0, (LPARAM)"고용상인");
		SendDlgItemMessage(hDlg, IDC_COMBO6, CB_ADDSTRING, 0, (LPARAM)"일반 상점오픈 허가증");
		SendDlgItemMessage(hDlg, IDC_COMBO6, CB_ADDSTRING, 0, (LPARAM)"하늘색 나무집 상점오픈 허가증");
		SendDlgItemMessage(hDlg, IDC_COMBO6, CB_ADDSTRING, 0, (LPARAM)"분홍색 나무집 상점오픈 허가증");
		SendDlgItemMessage(hDlg, IDC_COMBO6, CB_ADDSTRING, 0, (LPARAM)"주황색 나무집 상점오픈 허가증");
		SendDlgItemMessage(hDlg, IDC_COMBO6, CB_ADDSTRING, 0, (LPARAM)"연두색 나무집 상점오픈 허가증");
		SendDlgItemMessage(hDlg, IDC_COMBO6, CB_ADDSTRING, 0, (LPARAM)"같은그림찾기세트");
		SendDlgItemMessage(hDlg, IDC_COMBO7, CB_ADDSTRING, 0, (LPARAM)"메인 프로시저 후킹");
		SendDlgItemMessage(hDlg, IDC_COMBO7, CB_ADDSTRING, 0, (LPARAM)"OffsetRect() 후킹");
		count = NpfFindAllDevices();
		for (i = 1; i <= count; ++i)
		{
			NpfGetDeviceInfo(i, name, desc);
			SendDlgItemMessage(hDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)desc);
		}
		ReadINI();
		SetDlgItemText(hDlg, IDC_EDIT2, title);
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, target_sel, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO2, CB_SETCURSEL, adapter_sel, 0);
		CheckDlgButton(hDlg, IDC_CHECK1, only_response);
		CheckDlgButton(hDlg, IDC_CHECK3, minimize_sel);
		CheckDlgButton(hDlg, IDC_CHECK4, open_slowly);
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_SETCURSEL, shop_sel, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO5, CB_SETCURSEL, response_sel, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO6, CB_SETCURSEL, miniroom_sel, 0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			packet_allow();
			break;
		case 1003:
			packet_disallow();
			break;
		case 1021:
			if (!PacketWnd)
				PacketWnd = CreateDialog(module_base, MAKEINTRESOURCE(IDD_DIALOG1), g_hWnd, DlgPacketProc);
			else
				SetForegroundWindow(PacketWnd);
			break;
		case 1022:
			if (!g_hWarpWnd)
				g_hWarpWnd = CreateDialog(module_base, MAKEINTRESOURCE(IDD_DIALOG4), g_hWnd, WarpDlgProc);
			else
				SetForegroundWindow(g_hWarpWnd);
			break;
		case 1023:
			i = wari_state;
			wari_state = 1;
			와리(0, 0, 0, 0);
			wari_state = i;
			break;
		case 1024:
			if ((i = *(unsigned int*)pt_CUserLocal) != 0 && cswari_mode == 0)
			{
				thiscall1(*(void **)(*(unsigned int *)(i + 4u) + pt_getpos), (void *)(i + 4u), (void *)&save_pos);
				if (save_pos.c == -146 || save_pos.c == -206 || save_pos.c == -416 || save_pos.c == 34 || save_pos.c == -26 || save_pos.c == -86)
					DoCSWari(MainWnd);
				else
					MessageBox(0, "공중이나 사다리에서는 캐샵와리 테스트를 할 수 없습니다", "알림", MB_ICONINFORMATION);
			}
			break;
		case 1025:
			sscanf(time_remaning, "%4s%2s%2s", year, month, day);
			wsprintf(buf, "%s년 %s월 %s일까지 사용 가능합니다!", year, month, day);
			MessageBox(0, buf, "알림", MB_ICONINFORMATION);
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT2, title, sizeof(title) - 1);
			target_sel = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
			adapter_sel = SendDlgItemMessage(hDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0);
			only_response = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			minimize_sel = IsDlgButtonChecked(hDlg, IDC_CHECK3);
			open_slowly = IsDlgButtonChecked(hDlg, IDC_CHECK4);
			shop_sel = SendDlgItemMessage(hDlg, IDC_COMBO4, CB_GETCURSEL, 0, 0);
			response_sel = SendDlgItemMessage(hDlg, IDC_COMBO5, CB_GETCURSEL, 0, 0);
			miniroom_sel = SendDlgItemMessage(hDlg, IDC_COMBO6, CB_GETCURSEL, 0, 0);

			if (only_response && shop_sel)
			{
				MessageBox(GetDesktopWindow(), "반응만 하기와 자동일고랜이 함께 선택될 수 없습니다", "알림", MB_ICONERROR);
				return 0;
			}
			if (shop_sel && warp_sel)
				MessageBox(GetDesktopWindow(), "워프 후 자동일고랜을 하면 삑이 날 수 있습니다", "알림", MB_ICONINFORMATION);
			WritePrivateProfileString("SETTING", "제목", title, INIPath);
			wsprintf(buf, "%u", target_sel);
			WritePrivateProfileString("SETTING", "target_sel", buf, INIPath);
			wsprintf(buf, "%u", adapter_sel);
			WritePrivateProfileString("SETTING", "adapter_sel", buf, INIPath);
			wsprintf(buf, "%u", only_response);
			WritePrivateProfileString("SETTING", "only_response", buf, INIPath);
			wsprintf(buf, "%u", minimize_sel);
			WritePrivateProfileString("SETTING", "minimize_sel", buf, INIPath);
			wsprintf(buf, "%u", open_slowly);
			WritePrivateProfileString("SETTING", "open_slowly", buf, INIPath);
			wsprintf(buf, "%u", shop_sel);
			WritePrivateProfileString("SETTING", "shop_sel", buf, INIPath);
			wsprintf(buf, "%u", response_sel);
			WritePrivateProfileString("SETTING", "response_sel", buf, INIPath);
			wsprintf(buf, "%u", miniroom_sel);
			WritePrivateProfileString("SETTING", "miniroom_sel", buf, INIPath);
			PrepareShopPacket();
			NpfGetDeviceInfo(adapter_sel + 1, name, desc);
			NpfSetDevice(name);
			NpfStart();
			g_hSettingWnd = 0;
			DestroyWindow(hDlg);
			break;
		case IDCANCEL:
			g_hSettingWnd = 0;
			DestroyWindow(hDlg);
			break;
		case IDC_BUTTON6:
			if (!g_hSnifferWnd)
				g_hSnifferWnd = CreateDialog(module_base, MAKEINTRESOURCE(IDD_DIALOG5), NULL, SnifferDlgProc);
			else
				SetForegroundWindow(g_hSnifferWnd);
			break;
		}
		break;
		//case WM_CLOSE:
		//g_hSettingWnd = 0;
		//DestroyWindow(hDlg);
		//return 0;
	default:
		return FALSE;
	}
	return TRUE;
}

unsigned int GetMapleResolution(HWND hwnd)
{
	RECT rect;
	unsigned int xs, ys;

	GetClientRect(hwnd, &rect);
	xs = rect.right - rect.left;
	ys = rect.bottom - rect.top;


	if (xs == 800 && ys == 600)
		return 800;
	else if (xs == 1024 && ys == 768)
		return 1024;
	else if (xs == 1366 && ys == 768)
		return 1366;
	else if (xs == 910 && ys == 512)
		return 910;
	else
		return 0;

}


void MouseMove(unsigned int x, unsigned int y)
{
	POINT xy = { x, y };
	ClientToScreen(MainWnd, &xy);
	SetCursorPos(xy.x, xy.y);
}

void MouseClick(unsigned int x, unsigned int y, unsigned int k)
{
	POINT xy = { x, y };
	ClientToScreen(MainWnd, &xy);
	SetCursorPos(xy.x, xy.y);
	Sleep(10);
	for (unsigned int i = 0; i < k; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}




void CALLBACK 와리(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	unsigned int temp, i;
	WINDOWINFO wi;

	if (!wari_state || only_response)
		return;


	temp = operating;
	operating = 0;


	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(MainWnd, &wi);
	if (wi.dwStyle & WS_MINIMIZE)
	{
		PostMessage(MainWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		Sleep(300);
	}


	SetForegroundWindow(MainWnd);
	Sleep(100);

	해상도 = GetMapleResolution(MainWnd);

	if (shop_sel)
	{
		switch (해상도)
		{
		case 800:
			MouseClick(488, 362, 1);
			break;
		case 1024:
			MouseClick(602, 445, 1);
			break;
		case 1366:
		case 910:
			MouseClick(773, 450, 1);
			break;
		}
		Sleep(150);
	}



	if (wari_shop_idx == _countof(employee_list) - 1)
		wari_shop_idx = 0;

	while (1)
	{
		for (i = wari_shop_idx; i < _countof(employee_list); ++i)
		{
			if (employee_list[i].shopid != 0)
			{
				wari_shop_idx = i;
				break;
			}
		}
		if (i == _countof(employee_list))
			return;
		*(unsigned int*)&packet[0].data[3] = employee_list[wari_shop_idx].shopid;

		SendPacket(0, 0)


			Sleep(500);
		if (*(unsigned int *)pt_CUniqueModeless != 0)
		{
			PostMessage(MainWnd, WM_KEYDOWN, VK_ESCAPE, 0);
			break;
		}
		++wari_shop_idx;
		PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(200);
	}

	Sleep(50);
	PostMessage(MainWnd, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);

	if (shop_sel) //자동일고랜
	{
		switch (해상도)
		{
		case 800:
			MouseClick(776, 38, 1);
			break;
		case 1024:
			MouseClick(999, 37, 1);
			break;
		case 1366:
		case 910:
			MouseClick(1340, 41, 1);
			break;
		}
		Sleep(150);
		switch (해상도)
		{
		case 800:
			MouseClick(693, 63, 2);
			break;
		case 1024:
			MouseClick(917, 67, 2);
			break;
		case 1366:
		case 910:
			MouseClick(1259, 62, 2);
			break;
		}
		Sleep(201);

	}

	if (minimize_sel)
	{
		PostMessage(MainWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		Sleep(205);
	}
	operating = temp;
}


void GetWindowPos(HWND hwnd, RECT *rect)
{
	GetWindowRect(hwnd, rect);

	ScreenToClient(GetParent(hwnd), &((POINT*)rect)[0]);
	ScreenToClient(GetParent(hwnd), &((POINT*)rect)[1]);
}


LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (iMessage)
	{
	case WM_KEYDOWN:
		if (wParam == 'A' && GetKeyState(VK_CONTROL) & 0x8000)
			SendMessage(hWnd, EM_SETSEL, 0, -1);
		break;
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}

BOOL CALLBACK SnifferDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	LVCOLUMN col;
	char buf[256];

	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK1:
			sniffing = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			break;
		}
		break;
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->idFrom == IDC_LIST1)
		{
			if (((LPNMLISTVIEW)lParam)->hdr.code == LVN_ITEMCHANGED)
			{
				if (((LPNMLISTVIEW)lParam)->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
				{
					ListView_GetItemText(g_hList3, ((LPNMLISTVIEW)lParam)->iItem, 1, buf, sizeof(buf));
					SetDlgItemText(hDlg, IDC_EDIT1, buf);
					ListView_GetItemText(g_hList3, ((LPNMLISTVIEW)lParam)->iItem, 2, buf, sizeof(buf));
					SetDlgItemText(hDlg, IDC_EDIT2, buf);

				}
			}
		}
		break;
	case WM_INITDIALOG:
		g_hList3 = GetDlgItem(hDlg, IDC_LIST1);
		listidx = 0;
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"안 함");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"Inbound");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"Outbound");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, 0, 0);
		ListView_SetExtendedListViewStyle(g_hList3, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		col.fmt = LVCFMT_LEFT;
		col.cx = 100;
		col.pszText = "방향";
		col.cchTextMax = strlen("방향");
		col.iSubItem = 0;
		SendMessage(g_hList3, LVM_INSERTCOLUMN, 1, (LPARAM)&col);

		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		col.fmt = LVCFMT_LEFT;
		col.cx = 80;
		col.pszText = "사이즈";
		col.cchTextMax = strlen("사이즈");
		col.iSubItem = 1;
		SendMessage(g_hList3, LVM_INSERTCOLUMN, 1, (LPARAM)&col);

		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		col.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
		col.cx = 420;
		col.pszText = "패킷";
		col.cchTextMax = strlen("패킷");
		col.iSubItem = 2;
		SendMessage(g_hList3, LVM_INSERTCOLUMN, 2, (LPARAM)&col);

		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		col.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
		col.cx = 420;
		col.pszText = "아스키";
		col.cchTextMax = strlen("아스키");
		SendMessage(g_hList3, LVM_INSERTCOLUMN, 3, (LPARAM)&col);

		CheckDlgButton(hDlg, IDC_CHECK1, (sniffing = 1));

		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), EditSubclassProc, 0, 0);
		SetWindowSubclass(GetDlgItem(hDlg, IDC_EDIT2), EditSubclassProc, 1, 0);
		break;
	case WM_CLOSE:
		g_hSnifferWnd = 0;
		RemoveWindowSubclass(GetDlgItem(hDlg, IDC_EDIT1), EditSubclassProc, 0);
		RemoveWindowSubclass(GetDlgItem(hDlg, IDC_EDIT2), EditSubclassProc, 1);
		DestroyWindow(hDlg);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}




BOOL CALLBACK FindDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char data[128];
	char buf[128];

	switch (iMessage)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, 12, 0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT1)) == 0)
			{
				MessageBox(GetDesktopWindow(), "닉네임을 입력해주세요.", "알림", MB_ICONINFORMATION);
				return 0;
			}
			GetDlgItemText(hDlg, IDC_EDIT1, data, sizeof(data));
			for (int i = 0; i < _countof(employee_list); ++i)
			{
				if (lstrcmpi(data, employee_list[i].nick) == 0)
				{
					wsprintf(buf, "지정 고상: %s", employee_list[i].nick);
					ListBox_Modify(g_hList1, 2, buf);
					target_userid = employee_list[i].id;

					break;
				}
				if (i == 39)
				{
					MessageBox(GetDesktopWindow(), "해당 고용상인을 찾지 못했습니다.", "알림", MB_ICONINFORMATION);
					return 0;
				}
			}
			g_hFindWnd = 0;
			DestroyWindow(hDlg);
			break;
		case IDCANCEL:
			g_hFindWnd = 0;
			DestroyWindow(hDlg);
			break;
		}
		break;
		//case WM_CLOSE:
		//	g_hFindWnd = 0;
		//	EndDialog(hDlg, 0);
		//	break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char name[128], desc[128];

	switch (iMessage)
	{
	case WM_SOCKET:
		return ProcessSocketMessage(hDlg, wParam, lParam);
	case WM_HOTKEY:
		if (wParam == 3)
		{
			ListBox_Modify(g_hList1, 2, "지정 고상: NULL");
			target_userid = 0;
			return 0;
		}
		else if (wParam == 10)
		{
			if (operating)
				return 0;
		}
		else if (wParam == 11)
			operating = 1;
		SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
		break;
	case WM_INITDIALOG:
		SetWindowText(hDlg, "");
		VIRTUALIZER_SHARK_WHITE_START
		{
			g_hWnd = hDlg;
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
		g_hButton4 = GetDlgItem(hDlg, IDC_BUTTON4);
		SystemParametersInfo(SPI_SETKEYBOARDDELAY, 0, 0, SPIF_UPDATEINIFILE);
		SystemParametersInfo(SPI_SETKEYBOARDSPEED, 31, 0, SPIF_UPDATEINIFILE);
		packet_init();
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM)"현재 채널: NULL");
		SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"캐릭터 고유 ID: 0");
		SendMessage(g_hList1, LB_INSERTSTRING, 2, (LPARAM)"지정 고상: NULL");
		SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)"일상 반응횟수: 0회");
		SendMessage(g_hList2, LB_INSERTSTRING, 1, (LPARAM)"고상 반응횟수: 0회");

		if (RegisterHotKey(g_hWnd, 3, MOD_ALT | MOD_NOREPEAT, VK_F3) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(GetDesktopWindow(), "핫키 등록 실패!: ALT + F3", "알림", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 10, MOD_NOREPEAT, VK_F10) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(GetDesktopWindow(), "핫키 등록 실패!: F10", "알림", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 11, MOD_NOREPEAT, VK_F11) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(GetDesktopWindow(), "핫키 등록 실패!: F11", "알림", MB_ICONERROR);
		ReadINI();
		switch (response_sel)
		{
		case 0:
			CheckDlgButton(hDlg, IDC_RADIO1, 1);
			break;
		case 1:
			CheckDlgButton(hDlg, IDC_RADIO3, 1);
			break;
		case 2:
			CheckDlgButton(hDlg, IDC_RADIO4, 1);
			break;
		}
		PrepareShopPacket();
		SetTimer(g_hWnd, 1, 39 * 60 * 1000, 와리);
		SetTimer(g_hWnd, 2, 20 * 60 * 1000, DoEmptyWorkingSet);
		SetTimer(g_hWnd, 3, 5 * 60 * 1000, FillNOP);


		if (NpfFindAllDevices() >= adapter_sel + 1)
		{
			NpfGetDeviceInfo(adapter_sel + 1, name, desc);
			NpfSetDevice(name);
			if (NpfStart())
				hPcapThread = CreateThread(0, 0, NpfLoop, 0, 0, 0);
			else
			{
				MessageBox(GetDesktopWindow(), "NpfStart() error!", "알림", MB_ICONERROR);
				KillProcess();

			}
		}
		else
			MessageBox(GetDesktopWindow(), "ini에서 읽어온 어댑터가 잘못됐으니 재설정을 해주세요.", "알림", MB_ICONERROR);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON2: //사용법
			MessageBox(GetDesktopWindow(), "※핫키 설명※\nF1: 반응할 고상 지정\nF2: 상점 열려있으면 ESC 날림(일상창 위에 고상 제목창이 있어도 됨)\nALT + F3: 지정 고상 초기화\nF3: 투상 펴기\nF5: 워프 위치 세팅\nF9: 캐릭터 좌표 확인\nF10: 인식 시작\nF11: 인식 정지", "알림", MB_ICONINFORMATION);
			MessageBox(GetDesktopWindow(), "일상이 닫혀도 반응하지 않는 경우\n1. 인식 작동 중이 아닐 때\n2. 반응상점이 고용상인일 때\n3. 지정 상점이 ON이면서 내가 지정한 상점 주인과 일상 닫은 캐릭이 다르거나 지정 고상이 있는 경우", "알림", MB_ICONINFORMATION);
			MessageBox(GetDesktopWindow(), "고상이 닫혀도 반응하지 않는 경우\n1. 인식 작동 중이 아닐 때\n2. 반응상점이 일반상점일 때\n3. 지정 상점이 ON이면서 내가 지정한 고상과 닫힌 고상이 다른 경우", "알림", MB_ICONINFORMATION);
			MessageBox(GetDesktopWindow(), "인식이 작동 중일 때는 워프 위치 세팅이 안 되고 지정 상점이 잡히지 않고 캐릭터가 맵에 나타나도 패킷에서 정보를 읽어오지 않음\n작동 중이 아닌데도 F1로 고상 지정이 안 된다면 직접 지정 하면 됨", "알림", MB_ICONINFORMATION);
			MessageBox(GetDesktopWindow(), "※캐릭터 고유 ID 따는 법※\n대기탈 일상에 들어가서 판매자정보 버튼을 누르거나\n대기탈 일상 주인 캐릭을 우클릭하고 개인정보 버튼을 누르면 됨", "알림", MB_ICONINFORMATION);
			MessageBox(GetDesktopWindow(), "※반응할 고상 지정하는 법※\nF1을 누른상태로 반응할 고상에 들어가서 F1을 뗀다", "알림", MB_ICONINFORMATION);
			MessageBox(GetDesktopWindow(), "※워프 위치 세팅하는 법※\n워프 설정창에서 위치 세팅 버튼을 누르고 버튼이 비활성화되면\nF5를 누른 상태로 날아갈 자리로 가서 F5를 뗀다", "알림", MB_ICONINFORMATION);
			MessageBox(GetDesktopWindow(), "인식이랑 센더는 같이 쓸 수 없음", "알림", MB_ICONINFORMATION);
			MessageBox(GetDesktopWindow(), "랜작을 하면 낮은 확률로 복호화가 꼬임. 스니퍼로 패킷을 봐서 이상하게 깨져서 나온다면 캐샵을 갔다 와야 됨", "알림", MB_ICONINFORMATION);
			break;
		case IDC_BUTTON3: //설정
			if (!g_hSettingWnd)
				g_hSettingWnd = CreateDialog(module_base, MAKEINTRESOURCE(IDD_DIALOG3), g_hWnd, SettingDlgProc);
			else
				SetForegroundWindow(g_hSettingWnd);
			break;
		case IDC_BUTTON4:
			해상도 = GetMapleResolution(MainWnd);
			if (startup == 0)
			{
				MessageBox(GetDesktopWindow(), "캐시샵을 갔다 와서 현재 채널이\n제대로 표시된 후 대기를 시작해주세요.", "알림", MB_ICONERROR);
				return 0;
			}

			operating = !operating;
			if (operating)
			{
				SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
				SetThreadPriority(hPcapThread, THREAD_PRIORITY_HIGHEST);
				SetWindowText(g_hButton4, "인식 정지[F11]");
				if (shop_sel) //자동일고랜
				{
					switch (해상도)
					{
					case 800:
						MouseClick(776, 38, 1);
						break;
					case 1024:
						MouseClick(999, 37, 1);
						break;
					case 1366:
					case 910:
						MouseClick(1340, 41, 1);
						break;
					}
					Sleep(151);
					switch (해상도)
					{
					case 800:
						MouseClick(693, 63, 2);
						break;
					case 1024:
						MouseClick(917, 67, 2);
						break;
					case 1366:
					case 910:
						MouseClick(1259, 62, 2);
						break;
					}
					Sleep(201);

				}
				wari_state = 1;
				if (minimize_sel)
					PostMessage(MainWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
			}
			else
			{
				SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				SetThreadPriority(hPcapThread, THREAD_PRIORITY_NORMAL);
				SetWindowText(g_hButton4, "인식 시작[F10]");

				PostMessage(MainWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
				SetForegroundWindow(MainWnd);
				wari_state = 0;
			}
			break;
		case IDC_BUTTON5:
			userid = 0;
			ListBox_Modify(g_hList1, 1, "캐릭터 고유 ID: 0");
			break;
		case IDC_BUTTON6:
			if (!g_hFindWnd)
				g_hFindWnd = CreateDialog(module_base, MAKEINTRESOURCE(IDD_DIALOG6), NULL, FindDlgProc);
			else
				SetForegroundWindow(g_hFindWnd);
			break;

		}
		break;
	case WM_CLOSE:
		breakloop = 1;
		while (breakloop)
			SwitchToThread();
		packet_allow();
		packet_term();
		if (g_clntSock != INVALID_SOCKET)
		{
			closesocket(g_clntSock);
			g_clntSock = INVALID_SOCKET;
		}
		WSACleanup();
		UnregisterHotKey(g_hWnd, 2);
		UnregisterHotKey(g_hWnd, 10);
		UnregisterHotKey(g_hWnd, 11);
		KillTimer(g_hWnd, 1);
		KillTimer(g_hWnd, 2);
		KillTimer(g_hWnd, 3);
		PostQuitMessage(0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

VOID CALLBACK PacketTimer(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	VIRTUALIZER_TIGER_BLACK_START
	{
		DlgPacketProcSub(hwnd, 0, 0);
	}
		VIRTUALIZER_TIGER_BLACK_END
}

BOOL CALLBACK DlgPacketProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	unsigned int i;

	switch (uMsg)
	{
	case WM_CLOSE:
		VIRTUALIZER_TIGER_BLACK_START
		{
			PacketWnd = NULL;
			DestroyWindow(hwnd);
		}
			VIRTUALIZER_TIGER_BLACK_END
			break;

	case WM_INITDIALOG:
		VIRTUALIZER_TIGER_BLACK_START
		{
			PacketWnd = hwnd;
			SetDlgItemInt(hwnd, IDC_EDIT2, 1000, 0);
		}
			VIRTUALIZER_TIGER_BLACK_END
			break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			VIRTUALIZER_TIGER_BLACK_START
			{
				DlgPacketProcSub(hwnd, 0, 0);
			}
				VIRTUALIZER_TIGER_BLACK_END
				break;
		case IDC_CHECK1:
			if (operating)
			{
				MessageBox(GetDesktopWindow(), "인식과 패킷 스팸은 같이 쓸 수 없습니다", "알림", MB_ICONINFORMATION);
				return 0;
			}
			VIRTUALIZER_TIGER_BLACK_START
			{
				if (IsDlgButtonChecked(hwnd, IDC_CHECK1) != BST_UNCHECKED)
				{
					if ((i = (unsigned int)GetDlgItemInt(hwnd, IDC_EDIT2, NULL, 0)) == 0)
						i = 1;
					SetTimer(hwnd, 12, i, PacketTimer);
				}
				else
					KillTimer(hwnd, 12);
			}
				VIRTUALIZER_TIGER_BLACK_END
				break;

		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

