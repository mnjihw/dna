#pragma warning(disable : 4091)
#pragma warning(disable : 4244)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "pcap\Packet32.h"
#include <CommCtrl.h>
#include <WinSock2.h>
#include "resource.h"
#include <VirtualizerSDK.h>
#include "aes256.h"
#include <TlHelp32.h>
#include <stdio.h>
#include <ShlObj.h>
#include <stdbool.h>
#pragma comment(lib,"pcap\\lib\\packet.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Comctl32.lib")


#define WM_SOCKET (WM_USER + 1)
#define NOP_SIZE 1024
#define _FILL_NOP(nop, size) nop##size
#define FILL_NOP(size) _FILL_NOP(NOP, size)
#define NOP __asm{nop};
#define NOP2 NOP NOP
#define NOP4 NOP2 NOP2
#define NOP8 NOP4 NOP4
#define NOP16 NOP8 NOP8
#define NOP32 NOP16 NOP16
#define NOP64 NOP32 NOP32
#define NOP128 NOP64 NOP64
#define NOP256 NOP128 NOP128
#define NOP512 NOP256 NOP256
#define NOP1024 NOP512 NOP512
void nop5();
void nop6();
void nop7();
void nop8();


typedef struct session
{
	unsigned int auth, recv_iv, send_iv, ver, size;
	unsigned char data[128 * 1024];
} session_t;

typedef struct charinfo
{
	unsigned int id;
	char nick[13];
}charinfo_t;

typedef struct server
{
	char name[16];
	char value;
}server_t;


typedef struct {
	unsigned char buffer[64];
	unsigned __int64 bytes;
	unsigned int state[5];
} sha1_t;

#pragma pack(1)
typedef struct iphdr { /* 20 bytes */
	unsigned __int8 ihl : 4, version : 4, tos;
	unsigned __int16 tot_len;
	unsigned __int16 id, frag_off;
	unsigned __int8 ttl, protocol;
	unsigned __int16 check;
	unsigned __int32 saddr, daddr;
} iphdr_t;
typedef struct tcphdr { /* 20 bytes */
	unsigned __int16 sport, dport;
	unsigned __int32 seq, ack_seq;
	unsigned __int8 nsf : 1, res1 : 3, doff : 4, fin : 1, syn : 1, rst : 1, psh : 1, ack : 1, urg : 1, ece : 1, cwr : 1;
	unsigned __int16 window, check, urg_ptr;
} tcphdr_t;
#pragma pack()


ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];


/* util */
extern void dbg(const char *fmt, ...);
extern void dump(const void *data, unsigned int size);
extern unsigned int atoi32(const char *string);

extern void sha1_init(sha1_t *sha1);
extern void sha1_update(sha1_t *sha1, const void *data, unsigned int size);
extern void sha1_final(sha1_t *sha1);

/* mt19937 */
extern void init_rand(unsigned int seed);
extern unsigned int gen_rand(void);

unsigned int NpfFindAllDevices(void);
unsigned int NpfGetDeviceCount(void);
unsigned int __stdcall NpfGetDeviceInfo(unsigned int index, char *device_name, char *device_desc);
unsigned int __stdcall NpfSetDevice(const char *device_name);
unsigned int NpfStart(void);
void NpfStop(void);
void NpfCheckMessage(void);

/* wzcrypt */
extern unsigned int __fastcall wzcrypt_nextiv(unsigned int seed);
extern void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, unsigned char *in, unsigned char *out);

void RFCrypt(int size, const void *in, void *out);
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
void packet_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
char* gen_uuid(char buffer[48]);
void MessagePump(void);
void MessageLoop();
void SendData(const char *fmt, ...);
#pragma hdrstop