#include "common.h"

HWND g_hWnd, g_hTab, g_hList1, g_hList2, g_hList3, g_hEdit1, g_hEdit2;
SOCKET g_servSock = INVALID_SOCKET;

unsigned char lower_ascii[0x100];
unsigned int g_ClientCount, today;
Client g_Client[256];
char aeskey[128];
char fakekey[] = "\x63\x01\x02\x03\xB2\xBA\xCA\xFF\xCA\x13\x12\x1C\x2C\xAA\xBB\xCC\x6B\xAB\xAA\xA2\x5A\xB3\xB2\xCC\xBA\xFB\xCA\xCB\x0F\xA5\xAF\xCA";

unsigned int selcount;
FILE *log_fp;





void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey[] = "제시카ㅎㅎ";

	for (int i = 0; i != size; ++i)
		((char *)out)[i] = ((char *)in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
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


Client* FindClient(SOCKET s)
{
	unsigned int i;

	for (i = 0; i < g_ClientCount; ++i)
		if (g_Client[i].s == s)
			return &g_Client[i];

	return NULL;
}



bool SocketClosed(SOCKET s)
{
	char buf[256];
	SYSTEMTIME st;
	unsigned int i;

	GetLocalTime(&st);

	
	for (i = 0; i < g_ClientCount; ++i)
	{
		if (g_Client[i].s == s)
		{
			if (strcmp(g_Client[i].name, "") != 0 || strcmp(g_Client[i].username, "") != 0 || strcmp(g_Client[i].exename, "") != 0)
			{
				wsprintf(buf, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s) 님이 접속 종료하셨습니다.(%s)", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour > 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, g_Client[i].name, g_Client[i].username, g_Client[i].ip, g_Client[i].exename);
				fprintf(log_fp, "%s\n", buf);
				if (SendMessage(g_hList2, LB_GETTOPINDEX, 0, 0) + selcount == SendMessage(g_hList2, LB_GETCOUNT, 0, 0))
				{
					SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);
					SendMessage(g_hList2, WM_VSCROLL, SB_BOTTOM, 0);
				}
				else
					SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);
			}
			memcpy(&g_Client[i], &g_Client[i + 1], (g_ClientCount - i - 1) * sizeof(Client));
			--g_ClientCount;
			break;
		}
	}
	shutdown(s, SD_BOTH);
	closesocket(s);
	REFRESH_USERLIST();
	return 0;
}

bool check_version_is_newest(const char *exename, const char *version)
{
	char current_version[8];

	const char INIPath[] = "C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\버전.ini";

	if (GetPrivateProfileString("VERSION", exename, "1.00", current_version, sizeof(current_version), INIPath) == 0)
		WritePrivateProfileString("VERSION", exename, "1.00", INIPath);
	
	if (strcmp(version, current_version) == -1)
		return false;
	else
		return true;

}


int check_auth(param_t *param)
{
	const char path[] = "C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버";
	char buf[256], name[128], data[256];
	unsigned int auth = 0;
	int ret = 0;
	FILE *fp;


	if(check_version_is_newest(param->exename, param->version) == 0)
		return -1; //ret = -1;

	wsprintf(buf, "%s\\%s.txt", path, param->exename);
	fp = fopen(buf, "rt");
	if (fp == NULL)
	{
		dbg("파일 오픈 실패!");
		MessageBox(0, "파일 오픈 실패!", "알림", MB_ICONERROR);
		return 0; //이건 걍 리턴하자 ㅋ
	}
	
	while (fscanf(fp, "%s %s %s", name, data, buf) != EOF) //이름 하드번호 날짜
	{
		if (strcmp(data, param->uuid) == 0)
		{
			auth = 1;
			//strcpy(result, name);
			strcpy(param->result->name, name);
			break;
		}
	}

	fclose(fp);

	if (auth)
	{
		wsprintf(data, "%04u%02u%02u", param->st.wYear, param->st.wMonth, param->st.wDay); //data는 하드번호고 이제 쓸일 없으니 배열 재활용 ㅋ.ㅋ
		if (strcmp(buf, data) == -1) //기간만료란소리
			ret = -2;
		else
			ret = 1;
		strcpy(param->result->time_remaning, buf);
	}

	return ret;
}


void REFRESH_USERLIST()
{
	LVITEM li;
	unsigned int i, count, *arr = 0;

	count = ListView_GetSelectedCount(g_hList1);
	
	if (count != 0)
	{
		arr = malloc(count * sizeof(int));
		ListView_GetSelectedItem(g_hList1, arr);

	}
	
	ListView_DeleteAllItems(g_hList1);
	
	for (i = 0; i < g_ClientCount; ++i)
	{
		li.mask = LVIF_TEXT;
		li.pszText = g_Client[i].name;
		li.cchTextMax = strlen(g_Client[i].name);
		li.iItem = i;
		li.iSubItem = 0;

		ListView_InsertItem(g_hList1, &li);

		ListView_SetItemText(g_hList1, i, 1, g_Client[i].username);
		ListView_SetItemText(g_hList1, i, 2, g_Client[i].ip);
		ListView_SetItemText(g_hList1, i, 3, g_Client[i].server);
		ListView_SetItemText(g_hList1, i, 4, g_Client[i].nick);
		ListView_SetItemText(g_hList1, i, 5, g_Client[i].exename);
	}

	if (count != 0)
	{
		//ListView_SetItemState(g_hList1) 이건 안해도될듯 다지우고 삽입했으니
		for (i = 0; i < count; ++i)
			ListView_SetItemState(g_hList1, arr[i], LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		free(arr);
	}



}



void ProcessMessage(Client *client, char *packet, int packet_size)
{
	char uuid[128];
	char version[8];
	char buf[256], message[256];
	SYSTEMTIME st;
	int ret;
	result_t result;
	param_t param;

	memset(&result, 0, sizeof(result));
	GetLocalTime(&st);
	if (today != st.wDay)
	{
		today = st.wDay;
		//fwrite("\0", 1, 1, log_fp);
		fclose(log_fp);
		wsprintf(buf, "C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\로그\\%04u-%02u-%02u log.txt", st.wYear, st.wMonth, st.wDay);
		log_fp = fopen(buf, "atc");
		if (log_fp == NULL)
		{
			dbg("파일 오픈 실패!");
			MessageBox(0, "파일 오픈 실패", "알림", MB_ICONERROR);
			exit(0);
		}

	}
	switch (*packet)
	{
	case 'A': //auth
		if (sscanf(packet, "%*c %s %s %s %s", client->exename, uuid, client->username, version) == 4)
		{

			//ret = check_auth(client->exename, uuid, client->username, version, st, &result);
			param.exename = client->exename;
			param.uuid = uuid;
			param.username = client->username;
			param.version = version;
			param.st = st;
			param.result = &result;
			ret = check_auth(&param);

			strcpy(client->name, result.name);
			switch(ret)
			{
			case 1:
				SendData(client, "O %s %s", aeskey, result.time_remaning);
				wsprintf(message, "(%s)", client->exename);
				break;
			case 0:
				SendData(client, "X %s", fakekey);
				wsprintf(message, "(%s - 하드인증실패!)", client->exename);
				break;
			case -1:
				SendData(client, "E %s", fakekey);
				wsprintf(message, "(%s - 구버전(%s))", client->exename, version);
				break;
			case -2:
				SendData(client, "N %s", fakekey);
				wsprintf(message, "(%s - 기간 만료)", client->exename);
				break;		
			}
			wsprintf(buf, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s) 님이 접속하셨습니다.%s", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour > 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, client->name, client->username, client->ip, message);
			fprintf(log_fp, "%s\n", buf);
			if (SendMessage(g_hList2, LB_GETTOPINDEX, 0, 0) + selcount == SendMessage(g_hList2, LB_GETCOUNT, 0, 0))
			{
				SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);
				SendMessage(g_hList2, WM_VSCROLL, SB_BOTTOM, 0);
			}
			else
				SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);


		}
		else
			SendData(client, "?"); //MTU 줄였거나 할때 이럼
		REFRESH_USERLIST();
		break;
	case 'F':
		if (sscanf(packet, "%*c %s %s %s %s", client->exename, uuid, client->username, version) == 4)
		{
			if (check_version_is_newest(client->exename, version))
			{
				SendData(client, "O %s", aeskey);
				wsprintf(message, "(%s)", client->exename);
			}
			else
			{
				SendData(client, "E %s", fakekey);
				wsprintf(message, "(%s - 구버전(%s))", client->exename, version);
			}

			wsprintf(buf, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s) 님이 접속하셨습니다.%s", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour > 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, client->username, client->ip, message);
			fprintf(log_fp, "%s\n", buf);
			if (SendMessage(g_hList2, LB_GETTOPINDEX, 0, 0) + selcount == SendMessage(g_hList2, LB_GETCOUNT, 0, 0))
			{
				SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);
				SendMessage(g_hList2, WM_VSCROLL, SB_BOTTOM, 0);
			}
			else 
				SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);

		}
		else
			SendData(client, "?");
		REFRESH_USERLIST();
		break;
	case 'N':
		sscanf(packet, "%*c %s %s", client->server, client->nick);
		wsprintf(buf, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s) : %s - %s", st.wYear, st.wMonth, st.wDay, st.wHour >= 12 ? "오후" : "오전", st.wHour > 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond, client->name, client->username, client->ip, client->server, client->nick);
		fprintf(log_fp, "%s\n", buf);
		if (SendMessage(g_hList2, LB_GETTOPINDEX, 0, 0) + selcount == SendMessage(g_hList2, LB_GETCOUNT, 0, 0))
		{
			SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);
			SendMessage(g_hList2, WM_VSCROLL, SB_BOTTOM, 0);
		}
		else
			SendMessage(g_hList2, LB_ADDSTRING, 0, (LPARAM)buf);
		REFRESH_USERLIST();
		break;

	}
}

bool SocketConnected(SOCKET servSock)
{
	Client *client;
	SOCKET s;
	SOCKADDR_IN clntAdr;
	int clntAdrSz;

	clntAdrSz = sizeof(clntAdr);
	s = accept(servSock, (SOCKADDR *)&clntAdr, &clntAdrSz);
	if (s == INVALID_SOCKET)
		return false;

	if (WSAAsyncSelect(s, g_hWnd, WM_SOCKET, FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		closesocket(s);
		return false;
	}

	client = &g_Client[g_ClientCount++];
	memset(client, 0, sizeof(Client));
	client->s = s;
	strcpy(client->ip, inet_ntoa(clntAdr.sin_addr));
	REFRESH_USERLIST();
	return true;
}

bool MakeListen(int port)
{
	DWORD dwRet;
	SOCKET s;
	SOCKADDR_IN servAdr;
	struct tcp_keepalive keepalive;

	s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	keepalive.onoff = 1;
	keepalive.keepalivetime = 10000;
	keepalive.keepaliveinterval = 200;
	
	WSAIoctl(s, SIO_KEEPALIVE_VALS, &keepalive, sizeof(struct tcp_keepalive), 0, 0, &dwRet, NULL, NULL);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons((u_short)port);

	if (WSAAsyncSelect(s, g_hWnd, WM_SOCKET, FD_ACCEPT) == SOCKET_ERROR)
	{
		MessageBox(0, "WSAAsyncSelect() 에러!", 0, 0);
		closesocket(s);
		return false;
	}

	if (bind(s, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		MessageBox(0, "bind() 에러!", 0, 0);
		closesocket(s);
		return false;
	}

	if (listen(s, SOMAXCONN) == SOCKET_ERROR)
	{
		MessageBox(0, "listen() 에러!", 0, 0);
		closesocket(s);
		return false;
	}

	if (g_servSock != INVALID_SOCKET)
		closesocket(g_servSock);
	g_servSock = s;
	return true;
}

bool ReceiveMessage(SOCKET s)
{
	Client *client;
	int i, j;
	char data[16384];

	if ((client = FindClient(s)) == NULL)
		return false;

	if ((i = recv(s, &client->packet_data[client->packet_size], sizeof(client->packet_data) - client->packet_size, 0)) <= 0)
		return false;

	client->packet_size += i;

	if (client->processing)
		return false;

	client->processing = true;

	for (i = 0; client->packet_size >= i + 4 && client->packet_size >= i + (j = *(int *)&client->packet_data[i]) + 4; i += j + 4) {
		if (j > sizeof(data) - 1)
		{
			RFCrypt(sizeof(data) - 1, &client->packet_data[i + 4], data);
			data[sizeof(data) - 1] = '\0';
		}
		else
		{
			RFCrypt(j, &client->packet_data[i + 4], data);
			data[j] = '\0';
		}
		ProcessMessage(client, data, j);
	}

	if (client->packet_size -= i)
		memcpy(client->packet_data, &client->packet_data[i], client->packet_size);

	client->processing = false;
	return true;

}

void SendData(Client *client, const char *fmt, ...)
{
	int size;
	char data[2048];

	size = wvsprintf(data, fmt, (va_list)((DWORD_PTR)&fmt + sizeof(void *)));
	RFCrypt(size, data, data);

	// 패킷 크기를 먼저 보내고
	send(client->s, (char *)&size, 4, 0);

	// 패킷을 보낸다
	send(client->s, data, size, 0);
}




LRESULT ProcessSocketMessage(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	SOCKET s = (SOCKET)wParam;

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		ReceiveMessage(s);
		break;

	case FD_CLOSE:
		SocketClosed(s);
		break;

	case FD_ACCEPT:
		SocketConnected(s);

		break;
	}
	return 0;
}


unsigned int ListView_GetColumnCount(HWND hwnd)
{
	unsigned int i;
	LVCOLUMN col;

	i = 0;
	while (ListView_GetColumn(hwnd, i++, &col));

	return i - 1;
}


void ListView_GetSelectedItem(HWND hwnd, int *buf)
{
	unsigned int i = 0;

	int pos = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
	
	while (pos != -1)
	{
		buf[i++] = pos;
		pos = ListView_GetNextItem(hwnd, pos, LVNI_SELECTED);
	}
}

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (iMessage)
	{

	case WM_CHAR:
		if (wParam == VK_RETURN)
		{
			SendMessage(GetParent(hWnd), WM_COMMAND, IDC_BUTTON1, 0);
			return 0;			
		}
		break;
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}


LRESULT CALLBACK EditSubclassProc2(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (iMessage)
	{

	case WM_CHAR:
		if (wParam == VK_RETURN)
		{
			SendMessage(GetParent(hWnd), WM_COMMAND, IDC_BUTTON2, 0);
			return 0;
		}
		break;
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}

LRESULT CALLBACK ListSubclassProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	unsigned int i, j, count, column_count, *arr = 0, len = 0, len2 = 0;
	char **data, *buf;
	LVITEM li;

	if(iMessage == WM_CHAR)
	{
		if (wParam == 1)
		{
			ListView_SetItemState(hWnd, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
			for (i = 0; i < (unsigned int)ListView_GetItemCount(hWnd); ++i)
				ListView_SetItemState(hWnd, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			return 0;
		}
		else if (wParam == 3)
		{			
			if((count = ListView_GetSelectedCount(hWnd)) == 0)
				return 0;

			arr = malloc(count * sizeof(int));
			ListView_GetSelectedItem(hWnd, arr);
		
			column_count = ListView_GetColumnCount(hWnd);
			data = malloc(128 * column_count);
			for (i = 0; i < count; ++i)
			{
				for (j = 0; j < column_count; ++j)
				{
					li.iSubItem = j;
					li.cchTextMax = 128;
					li.pszText = (LPSTR)&data[j];
					len += SendMessage(hWnd, LVM_GETITEMTEXT, arr[i], (LPARAM)(LVITEM*)&li);
					if(i < count - 1)
						len += 2; //\r\n 공간 확보
				}
			}
			len += column_count - 1; // 스페이스바 넣을 공간 확보
			buf = malloc(len + 1) ;//널문자

			for (i = 0, len = 0; i < count; ++i)
			{
				for (j = 0; j < column_count; ++j)
				{
					li.iSubItem = j;
					li.cchTextMax = 128;
					li.pszText = (LPSTR)&data[j];
					len2 = SendMessage(hWnd, LVM_GETITEMTEXT, arr[i], (LPARAM)(LVITEM*)&li);
					memcpy(&buf[len], &data[j], len2);
					len += len2;
					buf[len++] = ' ';
				}
				if (i < count - 1)
				{
					memcpy(&buf[len], "\r\n", 2);
					len += 2;
				}
			}
			buf[len] = '\0';
			Clipboard(buf);

			free(data);
			free(arr);
			free(buf);

			return 0;

		}
	}
	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}


LRESULT CALLBACK ListSubclassProc2(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	unsigned int *arr, i, len, len2, count;
	char buf[128], *data;

	switch (iMessage)
	{
	case WM_CHAR:
		if (wParam == 1)
		{
			SendMessage(hWnd, LB_SETSEL, TRUE, -1);
			return 0;
		}
		else if (wParam == 3)
		{
			count = SendMessage(hWnd, LB_GETSELCOUNT, 0, 0);
			if(count == 0 || count == LB_ERR)
				return 0;

			arr = malloc(count * sizeof(int));
			SendMessage(hWnd, LB_GETSELITEMS, count, (LPARAM)arr);
			for (i = 0, len = 0; i < count; ++i)
			{
				len += SendMessage(hWnd, LB_GETTEXT, arr[i], (LPARAM)buf); //arr[0]으로 돼있엇음
				if (i < count - 1)
					len += 2;
			}
			data = malloc(len + 1); //널문자
			
			for (i = 0, len = 0; i < count; ++i)
			{
				len2 = SendMessage(hWnd, LB_GETTEXT, arr[i], (LPARAM)buf);
				memcpy(&data[len], buf, len2);
				len += len2;
				if (i < count - 1)
				{
					memcpy(&data[len], "\r\n", 2);
					len += 2;
				}
			}
			data[len] = '\0';
			Clipboard(data);
			
			free(arr);
			free(data);
			return 0;
		
		}
		
		break;
	}

	return DefSubclassProc(hWnd, iMessage, wParam, lParam);
}



INT_PTR CALLBACK OnInitDialog(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char buf[128];
	SYSTEMTIME st;
	RECT rect;
	TCITEM tie;
	LVCOLUMN col;


	InitCommonControls();
	g_hWnd = hDlg;
	g_hTab = GetDlgItem(hDlg, IDC_TAB1);
	g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
	g_hList2 = GetDlgItem(hDlg, IDC_LIST2);
	g_hList3 = GetDlgItem(hDlg, IDC_LIST3);
	g_hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
	g_hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);

	SetWindowSubclass(g_hList1, ListSubclassProc, 0, 0);
	SetWindowSubclass(g_hList2, ListSubclassProc2, 1, 0);
	SetWindowSubclass(g_hList3, ListSubclassProc2, 2, 0);
	SetWindowSubclass(g_hEdit1, EditSubclassProc, 2, 0);
	SetWindowSubclass(g_hEdit2, EditSubclassProc2, 3, 0);


	GetClientRect(g_hList2, &rect);
	selcount = (int)(rect.bottom - rect.top) / SendMessage(g_hList2, LB_GETITEMHEIGHT, 0, 0);


	ShowWindow(g_hList2, SW_HIDE);

	tie.mask = TCIF_TEXT;
	tie.pszText = "접속자 목록";
	TabCtrl_InsertItem(g_hTab, 0, &tie);
	tie.pszText = "로그";
	TabCtrl_InsertItem(g_hTab, 1, &tie);

	ListView_SetExtendedListViewStyle(g_hList1, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;
	col.cx = 120;
	col.pszText = "이름";
	col.cchTextMax = strlen("이름");
	ListView_InsertColumn(g_hList1, 0, &col);

	col.cx = 100;
	col.pszText = "유저이름";
	col.cchTextMax = strlen("유저이름");
	ListView_InsertColumn(g_hList1, 1, &col);

	col.cx = 100;
	col.pszText = "아이피";
	col.cchTextMax = strlen("아이피");
	ListView_InsertColumn(g_hList1, 2, &col);

	col.cx = 60;
	col.pszText = "서버";
	col.cchTextMax = strlen("서버");
	ListView_InsertColumn(g_hList1, 3, &col);

	col.cx = 100;
	col.pszText = "닉네임";
	col.cchTextMax = strlen("닉네임");
	ListView_InsertColumn(g_hList1, 4, &col);

	col.cx = 80;
	col.pszText = "프로그램";
	col.cchTextMax = strlen("프로그램");
	ListView_InsertColumn(g_hList1, 5, &col);


	if (!MakeListen(1818))
		PostQuitMessage(0);

	GetLocalTime(&st);
	today = st.wDay;
	wsprintf(buf, "C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\로그\\%04u-%02u-%02u log.txt", st.wYear, st.wMonth, st.wDay);
	log_fp = fopen(buf, "atc");
	if (log_fp == NULL)
	{
		dbg("파일 오픈 실패!");
		MessageBox(0, "파일 오픈 실패", "알림", MB_ICONERROR);
		return 0;
	}

	return 0;
}


int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int i;
	char buf[64], buf2[64];
	wchar_t wbuf[64], wbuf2[64];

	i = ((sortinfo_t*)lParamSort)->iSubItem;

	ListView_GetItemText(g_hList1, lParam1, i, buf, sizeof(buf));
	ListView_GetItemText(g_hList1, lParam2, i, buf2, sizeof(buf2));

	wsprintfW(wbuf, L"%S", buf);
	wsprintfW(wbuf2, L"%S", buf2);

	if(i == 0 || i == 1 || i == 3 || i == 4 || i == 6)
	{
		if (StrCmpLogicalW(wbuf, wbuf2) != 0 && wcscmp(wbuf, L"") == 0)
			return 1;
		else if (StrCmpLogicalW(wbuf, wbuf2) != 0 && wcscmp(wbuf2, L"") == 0)
			return -1;
	}


	return ((sortinfo_t*)lParamSort)->ascending ? StrCmpLogicalW(wbuf, wbuf2) : -StrCmpLogicalW(wbuf, wbuf2);
	 
}



_inline void build_lower_table()
{
	if (!lower_ascii[0xFF])
	{
		size_t i;
		for (i = 0; i < 'A'; ++i) lower_ascii[i] = (unsigned char)i;
		for (; i <= 'Z'; ++i) lower_ascii[i] = (unsigned char)i | 0x20;
		for (; i < 0x100; ++i) lower_ascii[i] = (unsigned char)i;
	}
}

_inline size_t has_zero_byte(const size_t n)
{
	const   size_t  finder = (size_t)0x0101010101010101ULL;
	const   size_t  masker = (size_t)0x8080808080808080ULL;
	return (n - finder) & (~n & masker);
}

_inline  size_t  has_some_byte(const unsigned char *s, const size_t cs)
{
	return has_zero_byte((*(size_t*)s | (size_t)0x2020202020202020ULL) ^ cs);
}


_inline char* _strstrcase(const unsigned char *src, const unsigned char *sub)
{
	unsigned char* s = (unsigned char*)src;
	unsigned char* r = (unsigned char*)sub;
	unsigned char* o = s;
	while (*s)
	{
		if (lower_ascii[*s] == lower_ascii[*r])
		{
			s++;
			r++;
			if (!*r) return (char*)o - 1;
		}
		else
		{
			s = o++;
			r = (unsigned char*)sub;
		}
	}
	return NULL;
}

_inline char* strstrcase(const char *src, const char *sub)
{
	build_lower_table();
	if (!((size_t)src | (size_t)sub)) return NULL;
	return _strstrcase((unsigned char*)src, (unsigned char*)sub);
}

_inline  int     strdiff_except_ascii(const unsigned char* s1, const unsigned char* s2)
{
	for (size_t i = 0; s2[i]; )
	{
		if (s1[i] & 0x80)
		{
			if (*(short*)(s1 + i) != *(short*)(s2 + i))   return 1;
			i += 2;
			continue;
		}
		i++;
	}
	return 0;
}

_inline char* _strstrcase_fast(const unsigned char* src, const unsigned char* sub)
{
	build_lower_table();
	if (!((size_t)src | (size_t)sub)) return NULL;

	size_t* s = (size_t*)src;
	unsigned char*  r = (unsigned char*)sub + 1;
	const unsigned char c0 = lower_ascii[sub[0]];
	const unsigned char c0t = sub[0] | 0x20;
	const size_t    c0s =
#ifdef _WIN64
		((size_t)c0t << 56) | ((size_t)c0t << 48) |
		((size_t)c0t << 40) | ((size_t)c0t << 32) |
#endif
		((size_t)c0t << 24) | ((size_t)c0t << 16) |
		((size_t)c0t << 8) | ((size_t)c0t);

	if (!*r)
	{
		while (!has_zero_byte(*(size_t*)s))
		{
			if (has_some_byte((unsigned char*)s, c0t))
			{
				unsigned char* ss = (unsigned char*)s;
				while (lower_ascii[*ss++] != c0);
				return (char*)ss - 1;
			}
			else s++;
		}
	}
	else
	{
		const unsigned char c1t = sub[1] | 0x20;
		const size_t        c1s =
#ifdef _WIN64
			((size_t)c1t << 56) | ((size_t)c1t << 48) |
			((size_t)c1t << 40) | ((size_t)c1t << 32) |
#endif
			((size_t)c1t << 24) | ((size_t)c1t << 16) |
			((size_t)c1t << 8) | ((size_t)c1t);

		while (!has_zero_byte(*(size_t*)s))
		{
			if (has_some_byte((unsigned char*)s, c0s) &&
				has_some_byte((unsigned char*)s + 1, c1s))
			{
				unsigned char* ss = (unsigned char*)s;
				s++;
				while (lower_ascii[*ss++] != c0);
			retry:
				for (int i = 0; lower_ascii[ss[i]] == lower_ascii[r[i]]; ++i)
				{
					if (!ss[i]) return NULL;
					if (!r[i + 1])
					{
						if (!strdiff_except_ascii(ss - 1, sub)) return (char*)ss - 1;
						break;
					}
				}
				do if (lower_ascii[*ss++] == c0) goto retry; while (ss < (unsigned char*)s);
			}
			else s++;
		}
	}
	return strstrcase((unsigned char*)s, sub);
}

char* strstrcase_fast(const char* src, const char* sub)
{
	return  _strstrcase_fast((unsigned char*)src, (unsigned char*)sub);
}




DWORD WINAPI SearchThread(LPVOID arg)
{
	WIN32_FIND_DATA fd;
	HANDLE hFile, hFind;
	char *data, *p, *new_p = 0, *nextline, buf[256], searchstring[256], server[16], nick[13], temp;
	char list[30][128] = { 0, };
	unsigned int i, idx = 0;
	DWORD size, read;
	
	strcpy(searchstring, arg);
	*(char*)arg = '\0';

	SendMessage(g_hList3, LB_RESETCONTENT, 0, 0);
	SetCurrentDirectory("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\로그");

	if((hFind = FindFirstFile("????-??-?? log.txt", &fd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			size = fd.nFileSizeLow;
			data = malloc(size + 1);
			if ((hFile = CreateFile(fd.cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0)) != INVALID_HANDLE_VALUE)
			{
				ReadFile(hFile, data, size, &read, 0);
				data[size] = '\0';
				CloseHandle(hFile);

				p = data;
				while (p = strstrcase_fast(p, searchstring))
				{
					new_p = p + 1;
					while (*--p != '\n' && p != data);
					for (i = 1; p[i] != '\n' && p[i] != EOF; ++i); //p[0]에 \n 있으니 1부터 시작
					nextline = &p[i];
					temp = *nextline;
					*nextline = '\0';
				
					if (strstr(p + 1, "님이 접속"))
						sscanf(p + 1, "%*s %*s %*s %s", buf);
					else
					{
						sscanf(p + 1, "%*s %*s %*s %s : %s - %s", buf, server, nick);
						wsprintf(buf, "%s : %s - %s", buf, server, nick);
					}
					*nextline = temp;
					
					for (i = 0; i < idx; ++i)
						if (strcmp(list[i], buf) == 0)
							break;
					if (i == idx && idx < _countof(list))
					{
						strcpy(list[idx], buf);
						wsprintf(buf, "%s - %s", fd.cFileName, list[idx++]);
						SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM)buf);
					}
					p = new_p;
				}
			
			}
			free(data);
		} while (FindNextFile(hFind, &fd));
		if (new_p == 0)
			SendMessage(g_hList3, LB_ADDSTRING, 0, (LPARAM)"검색된 결과가 없습니다.");
		FindClose(hFind);
	}
	return 0;
}


INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static sortinfo_t si;
	unsigned int count, *arr, i, j;
	char buf[256], ip[16], exename[32];

	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			if ((count = ListView_GetSelectedCount(g_hList1)) == 0)
			{
				MessageBox(0, "상대를 선택해주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}

			GetWindowText(g_hEdit1, buf, sizeof(buf));
			arr = malloc(count * sizeof(int));
			ListView_GetSelectedItem(g_hList1, arr);
			for (i = 0; i < count; ++i)
			{
				ListView_GetItemText(g_hList1, arr[i], 5, exename, sizeof(exename));
				if (strcmp(exename, "foo") == 0)
				{
					ListView_GetItemText(g_hList1, arr[i], 2, ip, sizeof(ip));
					for (j = 0; j < g_ClientCount; ++j)
					{
						if (strcmp(ip, g_Client[j].ip) == 0)
						{
							SendData(&g_Client[j], "M %s", buf);
							break;
						}
					}
				}
			}
			SetWindowText(g_hEdit1, "");
			break;
		case IDC_BUTTON2:
			if (GetWindowText(g_hEdit2, buf, sizeof(buf)) == 0)
			{
				MessageBox(0, "검색할 내용을 입력해주세요", "알림", MB_ICONINFORMATION);
				return 0;
			}
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SearchThread, buf, 0, 0));
			while (*buf != '\0')
				SwitchToThread();
			break;
		}
		break;
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)
		{
			if (((LPNMHDR)lParam)->hwndFrom == g_hList1)
			{
				si.ascending = !si.ascending;
				si.iSubItem = ((LPNMLISTVIEW)lParam)->iSubItem;
				ListView_SortItemsEx(g_hList1, CompareFunc, (LPARAM)&si);
			}
		}
		else if (((LPNMHDR)lParam)->code == TCN_SELCHANGE)
		{
			switch (TabCtrl_GetCurSel(g_hTab))
			{
			case 0:
				ShowWindow(g_hList1, SW_SHOW);
				ShowWindow(g_hList2, SW_HIDE);
				break;
			case 1:
				ShowWindow(g_hList1, SW_HIDE);
				ShowWindow(g_hList2, SW_SHOW);
				break;
			}
		}
		break;
	case WM_CLOSE:
		//fwrite("\0", 1, 1, log_fp);
		fflush(log_fp);
		fclose(log_fp);
		WSACleanup();
		RemoveWindowSubclass(g_hList1, ListSubclassProc, 0);
		RemoveWindowSubclass(g_hList2, ListSubclassProc2, 1);
		RemoveWindowSubclass(g_hList3, ListSubclassProc2, 2);
		RemoveWindowSubclass(g_hEdit1, EditSubclassProc, 2);
		RemoveWindowSubclass(g_hEdit2, EditSubclassProc2, 3);
		PostQuitMessage(0);
		break;
	case WM_INITDIALOG:
		return OnInitDialog(hDlg, iMessage, wParam, lParam);
		break;
	case WM_SOCKET:
		return ProcessSocketMessage(hDlg, iMessage, wParam, lParam);
	}
	return 0;
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
	WSADATA wsaData;
	FILE *fp;
	HWND hwnd;


	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.", "알림", MB_ICONERROR);
		return 0;
	}

	if ((hwnd = FindWindow("#32770", "Rhythm Server")))
	{
		SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		SetForegroundWindow(hwnd);
		return 0;

	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox(0, "WSAStartup() error!", "알림", MB_ICONERROR);
		return 0;
	}

	fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\aeskey.txt", "rt");
	if (fp == NULL)
	{
		MessageBox(0, "aeskey.txt 오픈 실패!", "알림", MB_ICONERROR);
		return 0;
	}
	fscanf(fp, "%s", aeskey);
	fclose(fp);

	CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MessageLoop();

	return 0;
}