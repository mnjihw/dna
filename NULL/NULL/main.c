#include "common.h"

char version[] = "1.51";

#define INTERNAL_IP "192.168.219.100"
#define EXTERNAL_IP "182.211.75.197"

SOCKET g_clntSock = INVALID_SOCKET;
char packet_data[4096];
int packet_size;


HANDLE hPcapThread;
unsigned int userid, miniroomCount, target_userid, breakloop, telpo_userid;
unsigned int USER_MINIROOM_BALLON, EMPLOYEE_LEAVE, SERVERINFO, USERINFO, SHOP_INFO, NOTE_INFO;
unsigned int operating, 해상도, 갈바_sel, adapter_sel, only_response, minimize_sel, shop_sel, response_sel, stop, wari_state, startup, telpo_sel, fullmode_sel, telpo_direction;
HWND g_hWnd, g_hMP, g_hList1, g_hList2, g_hButton4, g_hSettingWnd, g_hFindWnd, g_hDllWnd;
static void *engine;
static unsigned int lan_flag;
char username[128], title[51], INIPath[MAX_PATH];
extern aes256_context ctx;
image_t *상점, *상점나가기, *취소, *상점열기, *상점닫기, *확인, *캐럿;
employeeinfo_t employee_list[40];

void ListBox_Modify(HWND hwnd, int sel, char *str)
{
	SendMessage(hwnd, LB_DELETESTRING, sel, 0);
	SendMessage(hwnd, LB_INSERTSTRING, sel, (LPARAM)str);
}

 const unsigned char *findvalue(const unsigned char *data, const unsigned int size, unsigned int value)
{
	for (unsigned int i = size; i != 0; --i)
		if (*(unsigned int*) &data[i] == value)
			return &data[i];
	return NULL;  
}


 __declspec(naked) void nop1()
 {
	 VIRTUALIZER_SHARK_WHITE_START
	 {
		 FILL_NOP(NOP_SIZE);
	 }
	 VIRTUALIZER_SHARK_WHITE_END
 }


void packet_handle(unsigned char *data, unsigned int size)
{	
	char buf[128];
	static unsigned int employeeCount;
	int i, j;
	WINDOWINFO wi;

	if (*(unsigned short*) data == USER_MINIROOM_BALLON)
	{
		if (data[6] != 0 || !operating || response_sel == 2 || telpo_sel != 2 && target_userid)
			return;
		if (telpo_sel)
		{
			if (telpo_sel == 1 && *(unsigned int*) &data[2] != telpo_userid)
				return;
			else if (telpo_sel == 2 && *(unsigned int*) &data[2] != userid && *(unsigned int*) &data[2] != telpo_userid)
				return;
		}
		else if (갈바_sel)
			if (*(unsigned int*) &data[2] != userid || userid == 0) //갈바_sel이 0이면 OFF 1이면 ON
				return;
		if (telpo_sel == 0)
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		else if (telpo_sel == 2 && *(unsigned int*) &data[2] == userid)
		{
			if (shop_sel)
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		}
		else
		{
			BlockInput(1);
			wi.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(g_hMP, &wi);

			if (wi.dwStyle & WS_MINIMIZE)
			{
				PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
				Sleep(300);
			}
			if (GetForegroundWindow() != g_hMP)
			{
				SetForegroundWindow(g_hMP);
				Sleep(100);
			}
			switch (telpo_direction)
			{
			case 0:
				keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
				break;
			case 1:
				keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
				break;
			case 2:
				keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), 0, 0);
				break;
			case 3:
				keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
				break;
			}
			Sleep(50);
			keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), 0, 0);
			keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(50);
			switch (telpo_direction)
			{
			case 0:
				keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				break;
			case 1:
				keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				break;
			case 2:
				keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				break;
			case 3:
				keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				break;
			}
			Sleep(500);
			if (shop_sel)
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			BlockInput(0);
			
		}
		wsprintf(buf, "일상 반응횟수: %u회", ++miniroomCount);
		ListBox_Modify(g_hList2, 0, buf);

		if (!only_response)
		{
			wi.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(g_hMP, &wi);

			if (!telpo_sel && wi.dwStyle & WS_MINIMIZE)
			{
				PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
				Sleep(300);
			}
			if (GetForegroundWindow() != g_hMP)
			{
				SetForegroundWindow(g_hMP);
				Sleep(100);
			}
			Sleep(200);
			if (fullmode_sel)
			{
				i = SendMessage(g_hDllWnd, WM_USER + 7, 0, 0);
				if (i == 1)
				{
					operating = 0;
					PlaySound(MAKEINTRESOURCE(IDR_WAVE2), NULL, SND_ASYNC | SND_RESOURCE);
					상점개설(shop_sel);
					operating = 1;
					SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
				} 
				else if (i == 0) 
				{
					PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
					PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_ASYNC | SND_RESOURCE);
					operating = 0; 
					Sleep(100);
					if (shop_sel == 0)
						일상재대기();
					else
						고상재대기();
					if (telpo_sel == 1 || telpo_sel == 2 && *(unsigned int*)&data[2] == telpo_userid)
					{
						switch (telpo_direction)
						{
						case 0:
							keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
							break;
						case 1:
							keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
							break;
						case 2:
							keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
							break;
						case 3:
							keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), 0, 0);
							break;
						}
						Sleep(50);
						keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), 0, 0);
						keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						switch (telpo_direction)
						{
						case 0:
							keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							break;
						case 1:
							keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							break;
						case 2:
							keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							break;
						case 3:
							keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							break;
						}
					}
					operating = 1;
				}
			}
			else
				CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE) ImageSearchLoop, (void*)*(unsigned int*)&data[2], 0, 0));
		}
		else
			PlaySound(MAKEINTRESOURCE(IDR_WAVE2), NULL, SND_ASYNC | SND_RESOURCE);
	}				
	else if (*(unsigned short*) data == EMPLOYEE_LEAVE)
	{
		if (!operating || response_sel == 1)
			goto $end;
		if (telpo_sel)
		{
			if (telpo_sel == 1 && *(unsigned int*) &data[2] != telpo_userid)
				goto $end;
			else if (telpo_sel == 2 && *(unsigned int*) &data[2] != target_userid && *(unsigned int*) &data[2] != telpo_userid)
				goto $end;
		}
		else if (target_userid)
			if (target_userid != *(unsigned int*) &data[2])
				goto $end;
		else if (갈바_sel && userid != 0)
			goto $end;
	
		if (telpo_sel == 0)
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		else if (telpo_sel == 2 && *(unsigned int*) &data[2] == target_userid)
		{
			if (shop_sel)
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		}
		else
		{
			BlockInput(1);
			wi.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(g_hMP, &wi);

			if (wi.dwStyle & WS_MINIMIZE)
			{
				PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
				Sleep(300);
			}
			if (GetForegroundWindow() != g_hMP)
			{
				SetForegroundWindow(g_hMP);
				Sleep(100);
			}
			switch (telpo_direction)
			{
			case 0:
				keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
				break;
			case 1:
				keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
				break;
			case 2:
				keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), 0, 0);
				break;
			case 3:
				keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
				break;
			}
			Sleep(50);
			keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), 0, 0);
			keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(50);
			switch (telpo_direction)
			{
			case 0:
				keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				break;
			case 1:
				keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				break;
			case 2:
				keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				break;
			case 3:
				keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				break;
			}
			Sleep(500);
			if (shop_sel)
				PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			BlockInput(0);

		}
		wsprintf(buf, "고상 반응횟수: %u회", ++employeeCount);
		ListBox_Modify(g_hList2, 1, buf);

		if (!only_response)
		{
			wi.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(g_hMP, &wi);

			if (!telpo_sel && wi.dwStyle & WS_MINIMIZE)
			{
				PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
				Sleep(300);
			}
			if (GetForegroundWindow() != g_hMP)
			{
				SetForegroundWindow(g_hMP);
				Sleep(100);
			}
			Sleep(200);
			if (fullmode_sel)
			{
				i = SendMessage(g_hDllWnd, WM_USER + 7, 0, 0);
				if (i == 1)
				{
					operating = 0;
					PlaySound(MAKEINTRESOURCE(IDR_WAVE2), NULL, SND_ASYNC | SND_RESOURCE);
					상점개설(shop_sel);
					operating = 1;
					SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
				}
				else if (i == 0)
				{
					PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
					PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_ASYNC | SND_RESOURCE);
					operating = 0;
					Sleep(100);
					if (shop_sel == 0)
						일상재대기();
					else
						고상재대기();
					if (telpo_sel == 1 || telpo_sel == 2 && *(unsigned int*) &data[2] == telpo_userid)
					{
						switch (telpo_direction)
						{
						case 0:
							keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
							break;
						case 1:
							keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
							break;
						case 2:
							keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
							break;
						case 3:
							keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), 0, 0);
							break;
						}
						Sleep(50);
						keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), 0, 0);
						keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
						Sleep(50);
						switch (telpo_direction)
						{
						case 0:
							keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							break;
						case 1:
							keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							break;
						case 2:
							keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							break;
						case 3:
							keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
							break;
						}
					}
					operating = 1;
				}
			}
			else
				CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE) ImageSearchLoop, (void*)*(unsigned int*)&data[2], 0, 0));
		}
		else
			PlaySound(MAKEINTRESOURCE(IDR_WAVE2), NULL, SND_ASYNC | SND_RESOURCE);
	$end:;
		for (i = 0; i < _countof(employee_list); ++i)
		{
			if (*(unsigned int*) &data[2] == employee_list[i].id)
			{
				memset(&employee_list[i], 0, sizeof(employee_list[i]));
				break;
			}
		}
	}
	else if (*(unsigned short*)data == EMPLOYEE_LEAVE - 1) //고상 정보 패킷
	{
		if (!operating)
		{
			if (employee_list[_countof(employee_list) - 1].id != 0) //끝까지 비어있지 않으면
				memset(&employee_list, 0, sizeof(employee_list)); //전부 초기화
			for (i = 0; i < _countof(employee_list); ++i)
				if (employee_list[i].id == 0) //비어있는 곳에서 break
					break;
			employee_list[i].id = *(unsigned int*) &data[2];
			j = *(unsigned short*) &data[16]; //닉네임 사이즈
			memcpy(employee_list[i].nick, &data[18], j);
			employee_list[i].nick[j] = '\0';
			employee_list[i].shopid = *(unsigned int*) &data[16 + 1 + j + 2];
		}
	}
	/*else if (*(unsigned short*) data == CHARACTER_INFO)
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
				dbg("[test]나타남 %s", character_list[i].nick);
				break;
			}
		}
	}*/
	/*else if (*(unsigned short*) data == CHARACTER_INFO + 1) //캐릭터 사라질 때
	{
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*) &data[2])
			{
				memset(&character_list[i], 0, sizeof(character_list[i]));
				dbg("[test] 사라짐");
				break;
			}
		}
	}*/
	else if (*(unsigned short*) data == SERVERINFO)
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
		ListBox_Modify(g_hList1, 1, buf);
		//memset(&character_list, 0, sizeof(character_list));
		memset(&employee_list, 0, sizeof(employee_list)); //전부 초기화
	}
	else if (*(unsigned short*) data == USERINFO)
	{
		if (GetAsyncKeyState(VK_F3) & 0x8000)
		{
			if (*(unsigned int*) &data[2] == userid)
			{
				MessageBox(g_hWnd, "캐릭터 고유 ID와 지정하려는 텔대기 상대가 같습니다.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				return;
			}
			telpo_userid = *(unsigned int*) &data[2];
			wsprintf(buf, "텔대기 상대: 0x%X", telpo_userid);
			ListBox_Modify(g_hList2, 2, buf);
		}
		else if (!userid)
		{
			if (*(unsigned int*) &data[2] == telpo_userid)
			{
				MessageBox(g_hWnd, "텔대기 상대와 지정하려는 캐릭터 고유 ID가 같습니다.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				return;
			}
			userid = *(unsigned int*) &data[2];
			wsprintf(buf, "캐릭터 고유 ID: 0x%X", userid);
			ListBox_Modify(g_hList1, 0, buf);
		}
	}
	else if (*(unsigned short*) data == SHOP_INFO)
	{
		if (data[2] == 0x14)
		{
			if (data[3] == 6)
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
								break;
							} 
						}
					}
				}
				else if (!operating && GetAsyncKeyState(VK_F3) & 0x8000)
				{
					for (i = 0; i < _countof(employee_list); ++i)
					{
						if (employee_list[i].id == 0)
							continue;
						unsigned char *s;
						if ((s = (unsigned char*) findvalue(data, size, employee_list[i].shopid)))
						{
							for (j = 0; s[j] != 0; --j);
							++j;
							if (s[j - 2] >= 0x4 && s[j - 2] <= 0xC && s[j - 2] == -j)
							{
								wsprintf(buf, "텔대기 상대: %s", employee_list[i].nick);
								ListBox_Modify(g_hList2, 2, buf);
								telpo_userid = employee_list[i].id;
								break;
							}
						}
					}
				}
			}
		}
	}
	else if (*(unsigned short*) data == NOTE_INFO) //쪽지
	{
		if (data[2] == 0x4 && operating) //쪽지 왔으면서 인식 동작중일때
		{
			wi.cbSize = sizeof(WINDOWINFO);
			GetWindowInfo(g_hMP, &wi);

			if (wi.dwStyle & WS_MINIMIZE)
			{
				PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
				Sleep(300);
			}
			SetForegroundWindow(g_hMP);
			Sleep(100);
			switch (해상도)
			{
			case 800:
				MouseClick(403, 329, 1);
				break;
			case 1024:
				MouseClick(523, 415, 1);
				break;
			case 1366:
				MouseClick(675, 415, 1);
				break;
			case 910:
				MouseClick(421, 274, 1);
				break;
			}
			Sleep(100);
			if (minimize_sel)
				PostMessage(g_hMP, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
	}
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
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, srclen);
	buffer = (char*) GlobalLock(clipbuffer);
	strcpy_s(buffer, srclen, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

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

		if (operating)
			wzcrypt_decrypt(7, k, &data[i + 4u], pkt); //character_info에서 닉읽으려면 이렇게 해야함
		else
			wzcrypt_decrypt(j, k, &data[i + 4u], pkt);

		packet_handle(pkt, j);
		k = wzcrypt_nextiv(k);
	}

	s->recv_iv = k;
	return i;
}


__declspec(naked) void nop2()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
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
		{
			if (lan_flag)
				return;
			packet_handler_sub(s, (unsigned char *) pkt_data + j + 14u, i);
		}
	}
}



int 블럭체크(unsigned int xy)
{
	unsigned int i;
	for (i = 0; i < block_count; ++i)
		if (block_xy[i] == xy)
			return 1;
	return 0;
}

void 블럭초기화(void)
{
	block_count = 0;
}

void 블럭추가(unsigned int xy)
{
	if (블럭체크(xy) == 0)
		if (block_count != MAX_BLOCK_XY)
			block_xy[block_count++] = xy;
}

void 블럭삭제(unsigned int xy)
{
	unsigned int i;
	for (i = 0; i < block_count; ++i)
		if (block_xy[i] == xy) 
		{
			memcpy(&block_xy[i], &block_xy[i + 1], sizeof(int)* (block_count - i - 1u));
			break;
		}
}

unsigned int ImageSearch2sub(image_t *from, image_t *target)
{
	unsigned int x, y, z, n;

	n = 0;

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
					if (--z == 0 && n != MAX_SEARCH_XY)
						image_xy[n++] = ((y & 65535) << 16) | (x & 65535);
					if (memcmp(&target->data[target->xs * z], &from->data[from->xs * (y + z) + x], target->xs * 4u))
						break;
				}
		}
	}

	return n;
}




unsigned int Global_ImageSearch2(image_t *image) /* 반환값: 찾은 좌표 개수 */
{
	unsigned int i = 0;
	image_t *screen;
	if ((screen = get_screen(g_hMP)) != NULL)
	{
		i = ImageSearch2sub(screen, image);
		HeapFree(GetProcessHeap(), 0, screen);
	}

	return i;
}

void CALLBACK 와리(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	unsigned int i;
	MSG message;
	WINDOWINFO wi;

	if (!wari_state || only_response)
		return;
	operating = 0;


	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(g_hMP, &wi);

	if (wi.dwStyle & WS_MINIMIZE)
	{
		PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
		Sleep(300);
	}
	SetForegroundWindow(g_hMP);
	Sleep(100);


	MouseMove(30, 30);


	if (fullmode_sel)
	{
		keybd_event(VK_LMENU, MapVirtualKey(VK_LMENU, MAPVK_VK_TO_VSC), 0, 0);
		Sleep(50);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(50);
		keybd_event(VK_LMENU, MapVirtualKey(VK_LMENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		Sleep(3000);
	}



	if ((i = Global_ImageSearch(상점나가기)) != -1)
	{
		if (해상도 == 910)
			MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
		else
			MouseClick(i & 65535, (i >> 16), 1);
		Sleep(100);
	}
	else if ((i = Global_ImageSearch(취소)) != -1)
	{
		if (해상도 == 910)
			MouseClick((unsigned) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
		else
			MouseClick(i & 65535, (i >> 16), 1);
		Sleep(100);
	}
	else
	{
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		while (Global_ImageSearch(확인) == -1)
		{
			if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
			{
				if (message.message == WM_QUIT)
					ExitProcess(0);
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else
			{
				Sleep(50);
				if (stop)
					return;
			}
		}
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);

	}
	Sleep(300);

	while (wari_state)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				ExitProcess(0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			if (stop)
				return;

			if (해상도 == 910)
				MouseMove((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67));
			else
				MouseMove(i & 65535, (i >> 16));

			Sleep(150);
			i = Global_ImageSearch2(상점);
			while (i > 0)
			{
				--i;
				if (블럭체크(image_xy[i]) == 0)
					break;
			}
			//if (i == 0)
				//break;
			i = image_xy[i];


			if (해상도 == 910)
				MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 2);
			else
				MouseClick(i & 65535, (i >> 16), 2);
			while (wari_state)
			{
				if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
						ExitProcess(0);
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				else
				{
					if (stop)
						return;
					if (Global_ImageSearch(상점나가기) != -1)
					{
						i = 7;
						PostMessage(g_hMP, WM_KEYDOWN, VK_ESCAPE, 0);
						Sleep(50);
						break;
					}
					else if (Global_ImageSearch(확인) != -1) //블추됐단소리
					{
						블럭추가(i);
						PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
						MouseMove(30, 30);
						Sleep(150);
						i = Global_ImageSearch2(상점);
						while (i > 0)
						{
							--i;
							if (블럭체크(image_xy[i]) == 0)
								break;
						}
						//if (i == 0)
						//	break;
						i = image_xy[i];
						if (해상도 == 910)
							MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 2);
						else
							MouseClick(i & 65535, (i >> 16), 2);
					}
				}
			}
			if (i == 7)
				break;
		}
	}
	

	Sleep(300);
	
	if (shop_sel == 0)
		일상재대기();
	else
		고상재대기();

	if (fullmode_sel)
	{
		PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
		Sleep(100);
		SetForegroundWindow(g_hMP);
		Sleep(200);
		keybd_event(VK_LMENU, MapVirtualKey(VK_LMENU, MAPVK_VK_TO_VSC), 0, 0);
		Sleep(50);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(50);
		keybd_event(VK_LMENU, MapVirtualKey(VK_LMENU, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
		Sleep(3000);
	}
	
	if (minimize_sel && fullmode_sel)
		PostMessage(g_hMP, WM_SYSCOMMAND, SC_MINIMIZE, 0);

	
	operating = 1;
}

__declspec(naked) void nop3()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


void MouseMove(unsigned int x, unsigned int y)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
}

void MouseClick(unsigned int x, unsigned int y, unsigned int k)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
	Sleep(15);
	for (unsigned int i = 0; i < k; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
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
			FwpmFilterDeleteByKey0(engine, (GUID *) memcpy(&guid, "mbc-cafe-movntq1", 16));
			FwpmFilterDeleteByKey0(engine, (GUID *) memcpy(&guid, "mbc-cafe-movntq2", 16));
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


int FileNameCheck(char *fileName)
{ 
	unsigned int i;
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
		if (GetDIBits(hdc, hbitmap, 0, 0, NULL, (BITMAPINFO *) &bmi, DIB_RGB_COLORS) != 0)
		{
			bmi.bmiHeader.biBitCount = 32; /* 32비트가 아닐경우 이미지 사이즈를 4바이트 정렬 해야됨 */
			if ((image = (image_t *) HeapAlloc(GetProcessHeap(), 0, sizeof(image_t) + ((bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) << 2))) != NULL)
			{
				image->xs = (unsigned int) bmi.bmiHeader.biWidth;
				image->ys = (unsigned int) bmi.bmiHeader.biHeight;
				bmi.bmiHeader.biHeight = -bmi.bmiHeader.biHeight;
				if (GetDIBits(hdc, hbitmap, 0, -bmi.bmiHeader.biHeight, image->data, (BITMAPINFO *) &bmi, DIB_RGB_COLORS) == 0)
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
	int xs, ys;
	HBITMAP hbitmap;
	HDC hdc, hmemdc;
	char buf[128];

	
	if (!IsWindow(hwnd))
		g_hMP = FindWindow("MapleStoryClass", NULL);
	if (!GetClientRect(hwnd, &rect))
	{
		wsprintf(buf, "메이플 찾지 못함! %u", GetLastError());
		MessageBox(g_hWnd, buf, "알림", MB_ICONERROR | MB_SYSTEMMODAL);
		return 0;
	}
	
	xs = rect.right - rect.left;
	ys = rect.bottom - rect.top;
	

	hdc = GetDC(g_hMP);

	if (xs == 800 && ys == 600)
		해상도 = 800;
	else if (xs == 1024 && ys == 768)
		해상도 = 1024;
	else if (xs == 1366 && ys == 768)
		해상도 = 1366;
	else if (xs == 910 && ys == 512)
		해상도 = 910;
	if (해상도 == 910)
		hbitmap = CreateCompatibleBitmap(hdc, 1366, 768);
	else
		hbitmap = CreateCompatibleBitmap(hdc, xs, ys);
	hmemdc = CreateCompatibleDC(hdc);
	SelectObject(hmemdc, hbitmap);
	if (해상도 == 910)
		BitBlt(hmemdc, 0, 0, 1366, 768, hdc, 0, 0, SRCCOPY);
	else
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

__declspec(naked) void nop4()
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
	static char CryptKey [] = "제시카ㅎㅎ";

	VIRTUALIZER_SHARK_WHITE_START
	{
		for (unsigned int i = 0; i != size; ++i)
			((char *) out)[i] = ((char *) in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
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


void ProcessMessage(char *packet)
{
	unsigned char aeskey[32];
	unsigned char buf[65];

	VIRTUALIZER_SHARK_WHITE_START
	{

		if (*packet == 'O')
		{
			sscanf(packet, "%*c %s", buf);
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

	SendData("A NULL %s %s %s", data, username, version);
}



__declspec(naked) void nop5()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
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


void 일상재대기()
{
	MSG message;

	SetForegroundWindow(g_hMP);
	
	Sleep(150);

	switch (해상도)
	{
	case 800:
		MouseClick(773, 36, 1);
		break;
	case 1024:
		MouseClick(998, 36, 1);
		break;
	case 1366:
		MouseClick(1342, 36, 1);
		break;
	case 910:
		MouseClick(891, 24, 1);
		break;
	}
	Sleep(100);
	do
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				ExitProcess(0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Sleep(50);
			switch (해상도)
			{
			case 800:
				MouseClick(652, 65, 2);
				break;
			case 1024:
				MouseClick(879, 62, 2);
				break;
			case 1366:
				MouseClick(1222, 65, 2);
				break;
			case 910:
				MouseClick(813, 43, 2);
				break;
			}
			Sleep(50);
			if (fullmode_sel)
				break;
		}
	}
	while (Global_ImageSearch(취소) == -1 && !stop);
	if (stop)
		return;
	if (fullmode_sel)
		Sleep(500);
	do
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				ExitProcess(0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Clipboard(title);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
			Sleep(50);
			PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
			Sleep(50);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(200);
			if (fullmode_sel)
				break;
			else if (Global_ImageSearch(캐럿) == -1)
				break;
		}
	} while (!stop);
	if (minimize_sel)
		PostMessage(g_hMP, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void 고상재대기()
{
	MSG message;

	SetForegroundWindow(g_hMP);
	
	Sleep(150);
	switch (해상도)
	{
	case 800:
		MouseClick(773, 36, 1);
		break;
	case 1024:
		MouseClick(998, 36, 1);
		break;
	case 1366:
		MouseClick(1342, 36, 1);
		break;
	case 910:
		MouseClick(891, 24, 1);
		break;
	}
	Sleep(100);
	do
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				ExitProcess(0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Sleep(50);
			switch (해상도)
			{
			case 800:
				MouseClick(690, 65, 2);
				break;
			case 1024:
				MouseClick(916, 65, 2);
				break;
			case 1366:
				MouseClick(1256, 67, 2);
				break;
			case 910:
				MouseClick(837, 47, 2);
				break;
			}
			Sleep(50);
			if (fullmode_sel)
				break;
		}
	}
	while (Global_ImageSearch(취소) == -1 && !stop);
	if (stop)
		return;
	if (fullmode_sel)
		Sleep(500);
	
	do
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				ExitProcess(0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Clipboard(title);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
			Sleep(50);
			PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
			Sleep(50);
			keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
			Sleep(200);
			if (fullmode_sel)
				break;
			else if (Global_ImageSearch(캐럿) == -1)
				break;
		}
	} while (!stop);
	Sleep(100);
	if (!telpo_sel)
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);

	if (minimize_sel)
		PostMessage(g_hMP, WM_SYSCOMMAND, SC_MINIMIZE, 0);

}

void 상점개설(int shopSel)
{
	MSG message;
	unsigned int i;
	
	SetForegroundWindow(g_hMP);

	Sleep(100);
	switch (해상도)
	{
	case 800:
		MouseClick(656, 34, 1);
		break;
	case 1024:
		MouseClick(875, 38, 1);
		break;
	case 1366:
		MouseClick(1217, 37, 1);
		break;
	case 910:
		MouseClick(812, 27, 1);
		break;
	}
	Sleep(100);
	if (shopSel == 0)
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
			MouseClick(1226, 65, 1);
			break;
		case 910:
			MouseClick(811, 41, 1);
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
			MouseClick(1253, 63, 1);
			break;
		case 910:
			MouseClick(835, 45, 1);
			break;
		}
	}
	Sleep(100);
	switch (해상도)
	{
	case 800:
		MouseClick(323, 253, 1);
		break;
	case 1024:
		MouseClick(444, 337, 1);
		break;
	case 1366:
		MouseClick(610, 385, 1);
		break;
	case 910:
		MouseClick(398, 233, 1);
		break;
	}
	Sleep(100);
	for (i = 10; i != 0; --i)
		PostMessage(g_hMP, WM_CHAR, '9', 0);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
	Sleep(50);
	for (i = 15; i != 0; --i)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				ExitProcess(0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			switch (해상도)
			{
			case 800:
				MouseClick(321, 121, 2);
				break;
			case 1024:
				MouseClick(433, 202, 2);
				break;
			case 1366:
				MouseClick(600, 200, 2);
				break;
			case 910:
				MouseClick(405, 136, 2);
				break;
			}
			Sleep(100);
			switch (해상도)
			{
			case 800:
				MouseClick(660, 36, 2);
				break;
			case 1024:
				MouseClick(881, 35, 2);
				break;
			case 1366:
				MouseClick(1220, 35, 2);
				break;
			case 910:
				MouseClick(811, 23, 2);
				break;
			}
			Sleep(100);
			if (fullmode_sel)
			{
				if (i <= 13)
					break;
			}
			else if (Global_ImageSearch(상점열기) == -1 || stop)
				break;
		}
	}
	
}


__declspec(naked) void nop6()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

DWORD WINAPI ImageSearchLoop(LPVOID *arg)
{
	while (!stop)
	{
		if (Global_ImageSearch(상점열기) != -1)
		{
			operating = 0;
			PlaySound(MAKEINTRESOURCE(IDR_WAVE2), NULL, SND_ASYNC | SND_RESOURCE);
			상점개설(shop_sel);
			operating = 1;
			SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
			return 0;
		}
		else if (Global_ImageSearch(확인) != -1)
		{
			PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
			PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_ASYNC | SND_RESOURCE);
			operating = 0;
			Sleep(100);
			if (shop_sel == 0)
				일상재대기();
			else
				고상재대기();
			if (telpo_sel == 1 || telpo_sel == 2 && (unsigned int)arg == telpo_userid)
			{
				switch (telpo_direction)
				{
				case 0:
					keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), 0, 0);
					break;
				case 1:
					keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), 0, 0);
					break;
				case 2:
					keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), 0, 0);
					break;
				case 3:
					keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), 0, 0);
					break;
				}
				Sleep(50);
				keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), 0, 0);
				keybd_event(VK_F2, MapVirtualKey(VK_F2, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
				Sleep(50);
				switch (telpo_direction)
				{
				case 0:
					keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					break;
				case 1:
					keybd_event(VK_UP, MapVirtualKey(VK_UP, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					break;
				case 2:
					keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					break;
				case 3:
					keybd_event(VK_LEFT, MapVirtualKey(VK_LEFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
					break;
				}
			}
			operating = 1;
			return 0;
		}
		Sleep(50);
	}
	return 0;
}



void WriteINI()
{
	char buf[128];

	WritePrivateProfileString("SETTING", "제목", title, INIPath);
	wsprintf(buf, "%u", 갈바_sel);
	WritePrivateProfileString("SETTING", "갈바_sel", buf, INIPath);
	wsprintf(buf, "%u", adapter_sel);
	WritePrivateProfileString("SETTING", "adapter_sel", buf, INIPath);
	wsprintf(buf, "%u", only_response);
	WritePrivateProfileString("SETTING", "only_response", buf, INIPath);
	wsprintf(buf, "%u", minimize_sel);
	WritePrivateProfileString("SETTING", "minimize_sel", buf, INIPath);
	wsprintf(buf, "%u", shop_sel);
	WritePrivateProfileString("SETTING", "shop_sel", buf, INIPath);
	wsprintf(buf, "%u", response_sel);
	WritePrivateProfileString("SETTING", "response_sel", buf, INIPath);
	wsprintf(buf, "%u", telpo_sel);
	WritePrivateProfileString("SETTING", "telpo_sel", buf, INIPath);
	wsprintf(buf, "%u", fullmode_sel);
	WritePrivateProfileString("SETTING", "fullmode_sel", buf, INIPath);
	wsprintf(buf, "%u", telpo_direction);
	WritePrivateProfileString("SETTING", "telpo_direction", buf, INIPath);
}
INT_PTR CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int count, i;
	char name[128], desc[128];

	switch (iMessage)
	{
	case WM_INITDIALOG:
		SendDlgItemMessage(hDlg, IDC_EDIT1, EM_LIMITTEXT, 50, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"OFF");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"ON");
		SendDlgItemMessage(hDlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)"ON");
		SendDlgItemMessage(hDlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)"OFF");
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_ADDSTRING, 0, (LPARAM)"일반상점");
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_ADDSTRING, 0, (LPARAM)"고용상인");
		SendDlgItemMessage(hDlg, IDC_COMBO5, CB_ADDSTRING, 0, (LPARAM)"모두 반응");
		SendDlgItemMessage(hDlg, IDC_COMBO5, CB_ADDSTRING, 0, (LPARAM)"일반상점");
		SendDlgItemMessage(hDlg, IDC_COMBO5, CB_ADDSTRING, 0, (LPARAM)"고용상인");
		count = NpfFindAllDevices();
		for (i = 1; i <= count; ++i)
		{
			NpfGetDeviceInfo(i, name, desc);
			SendDlgItemMessage(hDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM) desc);
		}
		ReadINI();
		SetDlgItemText(hDlg, IDC_EDIT1, title);
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, 갈바_sel, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO2, CB_SETCURSEL, adapter_sel, 0);
		CheckDlgButton(hDlg, IDC_CHECK1, only_response);
		CheckDlgButton(hDlg, IDC_CHECK2, minimize_sel);
		CheckDlgButton(hDlg, IDC_CHECK3, telpo_sel);
		CheckDlgButton(hDlg, IDC_CHECK4, fullmode_sel);
		SendDlgItemMessage(hDlg, IDC_COMBO4, CB_SETCURSEL, shop_sel, 0);
		SendDlgItemMessage(hDlg, IDC_COMBO5, CB_SETCURSEL, response_sel, 0);
		switch (telpo_direction)
		{
		case 0:
			CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO1);
			break;
		case 1:
			CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO2);
			break;
		case 2:
			CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO3);
			break;
		case 3:
			CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO4);
			break;
		}
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON:
			packet_allow();
			break;
		case IDC_BUTTON1:
			packet_disallow();
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT1, title, sizeof(title) - 1);
			갈바_sel = SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
			adapter_sel = SendDlgItemMessage(hDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0);
			only_response = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			minimize_sel = IsDlgButtonChecked(hDlg, IDC_CHECK2);
			shop_sel = SendDlgItemMessage(hDlg, IDC_COMBO4, CB_GETCURSEL, 0, 0);
			response_sel = SendDlgItemMessage(hDlg, IDC_COMBO5, CB_GETCURSEL, 0, 0);
			telpo_sel = IsDlgButtonChecked(hDlg, IDC_CHECK3);
			if ((fullmode_sel = IsDlgButtonChecked(hDlg, IDC_CHECK4)))
				MessageBox(0, "전체화면 대기시 와리가 안 될 수 있음" , "알림", 0);
			
			if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
				telpo_direction = 0;
			else if (IsDlgButtonChecked(hDlg, IDC_RADIO2) == BST_CHECKED)
				telpo_direction = 1;
			else if (IsDlgButtonChecked(hDlg, IDC_RADIO3) == BST_CHECKED)
				telpo_direction = 2;
			else if (IsDlgButtonChecked(hDlg, IDC_RADIO4) == BST_CHECKED)
				telpo_direction = 3;
			if (telpo_sel && only_response)
			{
				MessageBox(g_hWnd, "텔대기는 반응만 하기와 동시에 할 수 없습니다", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				return 0;
			}

			g_hDllWnd = FindWindow("#32770", "ㅎㅇ리듬메이플런처");
			//g_hDllWnd = FindWindow("MapleStoryClass", NULL);
			if (!g_hDllWnd || !IsWindow(g_hDllWnd) && fullmode_sel)
			{
				MessageBox(g_hWnd, "전체화면 대기는 메이플을 리듬 메이플 런처로 켰을 때만 사용 가능함", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				fullmode_sel = 0;
			}
			

			WriteINI();

			NpfGetDeviceInfo(adapter_sel + 1, name, desc);
			NpfSetDevice(name);
			NpfStart();
			g_hSettingWnd = 0;
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			g_hSettingWnd = 0;
			EndDialog(hDlg, 0);
			break;
		}
		break;
	//case WM_CLOSE:
		//g_hSettingWnd = 0;
		//EndDialog(hDlg, 0);
		//return 0;
	}
	return 0;
}


void ReadINI()
{	
	GetModuleFileName(NULL, INIPath, sizeof(INIPath));

	memcpy(&INIPath[strlen(INIPath) - 3], "ini", 3);

	if (!(SERVERINFO = GetPrivateProfileInt("PACKET", "SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SERVERINFO", "", INIPath);
	if (!(USERINFO = GetPrivateProfileInt("PACKET", "USERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "USERINFO", "", INIPath);
	if (!(USER_MINIROOM_BALLON = GetPrivateProfileInt("PACKET", "USER_MINIROOM_BALLON", 0, INIPath)))
		WritePrivateProfileString("PACKET", "USER_MINIROOM_BALLON", "", INIPath);
	if (!(EMPLOYEE_LEAVE = GetPrivateProfileInt("PACKET", "EMPLOYEE_LEAVE", 0, INIPath)))
		WritePrivateProfileString("PACKET", "EMPLOYEE_LEAVE", "", INIPath);
	if (!(SHOP_INFO = GetPrivateProfileInt("PACKET", "SHOP_INFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SHOP_INFO", "", INIPath);
	if (!(NOTE_INFO = GetPrivateProfileInt("PACKET", "NOTE_INFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "NOTE_INFO", "", INIPath);
	
	if (GetPrivateProfileString("SETTING", "제목", "", title, sizeof(title), INIPath) == 0)
		WritePrivateProfileString("SETTING", "제목", "NULL", INIPath);
	if (!(갈바_sel = GetPrivateProfileInt("SETTING", "갈바_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "갈바_sel", "0", INIPath);
	if (!(adapter_sel = GetPrivateProfileInt("SETTING", "adapter_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "adapter_sel", "0", INIPath);
	if (!(only_response = GetPrivateProfileInt("SETTING", "only_response", 0, INIPath)))
		WritePrivateProfileString("SETTING", "only_response", "0", INIPath);
	if (!(minimize_sel = GetPrivateProfileInt("SETTING", "minimize_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "minimize_sel", "0", INIPath);
	if (!(shop_sel = GetPrivateProfileInt("SETTING", "shop_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "shop_sel", "0", INIPath);
	if (!(response_sel = GetPrivateProfileInt("SETTING", "response_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "response_sel", "0", INIPath);
	if (!(telpo_sel = GetPrivateProfileInt("SETTING", "telpo_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "telpo_sel", "0", INIPath);
	if (!(fullmode_sel = GetPrivateProfileInt("SETTING", "fullmode_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "fullmode_sel", "0", INIPath);
	if (!(telpo_direction = GetPrivateProfileInt("SETTING", "telpo_direction", 0, INIPath)))
		WritePrivateProfileString("SETTING", "telpo_direction", "0", INIPath);
}

VOID CALLBACK DoEmptyWorkingSet(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	HANDLE process;
	PROCESS_MEMORY_COUNTERS stat[2];
	DWORD pid;

	GetWindowThreadProcessId(g_hMP, &pid);
	if ((process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid)) != NULL)
	{
		memset(stat, 0, sizeof(stat));
		GetProcessMemoryInfo(process, &stat[0], sizeof(stat[0]));
		SetProcessWorkingSetSize(process, -1, -1);
		GetProcessMemoryInfo(process, &stat[1], sizeof(stat[1]));
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


INT_PTR CALLBACK FindDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
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
				MessageBox(g_hWnd, "닉네임을 입력해주세요.", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
				return 0;
			}
			GetDlgItemText(hDlg, IDC_EDIT1, data, sizeof(data));
			for (int i = 0; i < _countof(employee_list); ++i)
			{
				if (lstrcmpi(data, employee_list[i].nick) == 0)
				{
					if (GetAsyncKeyState(VK_F3) & 0x8000)
					{
						wsprintf(buf, "텔대기 상대: %s", employee_list[i].nick);
						ListBox_Modify(g_hList2, 2, buf);
						telpo_userid = employee_list[i].id;
					}
					else
					{ 
						wsprintf(buf, "지정 고상: %s", employee_list[i].nick);
						ListBox_Modify(g_hList1, 2, buf);
						target_userid = employee_list[i].id;
					}
					break; 
				}
				if (i == 39)
				{
					MessageBox(g_hWnd, "해당 고용상인을 찾지 못했습니다.", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
					return 0;
				}
			}
			g_hFindWnd = 0;
			EndDialog(hDlg, 0);
			break;
		case IDCANCEL:
			g_hFindWnd = 0;
			EndDialog(hDlg, 0);
			break;
		}
		break;
	//case WM_CLOSE:
	//	g_hFindWnd = 0;
	//	EndDialog(hDlg, 0);
	//	break;
	}
	return 0;
}

DWORD WINAPI ReconnectThread(LPVOID arg)
{
	unsigned int i;

	
	while (1)
	{
		if (ConnectToServer(EXTERNAL_IP, 1818) || ConnectToServer(INTERNAL_IP, 1818))
			break;
		for (i = 0; i < 5; ++i)
			Sleep(1000);
	}
	auth();
	

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
	WINDOWINFO wi;
	int i;
	char name[128], desc[128];

	switch (iMessage)
	{
	case WM_SOCKET:
		return ProcessSocketMessage(hDlg, wParam, lParam);
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		VIRTUALIZER_SHARK_WHITE_START
		{
			if (!ConnectToServer(EXTERNAL_IP, 1818))
			{
				if (!ConnectToServer(INTERNAL_IP, 1818))
				{
					MessageBox(GetDesktopWindow(), "서버 닫힘!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
					KillProcess();
				}
			}
			auth();
		}
		VIRTUALIZER_SHARK_WHITE_END
		g_hMP = FindWindow("MapleStoryClass", NULL);
		g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
		g_hList2 = GetDlgItem(hDlg, IDC_LIST2);
		g_hButton4 = GetDlgItem(hDlg, IDC_BUTTON4);
		SystemParametersInfo(SPI_SETKEYBOARDDELAY, 0, 0, SPIF_UPDATEINIFILE);
		SystemParametersInfo(SPI_SETKEYBOARDSPEED, 31, 0, SPIF_UPDATEINIFILE);
		packet_init();
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM)"캐릭터 고유 ID: 0");
		SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"현재 채널: NULL");
		SendMessage(g_hList1, LB_INSERTSTRING, 2, (LPARAM)"지정 고상: NULL");
		SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)"일상 반응횟수: 0회");
		SendMessage(g_hList2, LB_INSERTSTRING, 1, (LPARAM)"고상 반응횟수: 0회");
		SendMessage(g_hList2, LB_INSERTSTRING, 2, (LPARAM)"텔대기 상대: NULL");
		상점 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1)));
		상점나가기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2)));
		상점닫기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3)));
		상점열기 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP4)));
		취소 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP5)));
		확인 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP6)));
		캐럿 = get_imagesub(LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP7)));
		RegisterHotKey(g_hWnd, 2, MOD_ALT | MOD_NOREPEAT, VK_F2);
		RegisterHotKey(g_hWnd, 10, MOD_NOREPEAT, VK_F10); 
		RegisterHotKey(g_hWnd, 11, MOD_NOREPEAT, VK_F11);
		ReadINI();
		g_hDllWnd = FindWindow("#32770", "ㅎㅇ리듬메이플런처");
		if (!g_hDllWnd || !IsWindow(g_hDllWnd) && fullmode_sel)
		{
			MessageBox(g_hWnd, "전체화면 대기는 메이플을 리듬 메이플 런처로 켰을 때만 사용 가능함", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			WritePrivateProfileString("SETTING", "fullmode_sel", "0", INIPath);
			fullmode_sel = 0;
		}
		SetTimer(g_hWnd, 2, 20 * 60 * 1000, DoEmptyWorkingSet);
		SetTimer(g_hWnd, 3, 5 * 60 * 1000, FillNOP);
		NpfFindAllDevices();
		NpfGetDeviceInfo(adapter_sel + 1, name, desc);
		NpfSetDevice(name);
		if (NpfStart())
			hPcapThread = CreateThread(0, 0, NpfLoop, 0, 0, 0);
		else
		{
			MessageBox(g_hWnd, "NpfStart() error!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			ExitProcess(0);

		}
		
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1: //초기화
			if (GetAsyncKeyState(VK_MENU) & 0x8000) //ALT누른상태로 버튼누르면 텔대기 상대만 초기화
			{
				telpo_userid = 0;
				ListBox_Modify(g_hList2, 2, "텔대기 상대: NULL");
			}
			else
			{
				userid = 0;
				ListBox_Modify(g_hList1, 0, "캐릭터 고유 ID: 0");
			}
			break;
		case IDC_BUTTON2: //사용법
			MessageBox(g_hWnd, "※핫키 설명※\nF1: 반응할 고상 지정\nF2: 텔포 스킬 두는 키\nALT + F2: 지정한 고상 초기화\nF3: 텔대기 관련 핫키\nF10: 인식 시작\nF11: 인식 정지", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			MessageBox(g_hWnd, "※텔대기 설명※\n텔대기 체크박스를 한 번 체크해서 v표시일 때는 텔대기만 하고\n한 번 더 체크해서 ■표시일 때는 텔대기랑 일반 대기를 동시에 함", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			MessageBox(g_hWnd, "F3을 누른 상태로 캐릭 개인정보를 보거나 대기할 고상에 들어가거나 고상 직접 지정을 하면 텔대기 상대가 잡히고\nALT를 누른 상태로 캐릭터 고유 ID 초기화 버튼을 누르면 텔대기 상대가 초기화됨", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			MessageBox(g_hWnd, "일상이 닫히면 반응하지 않는 경우\n1. 인식 작동 중이 아닐 때\n2. 반응상점이 고용상인일 때\n3. 텔대기가 두 번 체크돼 있지 않으면서 지정 고상이 NULL이 아닐 때\n4. 텔대기가 한 번 체크돼 있는데 닫힌 상점이 내가 텔대기하는 상점이 아닌 경우\n5. 텔대기가 두 번 체크돼 있는데 내가 대기, 텔대기하는 일상이 아닌 경우\n6. 텔대기를 안하면서 갈바방어가 ON일 때 캐릭터 고유 ID가 0이거나 내가 따놓은 ID와 일상 닫힐 때 오는 ID가 다른 경우", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			MessageBox(g_hWnd, "고상이 닫히면 반응하지 않는 경우\n1. 인식 작동 중이 아닐 때\n2. 반응상점이 일반상점일 때\n3. 텔대기가 한 번 체크돼 있으면서 닫힌 고상이 내가 텔대기하는 고상이 아닐 때\n4. 텔대기가 두 번 체크돼 있으면서 닫힌 고상이 내가 대기, 텔대기하는 고상이 아닐 때\n5. 텔대기를 안하면서 지정 고상이 NULL이 아니면서 닫힌 고상이 내가 지정한 고상이 아닐 때\n6. 텔대기를 안하면서 지정 고상이 NULL이면서 갈바방어가 ON이면서 고유 ID가 0이 아닌 경우", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			MessageBox(g_hWnd, "※캐릭터 고유 ID 따는 법※\n대기탈 일상에 들어가서 판매자정보 버튼을 누르거나\n대기탈 일상 주인 캐릭을 우클릭하고 개인정보 버튼을 누르면 됨", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			MessageBox(g_hWnd, "※반응할 고상 지정하는 법※\nF1을 누른상태로 반응할 고상에 들어가서 F1을 뗀다", "알림", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			break;
		case IDC_BUTTON3: //설정
			if (!g_hSettingWnd)
				g_hSettingWnd = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			else
				SetForegroundWindow(g_hSettingWnd);
			break;
		case IDC_BUTTON4: //인식 시작
			if (startup == 0)
			{
				MessageBox(g_hWnd, "캐시샵을 갔다 와서 현재 채널이\n제대로 표시된 후 대기를 시작해주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				return 0;
			}
			g_hMP = FindWindow("MapleStoryClass", NULL);
			g_hDllWnd = FindWindow("#32770", "ㅎㅇ리듬메이플런처");
			if (g_hMP == NULL)
			{
				MessageBox(g_hWnd, "메이플 찾지 못함!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
				return 0;
			}
			HeapFree(GetProcessHeap(), 0, get_screen(g_hMP));
			SetForegroundWindow(g_hMP);
			
			operating = !operating;
			if (operating)
			{
				블럭초기화();
				SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
				SetThreadPriority(hPcapThread, THREAD_PRIORITY_HIGHEST);
				SetWindowText(g_hButton4, "인식 정지[F11]");
				stop = 0;
				if (shop_sel == 0)
					일상재대기();
				else
					고상재대기();

				wari_state = 1;
				SetTimer(g_hWnd, 1, 39 * 60 * 1000, 와리);
				

			}
			else
			{
				SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				SetThreadPriority(hPcapThread, THREAD_PRIORITY_NORMAL);
				SetWindowText(g_hButton4, "인식 시작[F10]");
				stop = 1;

				wi.cbSize = sizeof(WINDOWINFO);
				GetWindowInfo(g_hMP, &wi);
				if (wi.dwStyle & WS_MINIMIZE)
				{
					PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, 0);
					Sleep(300);
				}
				SetForegroundWindow(g_hMP);
				Sleep(150);
				
				
				if (!fullmode_sel)
				{
					i = Global_ImageSearch(취소);
					if (i != -1 && 해상도 == 910)
						MouseClick((unsigned int) ((i & 65535) * 0.67), (unsigned int) ((i >> 16) * 0.67), 1);
					else if (i != -1)
						MouseClick(i & 65535, (i >> 16), 1);
					else
					{
						i = Global_ImageSearch(확인);
						if (i != -1)
						{
							PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
							Sleep(100);
							PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
						}
					}
				}
				else
				{
					switch (해상도)
					{
					case 800:
						MouseClick(489, 363, 1);
						break;
					case 1024:
						MouseClick(595, 450, 1);
						break;
					case 1366:
						MouseClick(775, 445, 1);
						break;
					case 910:
						MouseClick(517, 297, 1);
						break;
					}
					Sleep(50);
				}
				wari_state = 0;
				KillTimer(g_hWnd, 1);
				
			}
			break;
		case IDC_BUTTON5:
			if (!g_hFindWnd)
				g_hFindWnd = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG3), NULL, FindDlgProc);
			else
				SetForegroundWindow(g_hFindWnd);
			break;

		}
		break;
	case WM_HOTKEY:
		if (wParam == 2)
		{
			ListBox_Modify(g_hList1, 2, "지정 고상: NULL");
			target_userid = 0;
			return 0;
		}
		else if (wParam == 10)
		{
			if (operating)
				return 0;
			stop = 0;
			wari_state = 1;
		}
		else if (wParam == 11)
		{
			operating = 1;
			stop = 1;
			wari_state = 0;
		}
		SendMessage(g_hWnd, WM_COMMAND, IDC_BUTTON4, 0);
		break;
	case WM_CLOSE:
		operating = 0;
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
		KillTimer(hDlg, 2);
		KillTimer(hDlg, 3);
		WSACleanup();
		DeleteObject(상점);
		DeleteObject(상점나가기);
		DeleteObject(상점닫기);
		DeleteObject(상점열기);
		DeleteObject(취소);
		DeleteObject(확인);
		DeleteObject(캐럿);
		UnregisterHotKey(g_hWnd, 2);
		UnregisterHotKey(g_hWnd, 10);
		UnregisterHotKey(g_hWnd, 11);
		PostQuitMessage(0);
		return 0;
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


__declspec(naked) void nop8()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}


LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
	TerminateProcess(GetCurrentProcess(), 0);
	return EXCEPTION_EXECUTE_HANDLER;
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

	for (count = 0; count < len; count++)
	{
		char buf[5] = { '0', 'x', pos[0], pos[1], 0 };
		data[count] = (unsigned char)strtol(buf, &endptr, 0);
		pos += 2 * sizeof(char);

		if (endptr[0] != '\0'); //return -1(non-hexadecimal chararcter encountered)

	}

	return 0;
}

#if 0
INT_PTR CALLBACK BirthdayDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		//PlaySound(MAKEINTRESOURCE(IDR_WAVE3), NULL, SND_ASYNC | SND_RESOURCE);
		SetWindowPos(hDlg, HWND_TOPMOST, (GetSystemMetrics(SM_CXSCREEN) - 500) / 2, (GetSystemMetrics(SM_CYSCREEN) - 516) / 2, 500, 516, 0);
		SetWindowPos(GetDlgItem(hDlg, IDC_STATIC1), HWND_TOP, 0, 0, 500, 500, 0);
		AnimateWindow(hDlg, 500, AW_ACTIVATE | AW_BLEND);
		Sleep(1000);
		AnimateWindow(hDlg, 500, AW_HIDE | AW_BLEND);
		MessageBox(GetDesktopWindow(), "2월 10일은 리듬님의 생일입니다. 생일 선물은 010-6276-8388\n(카톡: mnjihw) ☜로~(간단한 기프티콘이라도 주시면 감사)", "헐 대박", MB_ICONWARNING | MB_SYSTEMMODAL);
		EndDialog(hDlg, 0);
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return 0;
}
#endif 




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


void getuuid(char *data)
{
	VIRTUALIZER_TIGER_BLACK_START
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
		VIRTUALIZER_TIGER_BLACK_END
}


__declspec(naked) void nop9()
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
	void(*pt_nop[15])() = { 0, };
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
	SYSTEM_INFO SystemInfo;
	typedef void (WINAPI *PFN_GET_NATIVE_SYSTEM_INFO)(LPSYSTEM_INFO);
	PFN_GET_NATIVE_SYSTEM_INFO pGetNativeSystemInfo;
	//SYSTEMTIME st;
	WSADATA wsaData;
	DWORD username_size = sizeof(username) - 1;
	HWND hwnd;

	VIRTUALIZER_SHARK_WHITE_START
	{
		SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);

		if (IsUserAnAdmin() == FALSE)
		{
			MessageBox(g_hWnd, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

	
		pGetNativeSystemInfo = (PFN_GET_NATIVE_SYSTEM_INFO) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
		if (pGetNativeSystemInfo)
			pGetNativeSystemInfo(&SystemInfo);
		else
			GetNativeSystemInfo(&SystemInfo);
		if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		{
			MessageBox(g_hWnd, "32비트 미지원", "메세지박스", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

		if (!FileNameCheck("NULL.exe"))
			KillProcess();

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			MessageBox(g_hWnd, "WSAStartup error!", "알림", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}

		

		if((hwnd = FindWindow("#32770", "NULL")))
		{
			SetForegroundWindow(hwnd);
			KillProcess();
		}

		FillNOP(0, 0, 0, 0);

		/*GetLocalTime(&st);
		if (st.wMonth == 2 && st.wDay == 10)
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG4), 0, BirthdayDlgProc);*/


		init_rand(GetCurrentProcessId() ^ 0xBADF00Du);

		GetUserName(username, &username_size);

		CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
		MessageLoop();
		
	}
	VIRTUALIZER_SHARK_WHITE_END

	

	return 0;
}


__declspec(naked) void nop10()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}
