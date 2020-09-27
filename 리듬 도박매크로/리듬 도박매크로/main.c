#include "common.h"

char version[] = "1.03";

#define INTERNAL_IP "192.168.219.100"
#define EXTERNAL_IP "182.211.75.197"

SOCKET g_clntSock = INVALID_SOCKET;
char packet_data[4096];
int packet_size;

char INIPath[MAX_PATH];
unsigned int adapter_sel, ���þ�sel;
char ���þ�[128], username[128];
unsigned int operating, startup, testing, target_id, autotype, wincount, isdealer, isbaram, ������sel;
unsigned int SERVERINFO, CHARACTER_APPEAR, CHATTING;
HWND g_hWnd, g_hMP, g_hList1, g_hEdit1, g_hRadio1, g_hRadio2, g_hRadio3, g_hCombo, g_hCheck1, g_hCheck2, g_hCheck3;
extern aes256_context ctx;
const char simbols [] = "(+-*/)xX^ ";


unsigned int count_character(const char *str, const char ch)
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < strlen(str); ++i)
		if (str[i] == ch) 
			++count;
	return count; 
}

unsigned int count_hangul(const char *str, unsigned short value)
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < strlen(str); i += 2)
	{
		if (*(unsigned short*) &str[i] == _byteswap_ushort(value))
			++count;
	}
	return count;
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
				MessageBox(GetDesktopWindow(), "�������� �޾ƿ� AESkey�� ������ �ֽ��ϴ�!", "�˸�", MB_ICONERROR | MB_SYSTEMMODAL);
				KillProcess();
			}
			aes256_init(&ctx, aeskey);
		}
		else if (*packet == 'X')
		{
			MessageBox(GetDesktopWindow(), "�ϵ����� ����!", "�˸�", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == 'N')
		{
			MessageBox(GetDesktopWindow(), "�Ⱓ ����!", "�˸�", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == 'E')
		{
			MessageBox(GetDesktopWindow(), "�ֽ� ������ �ƴմϴ�.\n������ ���� �޾��ּ���.", "�˸�", MB_ICONERROR | MB_SYSTEMMODAL);
			KillProcess();
		}
		else if (*packet == '?')
		{
			MessageBox(GetDesktopWindow(), "?", "�˸�", MB_ICONERROR | MB_SYSTEMMODAL);
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

	SendData("A ���ڸ�ũ�� %s %s %s", data, username, version);

}



void packet_handle(unsigned char *data, unsigned int size)
{
	char buf[128];
	unsigned int i;

	if (*(unsigned short*) data == SERVERINFO)
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
			wsprintf(buf, "���� ä��: %u��", i);
		else
			wsprintf(buf, "���� ä��: %uä��", i);
		SendMessage(g_hList1, LB_DELETESTRING, 0, 0);
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM) buf);
		SendMessage(g_hCombo, CB_RESETCONTENT, 0, 0);
	}
	else if (*(unsigned short*) data == CHARACTER_APPEAR)
	{
		i = *(unsigned short*) &data[7];
		memcpy(buf, &data[9], i);
		buf[i] = '\0';
		SendMessage(g_hCombo, CB_SETITEMDATA, (WPARAM) SendMessage(g_hCombo, CB_ADDSTRING, 0, (LPARAM)buf), (LPARAM) *(unsigned int*) &data[2]);
	}
	else if (*(unsigned short*) data == CHARACTER_APPEAR + 1) //ĳ���� �������
	{
		for (i = 0; i < (unsigned int)SendMessage(g_hCombo, CB_GETCOUNT, 0, 0); ++i)
		{
			if (*(unsigned int*) &data[2] == SendMessage(g_hCombo, CB_GETITEMDATA, i, 0))
			{
				SendMessage(g_hCombo, CB_DELETESTRING, i, 0);
				break;
			}
		}	
	}
	else if (*(unsigned short*) data == CHATTING)
	{
		if (testing)
		{
			i = *(unsigned short*) &data[7];
			memcpy(buf, &data[9], i);
			buf[i] = '\0';
			if (strstr(buf, "�׽�Ʈ"))
			{
				MessageBox(GetDesktopWindow(), "�׽�Ʈ �Ϸ�!���� �۵�", "�˸�", MB_ICONINFORMATION);
				testing = 0;
				SetDlgItemText(g_hWnd, IDC_BUTTON2, "�׽�Ʈ");
				EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON2), TRUE);
			}
		}
		else if (operating)
		{
			i = *(unsigned short*) &data[7];
			memcpy(buf, &data[9], i);
			buf[i] = '\0';
			if (*(unsigned int*) &data[2] == target_id)
			{
				if (count_character(buf, '/') + count_character(buf, '?') == 2)
				{
					for (i = 0; i < strlen(buf); ++i)
					{
						if (buf[i] == '/' || buf[i] == '?')
						{
							if (buf[i + 1] != '/' && buf[i + 1] != '?' && buf[i + 2] != '/' && buf[i + 2] != '?' && buf[i + 3] != '/' && buf[i + 3] != '?')
							{
								text_process(buf);
								break;
							}
							else
							{
								_text_process(buf);
								break;
							}
						}
					}
				}
				else
					_text_process(buf);
			}
		}
	}
}

void sethangul(char *p, unsigned int count, unsigned int sel)
{
	unsigned int i, random;

	if (������sel == 2)
		random = gen_rand() % 2;
	for (i = 0; i < count; ++i)
	{
		switch (sel)
		{
		case 0:
			*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
			break;
		case 1:
			switch (������sel)
			{
			case 0:
				*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			case 1:
				*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			case 2:
				if (random == 0)
					*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				else
					*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			}
			break;
		case 2:
			switch (������sel)
			{
			case 0:
				*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			case 1:
				*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			case 2:
				if (random == 0)
					*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				else
					*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			}
			break;
		}
	}
	p[i << 1] = '\0';

}



void _sethangul(char *p, unsigned int count, unsigned int sel)
{
	unsigned int i;

	for (i = 0; i < count; ++i)
	{
		switch (sel)
		{
		case 0:
			*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
			break;
		case 1:
			*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
			break;
		}
	}
	p[i << 1] = '\0';
}

void __sethangul(char *p, unsigned int count)
{
	unsigned int i, random;
	
	if (������sel == 2)
		random = gen_rand() % 2;

	for (i = 0; i < count; ++i)
	{
		switch (gen_rand() % 3)
		{
		case 0:
			*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
			break;
		case 1:
			switch (������sel)
			{
			case 0:
				*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			case 1:
				*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			case 2:
				if (random == 0)
					*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				else
					*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			}
			break;
		case 2:
			switch (������sel)
			{
			case 0:
				*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			case 1:
				*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			case 2:
				if (random == 0)
					*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				else
					*(unsigned short*) &p[i << 1] = _byteswap_ushort('��');
				break;
			}
			break;
		}
	}
	p[i << 1] = '\0';
}

void setnumber(char *p, unsigned int count, unsigned int digit)
{
	unsigned int i;

	for (i = 0; i < count; ++i)
		p[i] = digit + '0';
	p[i] = '\0';
}

unsigned int getsel(char *p, unsigned char type)
{
	static unsigned int rand1 = -1, rand2; //rand1�� �̱�ų� �� ��° rand2�� �̱��� �����
	char strlist [][7] = { "������", "������", "������", "������", "������" };

	switch (type)
	{
	case 1:
		if (strstr(p, strlist[0]))
			return 2;
		else if (strstr(p, strlist[1]) || strstr(p, strlist[2]))
			return 0;
		else if (strstr(p, strlist[3]) || strstr(p, strlist[4]))
			return 1;
		else
			return -1;
		break;
	case 2:
		if (rand1 == -1)
			rand1 = gen_rand() % 3;
		if (wincount == rand1)
		{
			++wincount;
			rand2 = gen_rand() & 1; //0�̸� ���� 1�̸� ����
			if (rand2 == 0)
			{
				if (strstr(p, strlist[0]))
					return 0;
				else if (strstr(p, strlist[1]) || strstr(p, strlist[2]))
					return 1;
				else if (strstr(p, strlist[3]) || strstr(p, strlist[4]))
					return 2;
				else
					return -1;
			}
			else if (rand2 == 1)
			{
				if (strstr(p, strlist[0]))
					return 1;
				else if (strstr(p, strlist[1]) || strstr(p, strlist[2]))
					return 2;
				else if (strstr(p, strlist[3]) || strstr(p, strlist[4]))
					return 0;
				else
					return -1;
			}
		}
		++wincount;
		if (strstr(p, strlist[0]))
			return 2;
		else if (strstr(p, strlist[1]) || strstr(p, strlist[2]))
			return 0;
		else if (strstr(p, strlist[3]) || strstr(p, strlist[4]))
			return 1;
		else
			return -1;
		break;
	case 3:
		if (rand1 == -1)
			rand1 = gen_rand() % 3;
		if (wincount != rand1)
		{
			++wincount;
			rand2 = gen_rand() & 1; //0�̸� ���� 1�̸� ����
			if (rand2 == 0)
			{
				if (strstr(p, strlist[0]))
					return 0;
				else if (strstr(p, strlist[1]) || strstr(p, strlist[2]))
					return 1;
				else if (strstr(p, strlist[3]) || strstr(p, strlist[4]))
					return 2;
				else
					return -1;
			}
			else if (rand2 == 1)
			{
				if (strstr(p, strlist[0]))
					return 1;
				else if (strstr(p, strlist[1]) || strstr(p, strlist[2]))
					return 2;
				else if (strstr(p, strlist[3]) || strstr(p, strlist[4]))
					return 0;
				else
					return -1;
			}
		}
		++wincount;
		if (strstr(p, strlist[0]))
			return 2;
		else if (strstr(p, strlist[1]) || strstr(p, strlist[2]))
			return 0;
		else if (strstr(p, strlist[3]) || strstr(p, strlist[4]))
			return 1;
		else
			return -1;
		break;
	case 4:
		if (strstr(p, strlist[0]))
			return 0;
		else if (strstr(p, strlist[1]) || strstr(p, strlist[2]))
			return 1;
		else if (strstr(p, strlist[3]) || strstr(p, strlist[4]))
			return 2;
		else
			return -1;
		break;
	default:
		return -1;
	}

}



unsigned int _getsel(char *p, unsigned char type)
{
	static unsigned int rand1 = -1, rand2; //rand1�� �̱�ų� �� ��° rand2�� �̱��� �����

	switch (type)
	{
	case 1:
		if (*(unsigned short*) p == _byteswap_ushort('��'))
			return 2;
		else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
			return 0;
		else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
			return 1;
		else
			return -1;
		break;
	case 2:
		if (rand1 == -1)
			rand1 = gen_rand() % 3;
		if (wincount == rand1)
		{
			++wincount;
			rand2 = gen_rand() & 1; //0�̸� ���� 1�̸� ����
			if (rand2 == 0)
			{
				if (*(unsigned short*) p == _byteswap_ushort('��'))
					return 0;
				else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
					return 1;
				else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
					return 2;
				else
					return -1;
			}
			else if (rand2 == 1)
			{
				if (*(unsigned short*) p == _byteswap_ushort('��'))
					return 1;
				else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
					return 2;
				else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
					return 0;
				else
					return -1;
			}
		}
		++wincount;
		if (*(unsigned short*) p == _byteswap_ushort('��'))
			return 2;
		else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
			return 0;
		else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
			return 1;
		else
			return -1;
		break;
	case 3:
		if (rand1 == -1)
			rand1 = gen_rand() % 3;
		if (wincount != rand1)
		{
			++wincount;
			rand2 = gen_rand() & 1; //0�̸� ���� 1�̸� ����
			if (rand2 == 0)
			{
				if (*(unsigned short*) p == _byteswap_ushort('��'))
					return 0;
				else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
					return 1;
				else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
					return 2;
				else
					return -1;
			}
			else if (rand2 == 1)
			{
				if (*(unsigned short*) p == _byteswap_ushort('��'))
					return 1;
				else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
					return 2;
				else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
					return 0;
				else
					return -1;
			}
		}
		++wincount;
		if (*(unsigned short*) p == _byteswap_ushort('��'))
			return 2;
		else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
			return 0;
		else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
			return 1;
		else
			return -1;
		break;
	case 4:
		if (*(unsigned short*) p == _byteswap_ushort('��'))
			return 0;
		else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
			return 1;
		else if (*(unsigned short*) p == _byteswap_ushort('��') || *(unsigned short*) p == _byteswap_ushort('��'))
			return 2;
		else
			return -1;
		break;
	default:
		return -1;
	}

}



void trimstring(char *str)
{
	unsigned int i;

	for (i = 0; i < strlen(str); ++i)
	{
		if (str[i] == '?')
			str[i] = '/';
		else if (str[i] == ' ')
			memcpy(&str[i], &str[i + 1], strlen(str) - i), --i;
	}
}


unsigned int get_hangul_index(char *p, unsigned int count, unsigned int sel)
{
	unsigned int i, j;
	char str[3];

	switch (sel)
	{
	case 0:
		*(unsigned short*) str = _byteswap_ushort('��');
		break;
	case 1:
		*(unsigned short*) str = _byteswap_ushort('��');
		break;
	case 2:
		*(unsigned short*) str = _byteswap_ushort('��');
		break;
	}
	str[2] = '\0';

	for (i = 0, j = 0; i < strlen(p) && j != count; i += 2)
		if (*(unsigned short*) &p[i] == *(unsigned short*) str)
			++j;
	if (i != 0)
		return i >> 1;
	else
		return i;
}


void text_process(const char *data)
{
	unsigned int random;
	int firstsel, secondsel, thirdsel;
	char *token;
	char first[128];
	char second[128];
	char third[128];
	char buf[256];
	int idx[3];

	if (!operating)
		return;
	

	wincount = 0;

	trimstring(data);

	if (strlen(data) < 10)
		return;

	token = strtok((char*)data, "/");
	strcpy(first, token);
	token = strtok(NULL, "/");
	strcpy(second, token);
	token = strtok(NULL, "/");
	strcpy(third, token);


	firstsel = getsel(first, autotype);
	secondsel = getsel(second, autotype);
	thirdsel = getsel(third, autotype);

	if (firstsel == -1 || secondsel == -1 || thirdsel == -1)
		return;
	
	if (isbaram)
	{
		random = gen_rand() % 7 + 17;
		memset(first, 0, sizeof(first));
		__sethangul(first, random);

		random = gen_rand() % count_hangul(first, firstsel == 0 ? '��' : firstsel == 1 ? '��' : '��') + 1;
		idx[0] = get_hangul_index(first, random, firstsel);

		random = gen_rand() % count_hangul(first, secondsel == 0 ? '��' : secondsel == 1 ? '��' : '��') + 1;
		idx[1] = get_hangul_index(first, random, secondsel);

		random = gen_rand() % count_hangul(first, thirdsel == 0 ? '��' : thirdsel == 1 ? '��' : '��') + 1;
		idx[2] = get_hangul_index(first, random, thirdsel);

		switch (���þ�sel)
		{
		case 0:
			wsprintf(buf, "%s %u %u %u", first, idx[0], idx[1], idx[2]);
			break;
		case 1:
			wsprintf(buf, "%s %s %u %u %u", ���þ�, first, idx[0], idx[1], idx[2]);
			break;
		case 2:
			wsprintf(buf, "%s %u %u %u %s", first, idx[0], idx[1], idx[2], ���þ�);
			break;
		}
	}
	else
	{
		random = gen_rand() % 3 + 7;
		memset(first, 0, sizeof(first));
		sethangul(first, random, firstsel);

		random = gen_rand() % 3 + 7;
		memset(second, 0, sizeof(second));
		sethangul(second, random, secondsel);

		random = gen_rand() % 3 + 7;
		memset(third, 0, sizeof(third));
		sethangul(third, random, thirdsel);

		switch (���þ�sel)
		{
		case 0:
			wsprintf(buf, "%s/%s/%s", first, second, third);
			break;
		case 1:
			wsprintf(buf, "%s %s/%s/%s", ���þ�, first, second, third);
			break;
		case 2:
			wsprintf(buf, "%s/%s/%s %s", first, second, third, ���þ�);
			break;
		}
	}
	SetForegroundWindow(g_hMP);
	Clipboard(buf);
	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
	Sleep(50);
	PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
	Sleep(50);
	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	Sleep(150);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);

	operating = 0;
	autotype = 0;
	target_id = 0;
	EnableWindow(g_hEdit1, TRUE);
	EnableWindow(g_hCombo, TRUE);
	SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
	SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: �۵� ����");
}

void _trimstring(char *str)
{
	unsigned int i;

	for (i = 0; i < strlen(str); ++i)
		if (str[i] == '/' || str[i] == '?' || str[i] == ' ' || str[i] == '.' || str[i] == ',')
			while (str[i] == '/' || str[i] == '?' || str[i] == ' ' || str[i] == '.' || str[i] == ',')
				memcpy(&str[i], &str[i + 1], strlen(str) - i), --i;
}

unsigned int count_digit(const char *data)
{
	unsigned int i, count;

	for (i = 0, count = 0; i < strlen(data); ++i)
		if (isdigit(data[i]))
			++count;
	return count;
}

unsigned int count_operator(const char *data)
{
	unsigned int i, count;
	for (i = 0, count = 0; i < strlen(data); ++i)
		if (data[i] == '+' || data[i] == '-' || data[i] == '*' || data[i] == '/' || data[i] == 'x' || data[i] == 'X' || data[i] == '^')
			++count;
	return count;
}


char* GetPostFix(char *str1, char *str2)
{
	unsigned int i, j, size;
	cstack_int_t *s;

	s = cstack_alloc();

	for (i = 0, j = 0; i < strlen(str1); ++i)
	{
		if (!strchr(simbols, str1[i]))
		{
			str2[j++] = str1[i];
			continue;
		}
		str2[j++] = ' ';
		switch (str1[i])
		{
		case '(':
			cstack_push_int(s, '(');
			break;
		case ')':
			while (cstack_peek_int(s) != '(')
			{
				str2[j++] = cstack_pop_int(s);
				str2[j++] = ' ';
			}
			cstack_pop_int(s);
			break;
		case '+':
		case '-':
			while (cstack_isempty(s) == 0 && cstack_peek_int(s) != '(')
			{
				str2[j++] = cstack_pop_int(s);
				str2[j++] = ' ';
			}
			cstack_push_int(s, str1[i]);
			break;
		case '*':
		case '/':
		case 'x':
		case 'X':
			while (cstack_isempty(s) == 0 && (cstack_peek_int(s) == '*' || cstack_peek_int(s) == '/' || cstack_peek_int(s) == 'x' || cstack_peek_int(s) == 'X' || cstack_peek_int(s) == '^'))
			{
				str2[j++] = cstack_pop_int(s);
				str2[j++] = ' ';
			}
			cstack_push_int(s, str1[i]);
			break;
		case '^':
			while (cstack_isempty(s) == 0 && cstack_peek_int(s) == '^')
			{
				str2[j++] = cstack_pop_int(s);
				str2[j++] = ' ';
			}
			cstack_push_int(s, str1[i]);
			break;
			break;
		case ' ':
			break;
		}
		str2[j++] = ' ';
	}
	size = s->top + 1;
	for (i = 0; i < size; ++i)
	{
		str2[j++] = ' ';
		str2[j++] = cstack_pop_int(s);
	}

	cstack_free(s);
	str2[j] = '\0';
	return str2;
}



double Calculate(char *string)
{
	double result, d1, d2, dtemp;
	cstack_double_t *s;
	char *p;

	s = (cstack_double_t*) cstack_alloc();

	p = strtok(string, " ");
	while (p)
	{
		if (!strchr(simbols, *p))
		{
			dtemp = atof(p);
			cstack_push_double(s, dtemp);
		}
		else
		{
			d2 = cstack_pop_double(s);
			d1 = cstack_pop_double(s);
			//switch (simbols[strstr(simbols, p)]) �����ڰ� 2�� �پ������� ������ ��¥ ����
			//printf("%c\n", *p);
			switch (*p)
			{
			case '+':
				cstack_push_double(s, d1 + d2);
				break;
			case '-':
				cstack_push_double(s, d1 - d2);
				break;
			case '*':
			case 'x':
			case 'X':
				cstack_push_double(s, d1 * d2);
				break;
			case '/':
				cstack_push_double(s, d1 / d2);
				break;
			case '^':
				cstack_push_double(s, pow(d1, d2));
				break;
			}

		}
		p = strtok(NULL, " ");
	}

	result = cstack_pop_double(s);
	cstack_free((cstack_int_t*) s);
	return result;
}


void _text_process(const char *data)
{
	int firstsel = -1, secondsel = -1, thirdsel = -1, random;
	char first[128];
	char second[128];
	char third[128];
	char buf[256] = { 0, };
	char strlist[][7] = {"������", "������", "������", "������", "������", "������"};
	char *p, *context;
	unsigned int i, j, found, ���� = 0, count = 0;
	int idx[3];

	if (!operating)
		return;

	
	wincount = 0;

	if (strlen(data) < 8)
		return;
	found = -1;

	
	i = count_digit(data);
	j = count_operator(data);

	if (j >= 1 && i >= j + 1 && autotype >= 1 && autotype <= 4)
	{
		for (j = 0; j < strlen(data); ++j)
			if (isdigit(data[j]))
				break;
		p = strtok_s(data + j, " ", &context);
		if (p == NULL)
			return; 
		while (p)
		{
			GetPostFix(p, buf);
			idx[count++] = (unsigned int) Calculate(buf);
			p = strtok_s(NULL, " ", &context);

		}
		if (count == 0 || count == 2 || count > 3)
			return;
	}
	else if (i >= 5 && autotype == 9 || autotype == 8)
	{
		firstsel = 0;
		secondsel = 0;
		thirdsel = 0;	
	}


	if (count) //��Ģ���� �� ����
	{
		for (i = 0; i < strlen(data); ++i)
		{
			j = *(unsigned short*) &data[i];
			if (j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��')) //�� �� �� �� ��
				break;
			else if (i == strlen(data) - 1)
				return;
		}
		if (count == 1)
		{
			���� = 1;
			secondsel = 0;
			thirdsel = 0;
		}
		memcpy(first, &data[i + (idx[0] << 1) - 2], 2);
		first[2] = '\0';
		firstsel = _getsel(first, autotype);
		if (count == 3)
		{
			memcpy(second, &data[i + (idx[1] << 1) - 2], 2);
			memcpy(third, &data[i + (idx[2] << 1) - 2], 2);
			second[2] = '\0';
			third[2] = '\0';
			secondsel = _getsel(second, autotype);
			thirdsel = _getsel(third, autotype);
		}

	}
	else if (i >= 1 && i <= 2) //���� �ٶ��̸� FIXME ����̱�⿡�� �������� ���� �ø���
	{
		if (autotype != 1)
			return;
		���� = 1;
		secondsel = 0;
		thirdsel = 0;
		if (i == 1)
		{
			for (i = 0; i < strlen(data); ++i)
			{
				if (isdigit(data[i]))
				{
					idx[0] = data[i] - '0';
					break;
				}
			}
		}
		else
		{
			for (i = 0; i < strlen(data); ++i)
			{
				if (isdigit(data[i]) && isdigit(data[i + 1]))
				{
					idx[0] = 10 * (data[i] - '0') + data[i + 1] - '0';
					break;
				}
			}
		}
		for (i = 0; i < strlen(data); ++i)
		{
			j = *(unsigned short*) &data[i];
			if (j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��')) //�� �� �� �� ��
				break;
			else if (i == strlen(data) - 1)
				return;
		}
		memcpy(first, &data[i + (idx[0] << 1) - 2], 2);
		first[2] = '\0';
		firstsel = _getsel(first, autotype);
	}
	else if (i >= 3 && i <= 6) //���� �ٶ��̸�
	{
		if (!(autotype >= 1 && autotype <= 4))
			return;
		switch (i)
		{
		case 3:
			for (i = 0, j = 0; i < strlen(data); ++i)
				if (isdigit(data[i]))
					idx[j++] = data[i] - '0';
			break;
		case 4:
		case 5:
		case 6:
			for (i = 0, j = 0; i < strlen(data); ++i)
			{
				if (isdigit(data[i]) && isdigit(data[i + 1])) //���ڸ���
					idx[j++] = 10 * (data[i] - '0') + data[i + 1] - '0', ++i; //���ڸ����� ��ĭ �� �о���
				else if (isdigit(data[i]))
					if (data[i + 1] == '/' || data[i + 1] == '?' || data[i + 1] == '.' || data[i + 1] == ',' || data[i + 1] == ' ' || data[i + 1] == '\0') //���ڸ���
						idx[j++] = data[i] - '0';
			}
			break;

		}
		for (i = 0; i < strlen(data); ++i)
		{
			j = *(unsigned short*) &data[i];
			if (j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��') || j == _byteswap_ushort('��')) //�� �� �� �� ��
				break;
			else if (i == strlen(data) - 1)
				return;
		}
		memcpy(first, &data[i + (idx[0] << 1) - 2], 2);
		memcpy(second, &data[i + (idx[1] << 1) - 2], 2);
		memcpy(third, &data[i + (idx[2] << 1) - 2], 2);
		first[2] = '\0';
		second[2] = '\0';
		third[2] = '\0';
		firstsel = _getsel(first, autotype);
		secondsel = _getsel(second, autotype);
		thirdsel = _getsel(third, autotype);
	}
	else
	{
		found = 0;
		_trimstring(data);
		for (i = 0; i <= strlen(data) - 6; ++i)
		{
			if (memcmp(&data[i], "�Ǿռ�", 6) == 0 || memcmp(&data[i], "��ռ�", 6) == 0 || memcmp(&data[i], "������", 6) == 0)
			{
				found = 1;
				if (i == 0) //�Ǿտ� ��������
				{
					memcpy(first, &data[6], 2);
					memcpy(second, &data[8], 2);
					memcpy(third, &data[10], 2);
					break;
				}
				else
				{
					memcpy(first, &data[0], 2);
					memcpy(second, &data[2], 2);
					memcpy(third, &data[4], 2);
					break;
				}
			}
			else if (memcmp(&data[i], "�ǵڼ�", 6) == 0 || memcmp(&data[i], "��ڼ�", 6) == 0 || memcmp(&data[i], "������", 6) == 0)
			{
				found = 1;
				if (i + 6 == strlen(data)) //�ǵڿ���������
				{
					memcpy(first, &data[i - 6], 2);
					memcpy(second, &data[i - 4], 2);
					memcpy(third, &data[i - 2], 2);
					break;
				}
				else
				{
					memcpy(first, &data[strlen(data)] - 6, 2);
					memcpy(second, &data[strlen(data)] - 4, 2);
					memcpy(third, &data[strlen(data)] - 2, 2);
					break; 
				}
			}
		}
	}
	if (found == 1)
	{
		first[2] = '\0';
		second[2] = '\0';
		third[2] = '\0';
		firstsel = _getsel(first, autotype);
		secondsel = _getsel(second, autotype);
		thirdsel = _getsel(third, autotype);
	}
	else if (!found)
	{
		if (autotype == 8)
		{
			if (isdealer)
			{
				if ((p = strstr(data, strlist[1])) || (p = strstr(data, strlist[2])) || (p = strstr(data, strlist[5])))
				{
					memcpy(buf, p, 6);
					buf[6] = '\0';
					random = gen_rand() % 11 + 15;
					if (strcmp(buf, strlist[5]) == 0)
						setnumber(first, random, gen_rand() % 4 * 2 + 2);
					else
						setnumber(first, random, gen_rand() % 5 * 2 + 1);
				}
				else
					return;
			}
			else
			{
				for (i = 0; i < strlen(data); ++i)
					if (isdigit(data[i]))
						break;
				if (i == strlen(data) && !isdigit(data[i]))
					return;
				random = gen_rand() % 11 + 15;
				memset(first, 0, sizeof(first));
				_sethangul(first, random, !(data[i] & 1));
			}
		}
		else if (autotype == 9)
		{
			for (i = 0; i < strlen(data); ++i)
				if (isdigit(data[i]))
					break;
			if (i == strlen(data) && !isdigit(data[i]))
				return;
			random = gen_rand() % 11 + 15;
			memset(first, 0, sizeof(first));
			setnumber(first, random, data[i] - '0');
		}
		else
		{
			for (i = 0; i < 5; ++i)
			{
				if ((p = strstr(data, strlist[i])))
				{
					memcpy(buf, p, 6);
					buf[6] = '\0';
					break;
				}
			}
			if (p == NULL || !(autotype >= 1 && autotype <= 4))
				return;
			���� = 1;
			firstsel = getsel(buf, autotype);
			secondsel = getsel(buf, autotype);
			thirdsel = getsel(buf, autotype);
		}
		
	}
	
	if (firstsel == -1 || secondsel == -1 || thirdsel == -1)
	{
		SendMessage(g_hWnd, WM_HOTKEY, 6, 0);
		return;
	}
	
	

	if (!����)
	{
		if (autotype >= 1 && autotype <= 4)
		{
			if (isbaram)
			{
				random = gen_rand() % 7 + 17;
				memset(first, 0, sizeof(first));
				__sethangul(first, random);

				random = gen_rand() % count_hangul(first, firstsel == 0 ? '��' : firstsel == 1 ? '��' : '��') + 1;
				idx[0] = get_hangul_index(first, random, firstsel);

				random = gen_rand() % count_hangul(first, secondsel == 0 ? '��' : secondsel == 1 ? '��' : '��') + 1;
				idx[1] = get_hangul_index(first, random, secondsel);

				random = gen_rand() % count_hangul(first, thirdsel == 0 ? '��' : thirdsel == 1 ? '��' : '��') + 1;
				idx[2] = get_hangul_index(first, random, thirdsel);
			}
			else
			{
				random = gen_rand() % 3 + 7;
				memset(first, 0, sizeof(first));
				sethangul(first, random, firstsel);

				random = gen_rand() % 3 + 7;
				memset(second, 0, sizeof(second));
				sethangul(second, random, secondsel);

				random = gen_rand() % 3 + 7;
				memset(third, 0, sizeof(third));
				sethangul(third, random, thirdsel);
			}
		}
		
	}
	else
	{
		if (isbaram)
		{
			random = gen_rand() % 7 + 17;
			memset(first, 0, sizeof(first));
			__sethangul(first, random);

			random = gen_rand() % count_hangul(first, firstsel == 0 ? '��' : firstsel == 1 ? '��' : '��') + 1;
			idx[0] = get_hangul_index(first, random, firstsel);
			
		}
		else
		{
			random = gen_rand() % 9 + 20;
			memset(first, 0, sizeof(first));
			sethangul(first, random, firstsel);
		}

	}
	
	if (!���� && autotype != 8 && autotype != 9)
	{
		if (isbaram)
		{
			switch (���þ�sel)
			{
			case 0:
				wsprintf(buf, "%s %u %u %u", first, idx[0], idx[1], idx[2]);
				break;
			case 1:
				wsprintf(buf, "%s %s %u %u %u", ���þ�, first, idx[0], idx[1], idx[2]);
				break;
			case 2:
				wsprintf(buf, "%s %u %u %u %s", first, idx[0], idx[1], idx[2], ���þ�);
				break;
			}
		}
		else
		{
			switch (���þ�sel)
			{
			case 0:
				wsprintf(buf, "%s/%s/%s", first, second, third);
				break;
			case 1:
				wsprintf(buf, "%s %s/%s/%s", ���þ�, first, second, third);
				break;
			case 2:
				wsprintf(buf, "%s/%s/%s %s", first, second, third, ���þ�);
				break;
			}
		}
	}
	else
	{
		if (isbaram && ����)
		{
			switch (���þ�sel)
			{
			case 0:
				wsprintf(buf, "%s %u", first, idx[0]);
				break;
			case 1:
				wsprintf(buf, "%s %s %u", ���þ�, first, idx[0]);
				break;
			case 2:
				wsprintf(buf, "%s %u %s", first, idx[0], ���þ�);
				break;
			}
		}
		else
		{
			switch (���þ�sel)
			{
			case 0:
				wsprintf(buf, "%s", first);
				break;
			case 1:
				wsprintf(buf, "%s %s", ���þ�, first);
				break;
			case 2:
				wsprintf(buf, "%s %s", first, ���þ�);
				break;
			}
		}
	}

	SetForegroundWindow(g_hMP);
	Clipboard(buf);
	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
	Sleep(50);
	PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
	Sleep(50);
	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	Sleep(150);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);

	operating = 0;
	autotype = 0;
	target_id = 0;
	EnableWindow(g_hEdit1, TRUE);
	EnableWindow(g_hCombo, TRUE);
	SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
	SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: �۵� ����");
}


void __text_process(unsigned int sel)
{
	unsigned int random;
	char first[128], second[128], third[128], buf[128];
	int idx[3];

	if (!sel) //���� ��������
	{
		if (isbaram)
		{
			random = gen_rand() % 7 + 17;
			memset(first, 0, sizeof(first));
			__sethangul(first, random);

			idx[0] = gen_rand() % random + 1;
			idx[1] = gen_rand() % random + 1; //FIXME �ٶ� �������ڳ��� �����̶� �ҹ�������
			idx[2] = gen_rand() % random + 1;
		}
		else
		{
			random = gen_rand() % 3 + 7;
			memset(first, 0, sizeof(first));
			sethangul(first, random, random - 7);

			random = gen_rand() % 3 + 7;
			memset(second, 0, sizeof(second));
			sethangul(second, random, random - 7);

			random = gen_rand() % 3 + 7;
			memset(third, 0, sizeof(third));
			sethangul(third, random, random - 7);
		}
	}
	else //���� ��������
	{
		if (isbaram)
		{
			random = gen_rand() % 7 + 17;
			memset(first, 0, sizeof(first));
			__sethangul(first, random);

			idx[0] = gen_rand() % random + 1;

		}
		else
		{
			random = gen_rand() % 3 + 20;
			memset(first, 0, sizeof(first));
			sethangul(first, random, random - 20);
		}
	}

	if (!sel)
	{
		if (isbaram)
		{

			switch (���þ�sel)
			{
			case 0:
				wsprintf(buf, "%s %u %u %u", first, idx[0], idx[1], idx[2]);
				break;
			case 1:
				wsprintf(buf, "%s %s %u %u %u", ���þ�, first, idx[0], idx[1], idx[2]);
				break;
			case 2:
				wsprintf(buf, "%s %u %u %u %s", first, idx[0], idx[1], idx[2], ���þ�);
				break;
			}
		}
		else
		{
			switch (���þ�sel)
			{
			case 0:
				wsprintf(buf, "%s/%s/%s", first, second, third);
				break;
			case 1:
				wsprintf(buf, "%s %s/%s/%s", ���þ�, first, second, third);
				break;
			case 2:
				wsprintf(buf, "%s/%s/%s %s", first, second, third, ���þ�);
				break;
			}
		}
	}
	else
	{
		if (isbaram)
		{

			switch (���þ�sel)
			{
			case 0:
				wsprintf(buf, "%s %u", first, idx[0]);
				break;
			case 1:
				wsprintf(buf, "%s %s %u", ���þ�, first, idx[0]);
				break;
			case 2:
				wsprintf(buf, "%s %u %s", first, idx[0], ���þ�);
				break;
			}
		}
		else
		{
			switch (���þ�sel)
			{
			case 0:
				wsprintf(buf, "%s", first);
				break;
			case 1:
				wsprintf(buf, "%s %s", ���þ�, first);
				break;
			case 2:
				wsprintf(buf, "%s %s", first, ���þ�);
				break;
			}
		}
	}

	Clipboard(buf);
	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
	Sleep(50);
	PostMessage(g_hMP, WM_KEYDOWN, VK_INSERT, 0);
	Sleep(50);
	keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	Sleep(150);
	PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);

	operating = 0;
	autotype = 0;
	target_id = 0;
	EnableWindow(g_hEdit1, TRUE);
	EnableWindow(g_hCombo, TRUE);
	SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
	SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: �۵� ����");
	
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
			packet_handler_sub(s, (unsigned char *) pkt_data + j + 14u, i);
	}
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

void ReadINI()
{
	GetModuleFileName(GetModuleHandle(0), INIPath, sizeof(INIPath) - 1);

	memcpy(&INIPath[strlen(INIPath)] - 3, "ini", 3);

	if (!(SERVERINFO = GetPrivateProfileInt("PACKET", "SERVERINFO", 0, INIPath)))
		WritePrivateProfileString("PACKET", "SERVERINFO", "", INIPath);
	if (!(CHARACTER_APPEAR = GetPrivateProfileInt("PACKET", "CHARACTER_APPEAR", 0, INIPath)))
		WritePrivateProfileString("PACKET", "CHARACTER_APPEAR", "", INIPath);
	if (!(CHATTING = GetPrivateProfileInt("PACKET", "CHATTING", 0, INIPath)))
		WritePrivateProfileString("PACKET", "CHATTING", "", INIPath);
	if (!(adapter_sel = GetPrivateProfileInt("SETTING", "adapter_sel", 0, INIPath)))
		WritePrivateProfileString("SETTING", "adapter_sel", "0", INIPath);
}


DWORD WINAPI NpfLoop(LPVOID arg)
{

	for (;;)
		NpfCheckMessage();
	NpfStop();


	return 0;
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
			SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM) desc);
		}
		ReadINI();
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
			DestroyWindow(hDlg);
			break;
		case IDCANCEL:
			DestroyWindow(hDlg);
			return 0;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hDlg);
		break;

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
		for (i = 0; i < 10; ++i)
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
	char name[128], desc[128];

	switch (iMessage)
	{
	case WM_SOCKET:
		return ProcessSocketMessage(hDlg, wParam, lParam);
	case WM_HOTKEY:
		if (wParam == 5)
		{
			operating = 0;
			autotype = 0;
			target_id = 0;
			EnableWindow(g_hEdit1, TRUE);
			EnableWindow(g_hCombo, TRUE);
			SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
			SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: �۵� ����");
			return 0;
		}
		else if (wParam == 6 || wParam == 7)
		{
			operating = 0;
			autotype = 0;
			target_id = 0;
			EnableWindow(g_hEdit1, TRUE);
			EnableWindow(g_hCombo, TRUE);
			SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
			SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: �۵� ����");
			GetWindowText(g_hEdit1, ���þ�, sizeof(���þ�));
			__text_process(wParam - 6);
			return 0;
		}
		if (SendMessage(g_hCombo, CB_GETCURSEL, 0, 0) == -1)
		{
			MessageBox(GetDesktopWindow(), "������ ĳ���͸� �������ּ���", "�˸�", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			return 0;
		}
		if (���þ�sel != 0 && GetWindowTextLength(g_hEdit1) == 0)
		{
			MessageBox(GetDesktopWindow(), "���þ �Է����ּ���", "�˸�", MB_ICONINFORMATION | MB_SYSTEMMODAL);
			return 0;
		}
		
		
		GetWindowText(g_hEdit1, ���þ�, sizeof(���þ�));
		target_id = SendMessage(g_hCombo, CB_GETITEMDATA, (WPARAM) SendMessage(g_hCombo, CB_GETCURSEL, 0, 0), 0);
		operating = 1;		
		EnableWindow(g_hEdit1, FALSE);
		EnableWindow(g_hCombo, FALSE);
		autotype = wParam;
		SendMessage(g_hList1, LB_DELETESTRING, 1, 0);
		switch (autotype) 
		{
		case 1:
			SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: ��� �̱��");
			break;
		case 2:
			SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: 2�� �̱��");
			break;
		case 3:
			SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: 1�� �̱��");
			break;
		case 4:
			SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: ��� ����");
			break;
		case 8:
			SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: Ȧ/¦ ������");
			break;
		case 9:
			SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: ���� ������");
			break;
		
		}
		SetForegroundWindow(g_hMP);
		break;
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		VIRTUALIZER_SHARK_WHITE_START
		{
			if (!ConnectToServer(EXTERNAL_IP, 1818))
			{
				if (!ConnectToServer(INTERNAL_IP, 1818))
				{
					MessageBox(GetDesktopWindow(), "���� ����!", "�˸�", MB_ICONERROR | MB_SYSTEMMODAL);
					KillProcess();
				}
			}
			auth();
		}
		VIRTUALIZER_SHARK_WHITE_END
		g_hMP = FindWindow("MapleStoryClass", NULL);
		g_hCombo = GetDlgItem(hDlg, IDC_COMBO2);
		g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
		g_hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		g_hRadio1 = GetDlgItem(hDlg, IDC_RADIO1);
		g_hRadio2 = GetDlgItem(hDlg, IDC_RADIO2);
		g_hRadio3 = GetDlgItem(hDlg, IDC_RADIO3);
		g_hCheck1 = GetDlgItem(hDlg, IDC_CHECK1);
		g_hCheck2 = GetDlgItem(hDlg, IDC_CHECK2);
		g_hCheck3 = GetDlgItem(hDlg, IDC_CHECK3);
		SendMessage(g_hEdit1, EM_LIMITTEXT, 12, 0);
		SystemParametersInfo(SPI_SETKEYBOARDDELAY, 0, 0, SPIF_UPDATEINIFILE);
		SystemParametersInfo(SPI_SETKEYBOARDSPEED, 31, 0, SPIF_UPDATEINIFILE);
		SendMessage(g_hList1, LB_INSERTSTRING, 0, (LPARAM)"���� ä��: NULL");
		SendMessage(g_hList1, LB_INSERTSTRING, 1, (LPARAM)"����: �۵� ����");
		CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
		if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
			���þ�sel = 0;
		else if (IsDlgButtonChecked(hDlg, IDC_RADIO2) == BST_CHECKED)
			���þ�sel = 1;
		else
			���þ�sel = 2;
		
		if (RegisterHotKey(g_hWnd, 1, MOD_NOREPEAT, VK_F1) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F1", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 2, MOD_NOREPEAT, VK_F2) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F2", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 3, MOD_NOREPEAT, VK_F3) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F3", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 4, MOD_NOREPEAT, VK_F4) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F4", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 5, MOD_NOREPEAT, VK_F5) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F5", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 6, MOD_NOREPEAT, VK_F6) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F6", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 7, MOD_NOREPEAT, VK_F7) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F7", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 8, MOD_NOREPEAT, VK_F8) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F8", "�˸�", MB_ICONERROR);
		if (RegisterHotKey(g_hWnd, 9, MOD_NOREPEAT, VK_F9) == 0 && GetLastError() == ERROR_HOTKEY_ALREADY_REGISTERED)
			MessageBox(0, "��Ű ��� ����!: F9", "�˸�", MB_ICONERROR);

		ReadINI();
		NpfFindAllDevices();
		NpfGetDeviceInfo(adapter_sel + 1, name, desc);
		NpfSetDevice(name);
		if (NpfStart())
			CloseHandle(CreateThread(0, 0, NpfLoop, 0, 0, 0));
		else
		{
			MessageBox(hDlg, "NpfStart() error!", "�˸�", MB_ICONERROR);
			ExitProcess(0);

		}
		break;
	case WM_CLOSE:
		UnregisterHotKey(g_hWnd, 1);
		UnregisterHotKey(g_hWnd, 2);
		UnregisterHotKey(g_hWnd, 3);
		UnregisterHotKey(g_hWnd, 4);
		UnregisterHotKey(g_hWnd, 5);
		UnregisterHotKey(g_hWnd, 6);
		UnregisterHotKey(g_hWnd, 7);
		UnregisterHotKey(g_hWnd, 8);
		UnregisterHotKey(g_hWnd, 9);
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK1:
			isdealer = SendMessage(g_hCheck1, BM_GETCHECK, 0, 0);
			break;
		case IDC_CHECK2:
			isbaram = SendMessage(g_hCheck2, BM_GETCHECK, 0, 0);
			break;
		case IDC_CHECK3:
			������sel = SendMessage(g_hCheck3, BM_GETCHECK, 0, 0);
			break;
		case IDC_RADIO1:
			���þ�sel = 0;
			break;
		case IDC_RADIO2:
			���þ�sel = 1;
			break;
		case IDC_RADIO3:
			���þ�sel = 2;
			break;
		case IDC_BUTTON1: //����
			CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		case IDC_BUTTON2: //�׽�Ʈ
			if (!startup)
			{
				MessageBox(GetDesktopWindow(), "ĳ�ü��� ���� �ͼ� ���� ä���� �� �Ŀ� �ٽ� �õ����ּ���.", "�˸�", MB_ICONINFORMATION);
				return 0;
			}
			MessageBox(GetDesktopWindow(), "ä��â�� ��ο��Է� �׽�Ʈ��� ���ּ���", "�˸�", MB_ICONINFORMATION);
			testing = 1;
			SetDlgItemText(hDlg, IDC_BUTTON2, "�׽�Ʈ ��");
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON2), FALSE);
			break;
		case IDC_BUTTON3:
			MessageBox(GetDesktopWindow(), "F1: ��� �̱��\nF2: 2�� �̱��\nF3: 1�� �̱��\nF4: ��� ����\nF5: �۵� ����\nF6: ���� �������� ����\nF7: ���� �������� ����\nF8: Ȧ/¦ ������\nF9: ���� ������\n\n���������� ���� üũ�ڽ��� üũ�� ���� ������ ���������� ����, �� �� üũ�� ������ ���������� ����, �� �� üũ�� ������ �������� ��", "�˸�", MB_ICONINFORMATION);
			break;
		}
		break;
	}
	return 0;
}


void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey [] = "����ī����";

	for (unsigned int i = 0; i != size; ++i)
		((char *) out)[i] = ((char *) in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
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
		data[count] = (unsigned char) strtol(buf, &endptr, 0);
		pos += sizeof(char) << 1;

		if (endptr[0] != '\0'); //return -1(non-hexadecimal chararcter encountered)

	}

	return 0;
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




unsigned int nothing()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		MessageBox(0, "�줻", "�˸�", 0);
		WinExec("", 0);
		SetCurrentDirectory("C:\\");
		CloseHandle(0);
		memcpy(0, 0, 0);
		EnableWindow(0, FALSE);
		ReadFile(0, 0, 0, 0, 0);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MessageBox, 0, 0, 0);
		HeapFree(0, 0, 0);
		FlushInstructionCache(GetCurrentProcess(), 0, 0);
	}
	VIRTUALIZER_SHARK_WHITE_END
	return 0;
}

unsigned int _nothing()
{
	return 0;
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	DWORD oldprotect, username_size = sizeof(username) - 1;
	WSADATA wsaData;
	unsigned int i;
	unsigned char code[1024];
	
	VIRTUALIZER_SHARK_WHITE_START
	{
		SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
		vmemcpy(SetUnhandledExceptionFilter, "\xC2\x04\x00", 3);

		if (IsUserAnAdmin() == FALSE)
		{
			MessageBox(GetDesktopWindow(), "������ ������ �ƴմϴ�.\n������ �������� �ٽ� �������ּ���.", "�˸�", MB_ICONERROR);
			KillProcess();
		}

		if (!FileNameCheck("���� ���ڸ�ũ��.exe"))
			KillProcess();

		if (WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			MessageBox(GetDesktopWindow(), "WSAStartup error!", "�˸�", MB_ICONERROR);
			KillProcess();
		}

		if(FindWindow("#32770", "���� ���ڸ�ũ��"))
		{
			SetForegroundWindow(FindWindow("#32770", "���� ���ڸ�ũ��"));
			KillProcess();
		}


		init_rand(GetCurrentProcessId() ^ 0xBADF00Du);

		GetUserName(username, &username_size);


		for (i = 0; i < sizeof(code); ++i)
			code[i] = (unsigned char)gen_rand();
		i = abs((DWORD)_nothing - (DWORD)nothing);

		VirtualProtect(nothing, i, PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy(nothing, code, i);
		VirtualProtect(nothing, i, oldprotect, &oldprotect);

		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, DlgProc);

	}

	
	VIRTUALIZER_SHARK_WHITE_END

	return 0;
}