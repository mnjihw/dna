#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 100
#define MAX_CLNT 256

void RFCrypt(int size, const void *in, void *out);
void ErrorHandling(char *msg);

FILE *log_fp;
unsigned int sel, check;
char INIPath[MAX_PATH];
char version[8];


void vdbg(const char *fmt, va_list arg)
{
	char msg[4096];
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), msg, wvsprintfA(msg, fmt, arg), (DWORD *) &msg[1024], NULL);
}

void dbg(const char *fmt, ...)
{
	vdbg(fmt, (va_list) (((unsigned int) &fmt) + sizeof(void*)));
}


void RFCrypt(int size, const void *in, void *out)
{
	static char CryptKey [] = "����ī����";

	for (int i = 0; i != size; ++i)
		((char *) out)[i] = ((char *) in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
}



void Crypt(int size, const void *in, void *out)
{

	for (int i = 0; i != size; ++i)
	{
		if (((char*) out)[i] == ' ')
			continue;
		((char *) out)[i] = ((char *) in)[i] ^ 5;
	}

}

void datecrypt(const void *key, int keysize, const void *in, void *out, int size)
{
	int i, j;
	for (i = 0, j = 0; i != size; ++i, ++j)
		((char *) out)[i] = ((char *) in)[i] ^ ((char*) key)[keysize / 2];

}

void check_auth(char *data, char *ipaddr, char *���, char *username)
{
	static SYSTEMTIME now, today, st;
	int val = 0;
	static int count;
	char name[128] = { 0, }, buf[128];
	char version[8];
	char data2[2048];
	char �޼���[1024];
	FILE *fp = 0;


	GetLocalTime(&now);
	if (today.wDay == NULL)
		today.wDay = now.wDay;
	if (today.wDay != now.wDay)
		count = 0;
	++count;
	today.wDay = now.wDay;

	switch (sel)
	{
	case 1:
		strcpy(���, "NULL");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\����.txt", "rt");
		if (fp == NULL)
		{
			dbg("���� ���� ����!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "NULL", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "NULL", "1.00", INIPath);
		break;
	case 2:
		strcpy(���, "RN");
		break;
	case 3:
		strcpy(���, "����");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\����.txt", "rt");
		if (fp == NULL)
		{
			dbg("���� ���� ����!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "����", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "����", "1.00", INIPath);
		break;
	case 4:
		strcpy(���, "������ƿ");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\������ƿ.txt", "rt");
		if (fp == NULL)
		{
			dbg("���� ���� ����!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "������ƿ", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "������ƿ", "1.00", INIPath);
		break;
	case 5:
		strcpy(���, "RS");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\RS��.txt", "rt");
		if (GetPrivateProfileString("VERSION", "RS", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "RS", "1.00", INIPath);
		break;
	case 6:
		strcpy(���, "���ڸ�ũ��");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\���ڸ�ũ��.txt", "rt");
		if (fp == NULL)
		{
			dbg("���� ���� ����!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "���ڸ�ũ��", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "���ڸ�ũ��", "1.00", INIPath);
		break;
	case 7:
		strcpy(���, "����");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\����.txt", "rt");
		if (fp == NULL)
		{
			dbg("���� ���� ����!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "����", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "����", "1.00", INIPath);
		break;
	case 8:
		strcpy(���, "foo");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\foo.txt", "rt");
		if (fp == NULL)
		{
			dbg("���� ���� ����!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "foo", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "foo", "1.00", INIPath);
		break;
	case 9:
		strcpy(���, "����Ȯ��");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\����Ȯ��.txt", "rt");
		if (fp == NULL)
		{
			dbg("���� ���� ����!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "����Ȯ��", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "����Ȯ��", "1.00", INIPath);
		break;
	case 10:
		strcpy(���, "ä�ø�ũ��");
		fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\ê�ź�.txt", "rt");
		if (GetPrivateProfileString("VERSION", "ä�ø�ũ��", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "ä�ø�ũ��", "1.00", INIPath);
		break;
	case 11:
		strcpy(���, "RM");
		break;
	case -1:
		fprintf(log_fp, "opcode ����!(%s)\n", ipaddr);
		dbg("opcode ����!(%s)\n", ipaddr);
		strcpy(���, "Xopcode ����!\n");
		break;
	}


	if (sel == 2)
	{
		fprintf(log_fp, "%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s) : %s\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, data);
		dbg("%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s) : %s\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, data);
		return;
	}
	else if (sel == 11)
	{
		fprintf(log_fp, "%04u��%02u��%02u�� %s %02u��%02u��%02u�� (%s)%s\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, ipaddr, data);
		dbg("%04u��%02u��%02u�� %s %02u��%02u��%02u�� (%s)%s\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, ipaddr, data);
		return;
	}

	if (sel != 10 && sel != 5)
	{
		while (fscanf(fp, "%s %s %s", name, data2, buf) != EOF) //�̸� �ϵ��ȣ ��¥
		{

			if (strcmp(data, data2) == 0)
			{
				val = 7;
				strcpy(data, name);
				break;
			}
		}


		wsprintf(�޼���, "%04u%02u%02u", now.wYear, now.wMonth, now.wDay);

		if (val == 7 && strcmp(buf, �޼���) == -1)
			val = -3;


		if (strcmp(::version, version) == -1)
			val = -5;

		fclose(fp);
		
	}
	else
	{
		val = 8;
		while (fscanf(fp, "%s %s", name, data2) != EOF) //�̸� �ϵ��ȣ ��¥
		{
			if (strcmp(data, data2) == 0)
			{
				val = -7;
				break;
			}
		}

		if (strcmp(::version, version) == -1)
		{
			memset(name, 0, sizeof(name));
			val = -5;
		}
	}
	if (!check)
	{
		if (val == 7)
		{
			fprintf(log_fp, "%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, ���);
			dbg("%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, ���);
			strcpy(���, "O");

		}
		else if (val == 8)
		{
			fprintf(log_fp, "%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, data, ���);
			dbg("%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, data, ���);
			strcpy(���, "O");
		}
		else if (val == -3)
		{
			fprintf(log_fp, "%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n�Ⱓ ����!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, ���);
			dbg("%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n�Ⱓ ����!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, ���);
			strcpy(���, "N");
		}
		else if (val == -5)
		{
			fprintf(log_fp, "%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s - ������(%s))\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, ���, ::version);
			dbg("%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s - ������(%s))\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, ���, ::version);
			strcpy(���, "E");
		}
		else if (val == -7)
		{
			fprintf(log_fp, "%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n�Ǹ� �õ�!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, ���);
			dbg("%04u��%02u��%02u�� %s %02u��%02u��%02u�� %s(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n�Ǹ� �õ�!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, ���);
			strcpy(���, "F");
		}
		else
		{
			fprintf(log_fp, "%04u��%02u��%02u�� %s %02u��%02u��%02u�� ?(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n�ϵ���������!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, ���);
			dbg("%04u��%02u��%02u�� %s %02u��%02u��%02u�� ?(%s)(%s)���� �����ϼ̽��ϴ�.(%s)\n�ϵ���������!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "����" : "����", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, ���);
			strcpy(���, "X");
		}
	}
}




void dump(const void *data, unsigned int size)
{
	unsigned int i, j, k, z;
	char buffer[128];

	for (z = 0, i = 0; i < size; i += 16u) {
		k = 0;

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size) {
				for (; j < 16u; ++j) {
					*(unsigned int *) &buffer[k] = '    ';
					k += 3u;
				}
				break;
			}
			buffer[k + 0u] = "0123456789ABCDEF"[((unsigned char *) data)[i + j] >> 4];
			buffer[k + 1u] = "0123456789ABCDEF"[((unsigned char *) data)[i + j] & 15];
			buffer[k + 2u] = ' ';
			k += 3u;
		}

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size)
				break;
			if (z == 0) {
				if (((unsigned char *) data)[i + j] > 31u && ((unsigned char *) data)[i + j] < 127u)
					buffer[k++] = ((unsigned char *) data)[i + j];
				/* FIXME: i + j + 1u�� size���� ū ���, ���� ���� ���ڸ� memory access violation �߻� */
				else if (((((unsigned char *) data)[i + j] >= 0xB0u && ((unsigned char *) data)[i + j] <= 0xC8u) && (((unsigned char *) data)[i + j + 1u] >= 0xA1u && ((unsigned char *) data)[i + j + 1u] <= 0xFEu)) ||
					((((unsigned char *) data)[i + j] >= 0x81u && ((unsigned char *) data)[i + j] <= 0xA0u) && ((((unsigned char *) data)[i + j + 1u] >= 0x41u && ((unsigned char *) data)[i + j + 1u] <= 0x5Au) || (((unsigned char *) data)[i + j + 1u] >= 0x61u && ((unsigned char *) data)[i + j + 1u] <= 0x7Au) || (((unsigned char *) data)[i + j + 1u] >= 0x81u && ((unsigned char *) data)[i + j + 1u] <= 0xFEu))) ||
					((((unsigned char *) data)[i + j] >= 0xA1u && ((unsigned char *) data)[i + j] <= 0xC5u) && ((((unsigned char *) data)[i + j + 1u] >= 0x41u && ((unsigned char *) data)[i + j + 1u] <= 0x5Au) || (((unsigned char *) data)[i + j + 1u] >= 0x61u && ((unsigned char *) data)[i + j + 1u] <= 0x7Au) || (((unsigned char *) data)[i + j + 1u] >= 0x81u && ((unsigned char *) data)[i + j + 1u] <= 0xA0u))) ||
					(((unsigned char *) data)[i + j] == 0xC6u && (((unsigned char *) data)[i + j + 1u] >= 0x41u && ((unsigned char *) data)[i + j + 1u] <= 0x52u)) ||
					(((unsigned char *) data)[i + j] == 0xA4u && (((unsigned char *) data)[i + j + 1u] >= 0xA1u && ((unsigned char *) data)[i + j + 1u] <= 0xFEu))) {
					if (j != 15u) {
						*(unsigned short *) &buffer[k] = *(unsigned short *) &((unsigned char *) data)[i + j];
						k += 2u;
						j += 1u;
					}
					else {
						buffer[k++] = '.';
						z = 1;
					}
				}
				else
					buffer[k++] = '.';
			}
			else {
				buffer[k++] = '.';
				z = 0;
			}
		}

		
		{
			OutputDebugString((*(unsigned int *) &buffer[k] = 0, buffer));
		}
			
	}
}

unsigned int rand_mix(unsigned int a, unsigned int b)
{
	return a ^ b ^ 0xBADF00Du;
}


void main()
{
	UINT rand1 = 0, rand2 = 0;
	char aeskey[128];
	char fakekey [] = "\x63\x01\x02\x03\xB2\xBA\xCA\xFF\xCA\x13\x12\x1C\x2C\xAA\xBB\xCC\x6B\xAB\xAA\xA2\x5A\xB3\xB2\xCC\xBA\xFB\xCA\xCB\x0F\xA5\xAF\xCA";
	FILE *fp;
	FD_SET fds;
	struct timeval tv;
	char message[65536] = { 0, };
	char data[2048];
	char username[128];
	TCHAR message1[128] = { 0, };
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSz, size; //�����̸���������

	SetConsoleTitle("RF Server");

	log_fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\�α�.txt", "at");
	if (log_fp == NULL)
	{
		dbg("���� ���� ����!\n");
		getchar();
		return;
	}
	fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\aeskey.txt", "rt");
	if (fp == NULL)
	{
		dbg("���� ���� �ǻ�!\n");
		getchar();
		return;
	}
	fscanf(fp, "%s", aeskey); 
	fclose(fp);

	wsprintf(INIPath, "C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\����.ini");

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartp() error!");
	hServSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(6969);
	if (bind(hServSock, (SOCKADDR*) &servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	if (listen(hServSock, SOMAXCONN) == SOCKET_ERROR)
		ErrorHandling("listen() error!");

	clntAdrSz = sizeof(clntAdr);
	while (1)
	{
		hClntSock = accept(hServSock, (SOCKADDR*) &clntAdr, &clntAdrSz);
		if (hClntSock == INVALID_SOCKET)
			continue;
		fds.fd_count = 1;
		*fds.fd_array = hClntSock;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		if (select(0, &fds, NULL, NULL, &tv) == 1)
		{
			size = recv(hClntSock, message, sizeof(message) - 1, 0);
			if (size > 0)
			{
				RFCrypt(size, message, message);
				message[size] = '\0';
				switch (message[0])
				{
				case 'R':
					sel = 1;
					check = 0;
					break;
				case 'C':
					check = 1;
					break;
				default:
					sel = -1;
					wsprintf(message, "?");
					RFCrypt(sizeof(message), message, message);
					send(hClntSock, message, sizeof(message) - 1, 0);
					shutdown(hClntSock, SD_BOTH);
					dbg("�̻��� ���ڵ�\n");
					break;
				}
				switch (message[1])
				{ 
				case 'C': //ä�ø�ũ��
					if (sscanf(message, "%*s %s %s %s", data, username, version) == 3)
					{
						sel = 10;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						if (*message == 'O')
							wsprintf(message, "%s %s", data, aeskey); //data�� O�ְ� buf�� aeskey��
						else
							wsprintf(message, "%s %s", data, fakekey);

						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'A': //��������
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						sel = 9;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						if (*message == 'O')
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), aeskey); //data�� O�ְ� buf�� aeskey��
						else
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), fakekey);


						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'B':
					fp = fopen("C:\\Users\\��ȯ\\Desktop\\���� ���α׷�\\����\\����Ȯ��.txt", "at");
					if (fp == NULL)
					{
						dbg("���� ���� �ǻ�!\n");
						getchar();
						return;
					}
					sscanf(message, "%*s %s %s", data, username);
					fseek(fp, SEEK_END, 0);
					wsprintf(message, "%s %s %s\n", username, data, "20150915");
					fprintf(fp, message);
					fclose(fp);
					dbg("%s(%s) ����Ȯ�� ����������\n", username, data);
					shutdown(hClntSock, SD_BOTH);
					break;
				case 'F': // foo ����üũ
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						sel = 8;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(message1, data);
						
						strcpy(data, message);
						if (*message == 'O')
						{
							if (strstr(message1, "���׳�"))
							{
								*data = 'F';
								dbg("���׳���\n");
							}
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), aeskey); //data�� O�ְ� buf�� aeskey��
						}
						else
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), fakekey);
						
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;


				case 'S': // NULL ����üũ
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						if (*message == 'O')
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), aeskey); //data�� O�ְ� buf�� aeskey��
						else
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), fakekey);
						
						
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'M':
					wsprintf(data, "%s", &message[3]);
					{
						sel = 11;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'N':
					if (sscanf(message, "%*s %s %s", username, data) == 2)
					{
						sel = 2;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'D': //����
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						sel = 3;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						
						wsprintf(message, "%s %u", data, rand_mix(rand1, rand2));
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'U': //��ƿ
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						sel = 4;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						if (*message == 'O')
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), aeskey);
						else
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), fakekey);
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'T': //RS
					if (sscanf(message, "%*s %s %s %s", data, username, version) == 3)
					{
						sel = 5;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);

						
						wsprintf(message, "%s %s", data, aeskey);
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'G': //���ڸ�ũ��
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						sel = 6;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), aeskey);
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				case 'R':
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						sel = 7;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);

						strcpy(data, message);

						wsprintf(message, "%s %u", data, rand_mix(rand1, rand2));
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					else
					{
						wsprintf(message, "?");
						RFCrypt(sizeof(message), message, message);
						send(hClntSock, message, sizeof(message) - 1, 0);
						shutdown(hClntSock, SD_BOTH);
					}
					break;
				default:
					wsprintf(message, "?");
					RFCrypt(sizeof(message), message, message);
					send(hClntSock, message, sizeof(message) - 1, 0);
					shutdown(hClntSock, SD_BOTH);
					dbg("�̻��� ���ڵ�\n");
					break;
				}
			}
		}
		else
			dbg("1���̳� �޽����� ��� ���� (%s)\n", inet_ntoa(clntAdr.sin_addr));
		closesocket(hClntSock);
	}
	closesocket(hServSock);
	WSACleanup();
	fclose(log_fp);
	return;
}




void ErrorHandling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

