#define _CRT_SECURE_NO_WARNINGS
#include "common.h"

#define ����â�ݱ�x0 488
#define ����â�ݱ�y0 362
#define ����������x0 323
#define ����������y0 118
#define �ý���x0 757
#define �ý���y0 585
#define �溯��x0 763
#define �溯��y0 437
#define ä�ΰ���x0 333
#define ä�ΰ���y0 245
#define ä������x0 471
#define ä������y0 245
#define ��1x0 257
#define ��1_5y0 267
#define ��2x0 ��1x0 + 70
#define ��3x0 ��2x0 + 70
#define ��4x0 ��3x0 + 70
#define ��5x0 ��4x0 + 70
#define ��6x0 ��1x0
#define ��6_10y0 ��1_5y0 + 20
#define ��7x0 ��2x0
#define ��8x0 ��3x0
#define ��9x0 ��4x0
#define ��10x0 ��5x0
#define ��11x0 ��6x0
#define ��11_12y0 ��6_10y0 + 20
#define ��12x0 ��7x0

#define ����â�ݱ�x1 604
#define ����â�ݱ�y1 448
#define ����������x1 433
#define ����������y1 203
#define �ý���x1 764
#define �ý���y1 746
#define �溯��x1 763
#define �溯��y1 606
#define ä�ΰ���x1 445
#define ä�ΰ���y1 329
#define ä������x1 583
#define ä������y1 329
#define ��1x1 369
#define ��1_5y1 352
#define ��2x1 ��1x1 + 70
#define ��3x1 ��2x1 + 70
#define ��4x1 ��3x1 + 70
#define ��5x1 ��4x1 + 70
#define ��6x1 ��1x1
#define ��6_10y1 ��1_5y1 + 20
#define ��7x1 ��2x1
#define ��8x1 ��3x1
#define ��9x1 ��4x1
#define ��10x1 ��5x1
#define ��11x1 ��6x1
#define ��11_12y1 ��6_10y1 + 20
#define ��12x1 ��7x1

#define ����â�ݱ�x2 773
#define ����â�ݱ�y2 446
#define ����������x2 606
#define ����������y2 205
#define �ý���x2 758
#define �ý���y2 753
#define �溯��x2 756
#define �溯��y2 608
#define ä�ΰ���x2 614
#define ä�ΰ���y2 330
#define ä������x2 755
#define ä������y2 330
#define ��1x2 542
#define ��1_5y2 355
#define ��2x2 ��1x2 + 70
#define ��3x2 ��2x2 + 70
#define ��4x2 ��3x2 + 70
#define ��5x2 ��4x2 + 70
#define ��6x2 ��1x2
#define ��6_10y2 ��1_5y2 + 20
#define ��7x2 ��2x2
#define ��8x2 ��3x2
#define ��9x2 ��4x2
#define ��10x2 ��5x2
#define ��11x2 ��6x2
#define ��11_12y2 ��6_10y2 + 20
#define ��12x2 ��7x2


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void RFCrypt(int size, const void *in, void *out);
SOCKET ConnectToServer();
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInst;
LPSTR lpszClass = "";
HWND g_hMP, g_hWnd, g_hList1, g_hList2, g_hCheck1, g_hCheck2, g_hCheck3;
unsigned int clickState, startup, adapterSel;

typedef struct session 
{
	unsigned int auth, recv_iv, send_iv, ver, size;
	unsigned char data[128 * 1024];
} session_t;

ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];

void mouse_input2(HWND hwnd, int x, int y)
{
	POINT pt;

	ClientToScreen(hwnd, ((pt.x = x, pt.y = y), &pt));
	SetCursorPos(pt.x, pt.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	SetCursorPos(pt.x, pt.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void mouse_input(HWND hwnd, int x, int y)
{
	POINT pt;

	ClientToScreen(hwnd, ((pt.x = x, pt.y = y), &pt));
	SetCursorPos(pt.x, pt.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

}


void warp(unsigned int channel, unsigned int room, unsigned int currentChannel)
{
	unsigned int i;
	RECT rect;
	int xs, ys, �ػ�Sel;

	BlockInput(1);
	g_hMP = FindWindow("MapleStoryClass", NULL);
	GetClientRect(g_hMP, &rect);
	xs = rect.right - rect.left;
	ys = rect.bottom - rect.top;
	
	if (xs = 800 && ys == 600)
		�ػ�Sel = 0;
	else if (xs == 1024 && ys == 768)
		�ػ�Sel = 1;
	else if (xs == 1366 && ys == 768)
		�ػ�Sel = 2;

	PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, NULL);
	SetForegroundWindow(g_hMP);
	Sleep(200);
	
	switch (�ػ�Sel)
	{
	case 0:
		mouse_input(g_hMP, ����â�ݱ�x0, ����â�ݱ�y0);
		break;
	case 1:
		mouse_input(g_hMP, ����â�ݱ�x1, ����â�ݱ�y1);
		break;
	case 2:
		mouse_input(g_hMP, ����â�ݱ�x2, ����â�ݱ�y2);
		break;
	}
	Sleep(100);
	switch (�ػ�Sel)
	{
	case 0:
		mouse_input(g_hMP, ����������x0, ����������y0);
		break;
	case 1:
		mouse_input(g_hMP, ����������x1, ����������y1);
		break;
	case 2:
		mouse_input(g_hMP, ����������x2, ����������y2);
		break;
	}
	Sleep(100);
	switch (�ػ�Sel)
	{
	case 0:
		mouse_input(g_hMP, �ý���x0, �ý���y0);
		break;
	case 1:
		mouse_input(g_hMP, �ý���x1, �ý���y1);
		break;
	case 2:
		mouse_input(g_hMP, �ý���x2, �ý���y2);
		break;
	}
	Sleep(100);
	switch (�ػ�Sel)
	{
	case 0:
		mouse_input(g_hMP, �溯��x0, �溯��y0);
		break;
	case 1:
		mouse_input(g_hMP, �溯��x1, �溯��y1);
		break;
	case 2:
		mouse_input(g_hMP, �溯��x2, �溯��y2);
		break;
	}
	Sleep(100);
	if (SendMessage(g_hCheck3, BM_GETCHECK, BST_CHECKED, 0))
	{
		if (channel != 1 && channel != 20)
			++channel;
		else if (channel == 20)
			channel = 2;
		if (currentChannel != 1 && currentChannel != 20)
			++currentChannel;
		else if (currentChannel == 20)
			currentChannel = 2;
		if (channel < currentChannel)
		{
			switch (�ػ�Sel)
			{
			case 0:
				for (i = 0; i < currentChannel - channel; ++i)
					mouse_input(g_hMP, ä�ΰ���x0, ä�ΰ���y0);
				break;
			case 1:
				for (i = 0; i < currentChannel - channel; ++i)
					mouse_input(g_hMP, ä�ΰ���x1, ä�ΰ���y1);
				break;
			case 2:
				for (i = 0; i < currentChannel - channel; ++i)
					mouse_input(g_hMP, ä�ΰ���x2, ä�ΰ���y2);
				break;
			}

		}
		else if (channel > currentChannel)
		{
			switch (�ػ�Sel)
			{
			case 0:
				for (i = 0; i < channel - currentChannel; ++i)
					mouse_input(g_hMP, ä������x0, ä������y0);
				break;
			case 1:
				for (i = 0; i < channel - currentChannel; ++i)
					mouse_input(g_hMP, ä������x1, ä������y1);
				break;
			case 2:
				for (i = 0; i < channel - currentChannel; ++i)
					mouse_input(g_hMP, ä������x2, ä������y2);
				break;
			}
		}
	}
	else
	{
		if (channel < currentChannel)
		{
			switch (�ػ�Sel)
			{
			case 0:
				for (i = 0; i < currentChannel - channel; ++i)
					mouse_input(g_hMP, ä�ΰ���x0, ä�ΰ���y0);
				break;
			case 1:
				for (i = 0; i < currentChannel - channel; ++i)
					mouse_input(g_hMP, ä�ΰ���x1, ä�ΰ���y1);
				break;
			case 2:
				for (i = 0; i < currentChannel - channel; ++i)
					mouse_input(g_hMP, ä�ΰ���x2, ä�ΰ���y2);
				break;
			}
		}
		else if (channel > currentChannel)
		{
			switch (�ػ�Sel)
			{
			case 0:
				for (i = 0; i < channel - currentChannel; ++i)
					mouse_input(g_hMP, ä������x0, ä������y0);
				break;
			case 1:
				for (i = 0; i < channel - currentChannel; ++i)
					mouse_input(g_hMP, ä������x1, ä������y1);
				break;
			case 2:
				for (i = 0; i < channel - currentChannel; ++i)
					mouse_input(g_hMP, ä������x2, ä������y2);
				break;
			}
		}

	}
	Sleep(100);
	switch (room)
	{
	case 1:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��1x0, ��1_5y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��1x1, ��1_5y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��1x2, ��1_5y2);
			break;
		}
		break;
	case 2:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��2x0, ��1_5y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��2x1, ��1_5y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��2x2, ��1_5y2);
			break;
		}
		break;
	case 3:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��3x0, ��1_5y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��3x1, ��1_5y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��3x2, ��1_5y2);
			break;
		}
		break;
	case 4:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��4x0, ��1_5y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��4x1, ��1_5y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��4x2, ��1_5y2);
			break;
		}
		break;
	case 5:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��5x0, ��1_5y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��5x1, ��1_5y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��5x2, ��1_5y2);
			break;
		}
		break;
	case 6:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��6x0, ��6_10y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��6x1, ��6_10y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��6x2, ��6_10y2);
			break;
		}
		break;
	case 7:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��7x0, ��6_10y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��7x1, ��6_10y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��7x2, ��6_10y2);
			break;
		}
		break;
	case 8:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��8x0, ��6_10y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��8x1, ��6_10y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��8x2, ��6_10y2);
			break;
		}
		break;
	case 9:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��9x0, ��6_10y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��9x1, ��6_10y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��9x2, ��6_10y2);
			break;
		}
		break;
	case 10:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��10x0, ��6_10y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��10x1, ��6_10y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��10x2, ��6_10y2);
			break;
		}
		break;
	case 11:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��11x0, ��11_12y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��11x1, ��11_12y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��11x2, ��11_12y2);
			break;
		}
		break;
	case 12:
		switch (�ػ�Sel)
		{
		case 0:
			mouse_input2(g_hMP, ��12x0, ��11_12y0);
			break;
		case 1:
			mouse_input2(g_hMP, ��12x1, ��11_12y1);
			break;
		case 2:
			mouse_input2(g_hMP, ��12x2, ��11_12y2);
			break;
		}
		break;
	}
	BlockInput(0);
	keybd_event(VK_LCONTROL, 0, 2, 0);
}

void packet_handle(session_t *s, unsigned char *data, unsigned int size)
{
	static int channel, room, currentChannel;
	unsigned int i, textSize;
	char textData[128], buf[128], *p, *q, buf2[128];
	SYSTEMTIME st;
	
	switch (*(unsigned short*)data)
	{
	case 0x102: //ä�� ��Ŷ
		if (*(unsigned char*)&data[2] == 0)
			currentChannel = 1;
		else if (*(unsigned char*)&data[2] == 1)
			currentChannel = 20;
		else 
			currentChannel = *(unsigned char*)&data[2];
		if (currentChannel == 20)
			wsprintf(buf, "���� ä��: %d��", currentChannel);
		else
			wsprintf(buf, "���� ä��: %dä��", currentChannel);
		SendMessage(g_hList2, LB_DELETESTRING, 0, 0);
		SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)buf);
		break;
	case 0xD5:
		i = *(unsigned short*)&data[6];
		textSize = *(unsigned short*)&data[8 + i];
		memcpy(textData, &data[8 + i + 2], textSize);
		textData[textSize] = '\0';
		strcpy(buf, textData);
		p = _strupr(buf);
		if (!strstr(p, "GOGO"))
			return;
		p = strchr(buf, '-');
		if (p == NULL)
			return;
		i = -1;
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
		if (channel < 1 || 20 < channel)
			return;
		strcpy(buf, textData);
		p = strchr(buf, '-');
		i = 1;
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
		SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)textData), 0);
		if (SendMessage(g_hCheck1, BM_GETCHECK, BST_CHECKED, 0))
			PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
		if (SendMessage(g_hCheck2, BM_GETCHECK, BST_CHECKED, 0) || !clickState)
			warp(channel, room, currentChannel);
		
		break;
	case 0x4C: //Ȯ���� ��
		if (*(unsigned char*)&data[2] != 0x2/*Ȯ����*/ && *(unsigned char*)&data[2] != 0x3/*���� Ȯ����*/ && *(unsigned char*)&data[2] != 0x8/*������ Ȯ����*/)
		return;
		textSize = *(unsigned short*)&data[3];
		memcpy(textData, &data[5], textSize);
		textData[textSize] = '\0';
		strcpy(buf, textData);
		p = _strupr(buf);
		if (!strstr(p, "GOGO"))
			return;
		p = strchr(buf, '-');
		if (p == NULL)
			return;
		i = -1;
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
		if (channel < 1 || 20 < channel)
			return;
		strcpy(buf, textData);
		p = strchr(buf, '-');
		i = 1;
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
		SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)textData), 0);
		if (SendMessage(g_hCheck1, BM_GETCHECK, BST_CHECKED, 0))
			PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
		if (SendMessage(g_hCheck2, BM_GETCHECK, BST_CHECKED, 0) || !clickState)
			warp(channel, room, currentChannel);
		
		break;
	case 0x27: //êâ�� �ߴ� ��ȫ�޽���
		if (*(unsigned char*)&data[2] == 0x0B)
		{
			if (strncmp(&data[5], "����������", strlen("����������")) == 0)
			{
				if (SendMessage(g_hCheck1, BM_GETCHECK, 0, 0) == BST_CHECKED)
					PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
				memcpy(buf2, &data[5], *(unsigned short*)&data[3]);
				SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf2), 0);
			}
		}
		break;
	case 0x2FF:
		if (*(unsigned short*)&data[2] == 0x11C && *(unsigned char*)&data[3] == 0x11)
		{
			PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, NULL);
			SetForegroundWindow(g_hMP);
			if (SendMessage(g_hCheck1, BM_GETCHECK, 0, 0) == BST_CHECKED)
				PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
			SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)"������ ������ ��ǰ ���� �߿� �ֽ��ϴ�."), 0);
		}
		else if (*(unsigned char*)&data[2] != 0x14 || !clickState)
			return;
		if (*(unsigned short*)&data[4] == 0x12);
		else if (*(unsigned char*)&data[3] == 0x5 || *(unsigned char*)&data[3] == 0x6)
		{
			clickState = 0;
			GetLocalTime(&st);
			if (SendMessage(g_hCheck1, BM_GETCHECK, 0, 0) == BST_CHECKED)
				PlaySound(MAKEINTRESOURCE(IDR_WAVE1), GetModuleHandle(NULL), SND_ASYNC | SND_RESOURCE);
			wsprintf(buf, "%s %u�� %u�� %u�� ���� ���� ����! ¯����Ŭ ����", st.wHour >= 12 ? "����" : "����", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond);
			SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
		}
		break;
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
			//dbg("%p %s\n", s, "�ν�����");
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
			//dbg("%p %s\n", s, "�νĽ���");
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

	for (i = 0; *name; ++i) 
	{
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

	for (i = 1; *name; ++i) 
	{
		if (i == index) 
		{
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

int FileNameCheck(HINSTANCE hInstance, char *fileName)
{
	int i;
	char myFileName[128];

	GetModuleFileName(hInstance, myFileName, sizeof(myFileName));
	for (i = strlen(myFileName); myFileName[i] != '\\'; --i);
	if (strcmp(&myFileName[i + 1], fileName))
	{
		rename(&myFileName[i + 1], fileName);
		return FALSE;
	}
	return TRUE;

}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	int size;
	char message[1024];
	SOCKET hSock;
	DWORD dwSerialNumber;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
	
	g_hMP = FindWindow("MapleStoryClass",NULL);
	PostMessage(g_hMP, WM_SYSCOMMAND, SC_RESTORE, NULL);
	

	if (IsUserAnAdmin() == FALSE)
	{
		MessageBox(0, "������ ������ �ƴմϴ�.\n�����ڱ������� �ٽ� ���ּ���.", "�޼����ڽ�", MB_OK);
		return 0;
	}
	if (!FileNameCheck(hInstance, "���� ¯������.exe"))
		return 0;
	if (WSAStartup(MAKEWORD(2, 2), (LPWSADATA)message))
	{
		MessageBox(0, "WinSock �ʱ�ȭ ����", 0, 0);
		return 0;
	}
	VIRTUALIZER1_START
	{
		hSock = ConnectToServer(); 
		if (hSock == INVALID_SOCKET)
		{
			MessageBox(0, "���� ����!", 0, 0);
			return 0;
		}
		GetVolumeInformation("C:\\", NULL, 0, &dwSerialNumber, NULL, NULL, NULL, 0);
		size = wsprintf(message, "RW %d", dwSerialNumber);
		RFCrypt(size, message, message);
		send(hSock, message, size, 0);

		size = recv(hSock, message, size, 0);
		RFCrypt(size, message, message);
		closesocket(hSock);
		if (size == SOCKET_ERROR)
		{
			MessageBox(0, "recv() ����!", 0, 0);
			return 0;
		}
		message[size] = '\0';
		if (message[0] == 'O');
		else if (message[0] == 'X')
		{
			MessageBox(0, "�ϵ���������!", "�˸�", MB_ICONSTOP);
			return 0;
		}
		else if (message[0] == 'N')
		{
			MessageBox(0, "�Ⱓ ����!", "�˸�", MB_ICONINFORMATION);
			return 0;
		}
	}
	VIRTUALIZER_END
	CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

DWORD WINAPI NpfLoop(LPVOID arg)
{
	for (;;)
		NpfCheckMessage();
	NpfStop();

	return 0;
}

void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey[] = "����ī����";

	VIRTUALIZER1_START
	{
		for (int i = 0; i != size; ++i)
		((char *)out)[i] = ((char *)in)[i] ^ CryptKey[i % (sizeof(CryptKey)-1)];
	}
	VIRTUALIZER_END
}

u_long host2ip(const char *host)
{
	u_long ipaddr;
	struct hostent *h;

	if ((ipaddr = inet_addr(host)) == INADDR_NONE)
	if ((h = gethostbyname(host)) != NULL)
		ipaddr = *(u_long *)h->h_addr;

	return ipaddr;
}



SOCKET ConnectToServer()
{
	SOCKET hSock;
	SOCKADDR_IN servAdr;

	VIRTUALIZER1_START
	{
		hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = host2ip("mnjihw.kr.pe");
		servAdr.sin_port = htons(atoi("6969"));
		if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		{
			closesocket(hSock);
			return INVALID_SOCKET;
		}
	}
	VIRTUALIZER_END
	return hSock;
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

DWORD WINAPI clickfunc(LPVOID *arg)
{
	while (clickState)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(30);
		PostMessage(g_hMP, WM_KEYDOWN, VK_RETURN, 0);
		Sleep(30);
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
		switch(LOWORD(wParam))
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

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char buf[128];
	SYSTEMTIME st;

	switch (iMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_LIST1:
			switch (HIWORD(wParam))
			{
			case LBN_DBLCLK:
				SendMessage(g_hList1, LB_GETTEXT, (WPARAM)SendMessage(g_hList1, LB_GETCARETINDEX, 0, 0), (LPARAM)buf);
				Clipboard(buf);
				break;
			}
			break;
		case IDC_BUTTON1:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), g_hWnd, SettingDlgProc);
			break;
		}
		break;
	case WM_HOTKEY:
		if (!startup)
		{
			MessageBox(g_hWnd, "ĳ�ü��� ���� �ͼ� ���� ä����\n����� ǥ�õ� �� �ٽ� �õ��� �ּ���.", "�˸�", MB_ICONINFORMATION);
			return 0;
		}
		switch (wParam)
		{
		case 8:
			if (clickState)
				return 0;
			GetLocalTime(&st);
			wsprintf(buf, "%s %u�� %u�� %u�� ¯����Ŭ ����", st.wHour >= 12 ? "����" : "����", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond);
			SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
			g_hMP = FindWindow("MapleStoryClass", NULL);
			clickState = 1;
			CreateThread(0, 0, clickfunc, 0, 0, 0);
			break;
		case 10:
			GetLocalTime(&st);
			wsprintf(buf, "%s %u�� %u�� %u�� ¯����Ŭ ����", st.wHour >= 12 ? "����" : "����", st.wHour >= 12 ? st.wHour - 12 : st.wHour, st.wMinute, st.wSecond);
			SendMessage(g_hList1, LB_SETTOPINDEX, (WPARAM)SendMessage(g_hList1, LB_ADDSTRING, 0, (LPARAM)buf), 0);
			clickState = 0;
			break;
		}
		break;
	case WM_INITDIALOG:
		g_hWnd = hDlg;
		g_hList1 = GetDlgItem(hDlg, IDC_LIST1);
		g_hList2 = GetDlgItem(hDlg, IDC_LIST2);
		g_hCheck1 = GetDlgItem(hDlg, IDC_CHECKBOX1);
		g_hCheck2 = GetDlgItem(hDlg, IDC_CHECKBOX2);
		g_hCheck3 = GetDlgItem(hDlg, IDC_CHECKBOX3);
		SendMessage(g_hList2, LB_INSERTSTRING, 0, (LPARAM)"���� ä��: NULL");
		SendMessage(g_hCheck1, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(g_hCheck2, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(g_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1)));
		RegisterHotKey(g_hWnd, 8, NULL, VK_F8);
		RegisterHotKey(g_hWnd, 10, NULL, VK_F10);
		NpfFindAllDevices();
		if (NpfStart())
			CreateThread(0, 0, NpfLoop, 0, 0, 0);
		else
		{
			MessageBox(0, "NpfStart() ����", "�˸�", MB_ICONERROR);
			ExitProcess(1);
		}
		return 0;
	case WM_CLOSE:
		UnregisterHotKey(g_hWnd, 8);
		UnregisterHotKey(g_hWnd, 11);
		WSACleanup();
		PostQuitMessage(0);

	}
	return 0;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
