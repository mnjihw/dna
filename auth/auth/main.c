#include "common.h"


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

	VIRTUALIZER_SHARK_WHITE_START
	{
		for (unsigned int i = 0; i != size; ++i)
		((char *) out)[i] = ((char *) in)[i] ^ CryptKey[i % (sizeof(CryptKey) - 1)];
	}
		VIRTUALIZER_SHARK_WHITE_END
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

	VIRTUALIZER_SHARK_WHITE_START
	{
		hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		ioctlsocket(hSock, FIONBIO, (u_long *)"\x01\x00\x00\x00");
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = inet_addr("182.211.75.197");
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
		VIRTUALIZER_SHARK_WHITE_END
		return hSock;

}

SOCKET _ConnectToServer()
{
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	FD_SET fds;
	struct timeval tv;

	VIRTUALIZER_SHARK_WHITE_START
	{
		hSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		ioctlsocket(hSock, FIONBIO, (u_long *)"\x01\x00\x00\x00");
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = inet_addr("192.168.219.100");
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
		VIRTUALIZER_SHARK_WHITE_END
		return hSock;

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

void KillProcess()
{
	HANDLE hSnapshot, hThread;
	THREADENTRY32 te32;
	DWORD current_processid, current_threadid;

	VIRTUALIZER_SHARK_WHITE_START
	{
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



void main()
{
	char data[2048], message[2048];
	SOCKET hSock;
	int size;
	char username[128];
	DWORD username_size = sizeof(username);

	VIRTUALIZER_SHARK_WHITE_START
	{
		SetConsoleTitle("#svchost");

		if (IsUserAnAdmin() == FALSE)
		{
			dbg("관리자 권한이 아닙니다.\n관리자 권한으로 다시 실행해주세요.\n");
			getchar();
			KillProcess();
		}

		if (WSAStartup(MAKEWORD(2, 2), (LPWSADATA) data))
		{
			dbg("WSAStartup() error\n");
			getchar();
			KillProcess();
		}

		hSock = ConnectToServer();
		if (hSock == INVALID_SOCKET)
		{
			hSock = _ConnectToServer();
			if (hSock == INVALID_SOCKET)
			{
				dbg("서버 닫힘!\n");
				getchar();
				KillProcess();
			}
		}
		getuuid(data);



		GetUserName(username, &username_size);
		size = wsprintf(message, "RB %s %s", data, username);
		RFCrypt(size, message, message);

		send(hSock, message, size, 0);


		closesocket(hSock);
		dbg("완료!\n");
		getchar();
	}
		VIRTUALIZER_SHARK_WHITE_END
}