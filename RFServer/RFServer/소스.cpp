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
	static char CryptKey [] = "제시카ㅎㅎ";

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

void check_auth(char *data, char *ipaddr, char *결과, char *username)
{
	static SYSTEMTIME now, today, st;
	int val = 0;
	static int count;
	char name[128] = { 0, }, buf[128];
	char version[8];
	char data2[2048];
	char 메세지[1024];
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
		strcpy(결과, "NULL");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\인증.txt", "rt");
		if (fp == NULL)
		{
			dbg("파일 오픈 실패!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "NULL", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "NULL", "1.00", INIPath);
		break;
	case 2:
		strcpy(결과, "RN");
		break;
	case 3:
		strcpy(결과, "엔딜");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\엔딜.txt", "rt");
		if (fp == NULL)
		{
			dbg("파일 오픈 실패!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "엔딜", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "엔딜", "1.00", INIPath);
		break;
	case 4:
		strcpy(결과, "상점유틸");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\상점유틸.txt", "rt");
		if (fp == NULL)
		{
			dbg("파일 오픈 실패!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "상점유틸", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "상점유틸", "1.00", INIPath);
		break;
	case 5:
		strcpy(결과, "RS");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\RS블랙.txt", "rt");
		if (GetPrivateProfileString("VERSION", "RS", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "RS", "1.00", INIPath);
		break;
	case 6:
		strcpy(결과, "도박매크로");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\도박매크로.txt", "rt");
		if (fp == NULL)
		{
			dbg("파일 오픈 실패!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "도박매크로", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "도박매크로", "1.00", INIPath);
		break;
	case 7:
		strcpy(결과, "리상");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\리상.txt", "rt");
		if (fp == NULL)
		{
			dbg("파일 오픈 실패!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "리상", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "리상", "1.00", INIPath);
		break;
	case 8:
		strcpy(결과, "foo");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\foo.txt", "rt");
		if (fp == NULL)
		{
			dbg("파일 오픈 실패!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "foo", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "foo", "1.00", INIPath);
		break;
	case 9:
		strcpy(결과, "쪽지확인");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\쪽지확인.txt", "rt");
		if (fp == NULL)
		{
			dbg("파일 오픈 실패!\n");
			getchar();
			exit(1);
		}
		if (GetPrivateProfileString("VERSION", "쪽지확인", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "쪽지확인", "1.00", INIPath);
		break;
	case 10:
		strcpy(결과, "채팅매크로");
		fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\챗매블랙.txt", "rt");
		if (GetPrivateProfileString("VERSION", "채팅매크로", "", version, sizeof(version) - 1, INIPath) == 0)
			WritePrivateProfileString("VERSION", "채팅매크로", "1.00", INIPath);
		break;
	case 11:
		strcpy(결과, "RM");
		break;
	case -1:
		fprintf(log_fp, "opcode 에러!(%s)\n", ipaddr);
		dbg("opcode 에러!(%s)\n", ipaddr);
		strcpy(결과, "Xopcode 에러!\n");
		break;
	}


	if (sel == 2)
	{
		fprintf(log_fp, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s) : %s\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, data);
		dbg("%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s) : %s\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, data);
		return;
	}
	else if (sel == 11)
	{
		fprintf(log_fp, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 (%s)%s\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, ipaddr, data);
		dbg("%04u년%02u월%02u일 %s %02u시%02u분%02u초 (%s)%s\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, ipaddr, data);
		return;
	}

	if (sel != 10 && sel != 5)
	{
		while (fscanf(fp, "%s %s %s", name, data2, buf) != EOF) //이름 하드번호 날짜
		{

			if (strcmp(data, data2) == 0)
			{
				val = 7;
				strcpy(data, name);
				break;
			}
		}


		wsprintf(메세지, "%04u%02u%02u", now.wYear, now.wMonth, now.wDay);

		if (val == 7 && strcmp(buf, 메세지) == -1)
			val = -3;


		if (strcmp(::version, version) == -1)
			val = -5;

		fclose(fp);
		
	}
	else
	{
		val = 8;
		while (fscanf(fp, "%s %s", name, data2) != EOF) //이름 하드번호 날짜
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
			fprintf(log_fp, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s)\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, 결과);
			dbg("%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s)\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, 결과);
			strcpy(결과, "O");

		}
		else if (val == 8)
		{
			fprintf(log_fp, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s)\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, data, 결과);
			dbg("%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s)\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, data, 결과);
			strcpy(결과, "O");
		}
		else if (val == -3)
		{
			fprintf(log_fp, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s)\n기간 만료!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, 결과);
			dbg("%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s)\n기간 만료!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, 결과);
			strcpy(결과, "N");
		}
		else if (val == -5)
		{
			fprintf(log_fp, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s - 구버전(%s))\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, 결과, ::version);
			dbg("%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s - 구버전(%s))\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, 결과, ::version);
			strcpy(결과, "E");
		}
		else if (val == -7)
		{
			fprintf(log_fp, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s)\n판매 시도!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, 결과);
			dbg("%04u년%02u월%02u일 %s %02u시%02u분%02u초 %s(%s)(%s)님이 접속하셨습니다.(%s)\n판매 시도!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, name, username, ipaddr, 결과);
			strcpy(결과, "F");
		}
		else
		{
			fprintf(log_fp, "%04u년%02u월%02u일 %s %02u시%02u분%02u초 ?(%s)(%s)님이 접속하셨습니다.(%s)\n하드인증실패!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, 결과);
			dbg("%04u년%02u월%02u일 %s %02u시%02u분%02u초 ?(%s)(%s)님이 접속하셨습니다.(%s)\n하드인증실패!\n", now.wYear, now.wMonth, now.wDay, (now.wHour >= 12) ? "오후" : "오전", (now.wHour > 12) ? now.wHour - 12 : now.wHour, now.wMinute, now.wSecond, username, ipaddr, 결과);
			strcpy(결과, "X");
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
				/* FIXME: i + j + 1u가 size보다 큰 경우, 운이 아주 나쁘면 memory access violation 발생 */
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
	int clntAdrSz, size; //변수이름조갓네툐

	SetConsoleTitle("RF Server");

	log_fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\로그.txt", "at");
	if (log_fp == NULL)
	{
		dbg("파일 오픈 실패!\n");
		getchar();
		return;
	}
	fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\aeskey.txt", "rt");
	if (fp == NULL)
	{
		dbg("파일 오픈 실퍠!\n");
		getchar();
		return;
	}
	fscanf(fp, "%s", aeskey); 
	fclose(fp);

	wsprintf(INIPath, "C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\버전.ini");

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
					dbg("이상한 옵코드\n");
					break;
				}
				switch (message[1])
				{ 
				case 'C': //채팅매크로
					if (sscanf(message, "%*s %s %s %s", data, username, version) == 3)
					{
						sel = 10;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						if (*message == 'O')
							wsprintf(message, "%s %s", data, aeskey); //data에 O있고 buf에 aeskey있
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
				case 'A': //쪽지보기
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						sel = 9;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						if (*message == 'O')
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), aeskey); //data에 O있고 buf에 aeskey있
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
					fp = fopen("C:\\Users\\지환\\Desktop\\리듬 프로그램\\서버\\쪽지확인.txt", "at");
					if (fp == NULL)
					{
						dbg("파일 오픈 실퍠!\n");
						getchar();
						return;
					}
					sscanf(message, "%*s %s %s", data, username);
					fseek(fp, SEEK_END, 0);
					wsprintf(message, "%s %s %s\n", username, data, "20150915");
					fprintf(fp, message);
					fclose(fp);
					dbg("%s(%s) 쪽지확인 인증해줬음\n", username, data);
					shutdown(hClntSock, SD_BOTH);
					break;
				case 'F': // foo 인증체크
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						sel = 8;
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(message1, data);
						
						strcpy(data, message);
						if (*message == 'O')
						{
							if (strstr(message1, "아테네"))
							{
								*data = 'F';
								dbg("아테네임\n");
							}
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), aeskey); //data에 O있고 buf에 aeskey있
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


				case 'S': // NULL 인증체크
					if (sscanf(message, "%*s %u %u %s %s %s", &rand1, &rand2, data, username, version) == 5)
					{
						check_auth(data, inet_ntoa(clntAdr.sin_addr), message, username);
						strcpy(data, message);
						if (*message == 'O')
							wsprintf(message, "%s %u %s", data, rand_mix(rand1, rand2), aeskey); //data에 O있고 buf에 aeskey있
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
				case 'D': //엔딜
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
				case 'U': //유틸
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
				case 'G': //도박매크로
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
					dbg("이상한 옵코드\n");
					break;
				}
			}
		}
		else
			dbg("1초이내 메시지가 없어서 끊음 (%s)\n", inet_ntoa(clntAdr.sin_addr));
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

