#include "common.h"

// 이거풀면 dbgwindow 에 ㄱ패킷찍김
#define PACKET_DUMP

unsigned int secure_value_C;
unsigned int lan_mode, proxy_mode;
SOCKET socket_handle;
static cque_t *recv_q, *send_q;
static unsigned int send_iv, recv_iv, send_flag, packet_size;
static unsigned char packet_data[262144];

unsigned int recv_handlersub(unsigned char *packet_data, unsigned int packet_size)
{
	unsigned int i, j, k;
#ifdef PACKET_DUMP
	unsigned char data[65536];
#else
	unsigned char data[16];
#endif

	k = recv_iv;

	for (i = 0; packet_size >= i + (j = (*(unsigned int *)&packet_data[i] >> 16) ^ (*(unsigned int *)&packet_data[i] & 65535)) + 4u; i += j + 4u) {
#ifdef PACKET_DUMP
		wzcrypt_decrypt(j, k, &packet_data[i + 4u], data);
		dbg("-------- S->C [0x%02X] %u bytes", *(unsigned short *)data, j);
		dump(data, j);
#else
		wzcrypt_decrypt(7, k, &packet_data[i + 4u], data);
#endif
		// data에 받은 패킷 들어있음
		if (*(unsigned short *)data == R_USER_MINIROOM_BALLON) {
			if (data[6] == 0)
				OnShopClose(0, *(unsigned int *)&data[2]);
		} else if (*(unsigned short *)data == R_EMPLOYEE_LEAVE)
			OnShopClose(1, *(unsigned int *)&data[2]);
		else if (*(unsigned short *)data == PACKET_DENY[0] ||
			*(unsigned short *)data == PACKET_DENY[1] ||
			*(unsigned short *)data == PACKET_DENY[2] ||
			*(unsigned short *)data == PACKET_DENY[3] ||
			*(unsigned short *)data == PACKET_DENY[4] ||
			*(unsigned short *)data == PACKET_DENY[5] ||
			*(unsigned short *)data == PACKET_DENY[6] ||
			*(unsigned short *)data == PACKET_DENY[7] ||
			*(unsigned short *)data == PACKET_DENY[8] ||
			*(unsigned short *)data == PACKET_DENY[9]) {
			*(unsigned short *)&packet_data[i + 4u] = 0xFFFF;
			wzcrypt_encrypt(2, k, &packet_data[i + 4u], &packet_data[i + 4u]);
		}
		k = wzcrypt_nextiv(k);
	}

	recv_iv = k;
	return i;
}

void __stdcall recv_handler(DWORD err, DWORD bytes, WSAOVERLAPPED *ol, DWORD flags)
{
	unsigned int i;
	WSABUF wsabuf;

	if (socket_handle != INVALID_SOCKET)
		if (err == 0 && bytes) {
			if ((i = recv_handlersub(packet_data, packet_size += bytes)) != 0) {
				if (cque_write(recv_q, packet_data, i) == 0) {
					VIRTUALIZER1_START
					{
						SECURE_API(s_PostMessageW)(MainWnd, WM_SOCKET, socket_handle, FD_CLOSE);
					}
					VIRTUALIZER_END
					return;
				}
				SECURE_API(s_PostMessageW)(MainWnd, WM_SOCKET, socket_handle, FD_READ);
			}
			if (packet_size -= i)
				memcpy(packet_data, &packet_data[i], packet_size);
			if (SECURE_API(s_WSARecv)(socket_handle, (wsabuf.buf = (char *)&packet_data[packet_size], wsabuf.len = sizeof(packet_data) - packet_size, &wsabuf), 1, NULL, &flags, ol, recv_handler))
				if (SECURE_API(s_WSAGetLastError)() != WSA_IO_PENDING) {
					VIRTUALIZER1_START
					{
						SECURE_API(s_PostMessageW)(MainWnd, WM_SOCKET, socket_handle, FD_CLOSE);
					}
					VIRTUALIZER_END
				}
		} else {
			VIRTUALIZER1_START
			{
				SECURE_API(s_PostMessageW)(MainWnd, WM_SOCKET, socket_handle, FD_CLOSE);
			}
			VIRTUALIZER_END
		}
}

void __stdcall send_handler(DWORD err, DWORD bytes, WSAOVERLAPPED *ol, DWORD flags)
{
	unsigned int i;
	WSABUF wsabuf[2];

	if (socket_handle != INVALID_SOCKET)
		if (err == 0 && bytes) {
			if (cque_send_ok(send_q, bytes) && lan_mode == 0 && (i = cque_send_prepare(send_q, wsabuf)) != 0) {
				if (SECURE_API(s_WSASend)(socket_handle, wsabuf, i, NULL, 0, ol, send_handler))
					if (SECURE_API(s_WSAGetLastError)() != WSA_IO_PENDING) {
						VIRTUALIZER1_START
						{
							SECURE_API(s_PostMessageW)(MainWnd, WM_SOCKET, socket_handle, FD_CLOSE);
						}
						VIRTUALIZER_END
						return;
					}
				send_flag = 1;
			} else
				send_flag = 0;
		} else {
			VIRTUALIZER1_START
			{
				SECURE_API(s_PostMessageW)(MainWnd, WM_SOCKET, socket_handle, FD_CLOSE);
			}
			VIRTUALIZER_END
		}
}

void send_flush(void)
{
	static WSAOVERLAPPED ol;
	unsigned int i;
	WSABUF wsabuf[2];

	if (lan_mode == 0 && send_flag == 0 && socket_handle != INVALID_SOCKET && (i = cque_send_prepare(send_q, wsabuf)) != 0) {
		if (SECURE_API(s_WSASend)(socket_handle, wsabuf, i, NULL, 0, &ol, send_handler))
			if (SECURE_API(s_WSAGetLastError)() != WSA_IO_PENDING) {
				VIRTUALIZER1_START
				{
					SECURE_API(s_PostMessageW)(MainWnd, WM_SOCKET, socket_handle, FD_CLOSE);
				}
				VIRTUALIZER_END
				return;
			}
		send_flag = 1;
	}

	SECURE_API(s_SleepEx)(0, 1);
}

int __stdcall _closesocket(SOCKET s)
{
	int i;

	VIRTUALIZER1_START
	{	
		if (s == socket_handle) {
			socket_handle = INVALID_SOCKET;
			i = SECURE_API(s_closesocket)(s);
			SECURE_API(s_SleepEx)(16, 1);
			cque_clear(recv_q);
			cque_clear(send_q);
			send_flag = 0;
			packet_size = 0;
		} else
			i = SECURE_API(s_closesocket)(s);
	}
	VIRTUALIZER_END

	return i;
}

// 메이플 코드 상 recv 호출 후 10035가 떨어지면 Sleep(500)을 하게 되는 데
// connect를 하면서 미리 recv까지 해주면 Sleep(500)을 부를 일이 없게 됨

unsigned int check_auth(void)
{
	char data[64];

	VIRTUALIZER2_START
	{
		*(unsigned int *)data = 0;
		if (*gen_uuid(data)) {
			((int *)data)[0] = ~_byteswap_ulong(((int *)data)[0]);
			((int *)data)[1] = ~_byteswap_ulong(((int *)data)[1]);
			((int *)data)[2] = ~_byteswap_ulong(((int *)data)[2]);
			((int *)data)[3] = ~_byteswap_ulong(((int *)data)[3]);
			((int *)data)[4] = ~_byteswap_ulong(((int *)data)[4]);
			((int *)data)[5] = ~_byteswap_ulong(((int *)data)[5]);
			((int *)data)[6] = ~_byteswap_ulong(((int *)data)[6]);
			((int *)data)[7] = ~_byteswap_ulong(((int *)data)[7]);
			((int *)data)[8] = ~_byteswap_ulong(((int *)data)[8]);
			((int *)data)[9] = ~_byteswap_ulong(((int *)data)[9]);
			*(unsigned int *)data = crcsum32(0xBADF00Du, data, 40u);
		}
	}
	VIRTUALIZER_END

	return *(unsigned int *)data;
}

int __stdcall _connect(SOCKET s, struct sockaddr *name, int namelen)
{
	static WSAOVERLAPPED ol;
	unsigned int i, j, bytes, flags;
	WSABUF wsabuf;
	struct timeval tv;
	unsigned char data[1024];

	VIRTUALIZER1_START
	{
		if (check_auth() == secure_value_UUID && namelen == sizeof(struct sockaddr_in) && socket_handle == INVALID_SOCKET && send_flag == 0) {
			if (SECURE_API(s_setsockopt)(s, IPPROTO_TCP, TCP_NODELAY, "\x01\x00\x00", 4) == 0 &&
				SECURE_API(s_WSAAsyncSelect)(s, MainWnd, 0, 0) == 0 &&
				SECURE_API(s_ioctlsocket)(s, FIONBIO, (u_long *)"\x01\x00\x00") == 0) {
				if (proxy_mode) {
					((struct sockaddr_in *)data)->sin_family = AF_INET;
					((struct sockaddr_in *)data)->sin_port = _byteswap_ushort(4848);
					((struct sockaddr_in *)data)->sin_addr.s_addr = SECURE_API(s_inet_addr)("127.0.0.1");
					i = SECURE_API(s_connect)(s, (struct sockaddr_in *)data, sizeof(struct sockaddr_in));
				} else
					i = SECURE_API(s_connect)(s, name, namelen);
				if ((i == 0 || SECURE_API(s_WSAGetLastError)() == WSAEWOULDBLOCK) &&
					SECURE_API(s_select)(1, NULL, (((fd_set *)data)->fd_count = 1, *((fd_set *)data)->fd_array = s, (fd_set *)data), NULL, (tv.tv_sec = 0, tv.tv_usec = 500u * 1000u, &tv)) == 1 &&
					SECURE_API(s_ioctlsocket)(s, FIONBIO, (u_long *)"\x00\x00\x00") == 0) {
					if (proxy_mode && SECURE_API(s_WSASend)(s, (wsabuf.len = 16, wsabuf.buf = (char *)name, &wsabuf), 1, NULL, 0, (ol.hEvent = NULL, &ol), NULL))
						if (SECURE_API(s_WSAGetLastError)() != WSA_IO_PENDING || SECURE_API(s_WSAGetOverlappedResult)(s, &ol, &flags, TRUE, &flags) == 0)
							goto $err;
					i = 0;
					do {
						if (SECURE_API(s_WSARecv)(s, (wsabuf.buf = (char *)&data[i], wsabuf.len = 1, &wsabuf), 1, &bytes, (flags = 0, &flags), &ol, NULL))
							if (SECURE_API(s_WSAGetLastError)() != WSA_IO_PENDING || SECURE_API(s_WSAGetOverlappedResult)(s, &ol, &bytes, TRUE, &flags) == 0)
								goto $err;
						if (bytes == 0)
							goto $err;
					} while ((j = *(unsigned short *)data) + 2u != (i += bytes));
					j = *(unsigned short *)&data[4];
					send_iv = *(unsigned int *)&data[j + 6u];
					recv_iv = *(unsigned int *)&data[j + 10u];
					if (cque_write(recv_q, data, i) == 0)
						goto $err;
					if (SECURE_API(s_WSARecv)(s, (wsabuf.buf = (char *)packet_data, wsabuf.len = sizeof(packet_data), &wsabuf), 1, NULL, (flags = 0, &flags), &ol, recv_handler))
						if (SECURE_API(s_WSAGetLastError)() != WSA_IO_PENDING)
							goto $err;
					socket_handle = s;
					SECURE_API(s_PostMessageW)(MainWnd, WM_SOCKET, s, FD_CONNECT);
					SECURE_API(s_WSASetLastError)(WSAEWOULDBLOCK);
				}
			}
		} else
$err:		SECURE_API(s_WSASetLastError)(WSAEINVAL);
	}
	VIRTUALIZER_END

	return -1;
}

int __stdcall _recv(SOCKET s, void *buf, int len, int flags)
{
	unsigned int i;

	if (lan_mode == 0 && (i = cque_read(recv_q, buf, (unsigned int)len)) != 0)
		return (int)i;

	SECURE_API(s_SleepEx)(0, 1);
	return 0; /* 메이플 코드 상 WSAEWOULDBLOCK && -1 을 하지 않아도 된다. */
}

int __stdcall _send(SOCKET s, const void *buf, int len, int flags)
{
#ifdef PACKET_DUMP
	unsigned int i, j, k;
	unsigned char data[65536];
#endif

#ifdef PACKET_DUMP
	k = send_iv;

	for (i = 0; (unsigned int)len >= i + (j = (*(unsigned int *)&((unsigned char *)buf)[i] >> 16) ^ (*(unsigned int *)&((unsigned char *)buf)[i] & 0xFFFF)) + 4u; i += j + 4u) {
		wzcrypt_decrypt(j, k, &((unsigned char *)buf)[i + 4u], data);
		dbg("-------- C->S [0x%02X] %u bytes", *(unsigned short *)data, j);
		// data에 보낼 패킷 들어있음
		dump(data, j);
		k = wzcrypt_nextiv(k);
	}

	send_iv = k;
#endif

	if (cque_write(send_q, buf, (unsigned int)len) == 0) {
		VIRTUALIZER1_START
		{
			SECURE_API(s_WSASetLastError)(WSA_NOT_ENOUGH_MEMORY);
		}
		VIRTUALIZER_END
		return -1;
	}

	send_flush();
	return len;
}

void socket_hook(void)
{
	unsigned int i;
	PROC fn;

	VIRTUALIZER3_START
	{
		if ((secure_value_C ^ (init_rand(SECURE_API(s_GetCurrentProcessId)() ^ 0xBADF00Du), gen_rand() ^ gen_rand())) == 0x17199204u) {
			wzcrypt_setup();
			socket_handle = INVALID_SOCKET;
			if ((recv_q = cque_alloc(1048576u)) != NULL && (send_q = cque_alloc(1048576u)) != NULL) {
				for (fn = SECURE_API(s_closesocket); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)_closesocket);
				for (fn = SECURE_API(s_connect); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)_connect);
				for (fn = SECURE_API(s_recv); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)_recv);
				for (fn = SECURE_API(s_send); (i = (unsigned int)memscan_ptr(client_base, &fn, 4)) != 0; *(PROC *)i = (PROC)_send);
			} else
				SECURE_API(s_ExitProcess)(0);
		}
	}
	VIRTUALIZER_END
}