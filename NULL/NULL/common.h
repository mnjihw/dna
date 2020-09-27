#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "pcap\Packet32.h"
#include <VirtualizerSDK.h>
#include <psapi.h>
#include <fwpmu.h>
#include <ShlObj.h>
#include "resource.h"
#include "aes256.h"
#include <stdio.h>
#include <TlHelp32.h>
#include <stdbool.h>
#pragma comment(lib,"pcap\\lib\\packet.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Fwpuclnt.lib")
#pragma comment(lib,"winmm.lib")

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
void nop10();
void nop11();
void nop12();
void nop13();
void nop14();
void nop15();

typedef struct
{
	unsigned int id;
	char nick[13];
}charinfo_t;


typedef struct
{
	unsigned int id;
	unsigned int shopid;
	char nick[13];

}employeeinfo_t;



typedef struct 
{
	unsigned char buffer[64];
	unsigned __int64 bytes;
	unsigned int state[5];
} sha1_t;

extern void sha1_init(sha1_t *sha1);
extern void sha1_update(sha1_t *sha1, const void *data, unsigned int size);
extern void sha1_final(sha1_t *sha1);

#define MAX_BLOCK_XY 100
#define MAX_SEARCH_XY 100
unsigned int image_xy[MAX_SEARCH_XY];
unsigned int block_count;
unsigned int block_xy[MAX_SEARCH_XY];

typedef struct 
{
	unsigned int xs, ys;
	unsigned int data[1];
} image_t;


#pragma pack(1)
typedef struct iphdr 
{ /* 20 bytes */
	unsigned __int8 ihl : 4, version : 4, tos;
	unsigned __int16 tot_len;
	unsigned __int16 id, frag_off;
	unsigned __int8 ttl, protocol;
	unsigned __int16 check;
	unsigned __int32 saddr, daddr;
} iphdr_t;
typedef struct tcphdr 
{ /* 20 bytes */
	unsigned __int16 sport, dport;
	unsigned __int32 seq, ack_seq;
	unsigned __int8 nsf : 1, res1 : 3, doff : 4, fin : 1, syn : 1, rst : 1, psh : 1, ack : 1, urg : 1, ece : 1, cwr : 1;
	unsigned __int16 window, check, urg_ptr;
} tcphdr_t;
#pragma pack()

typedef struct session
{
	unsigned int auth, recv_iv, send_iv, ver, size;
	unsigned char data[128 * 1024];
} session_t;

ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];

char* gen_uuid(char buffer[48]);
DWORD WINAPI ImageSearchLoop(LPVOID *arg);
void RFCrypt(int size, const void *in, void *out);
image_t* get_screen(HWND hwnd);
void 일상재대기();
void 고상재대기();
void 상점개설(int shopSel);
void MouseMove(unsigned int x, unsigned int y);
void MouseClick(unsigned int x, unsigned int y, unsigned int k);
unsigned int Global_ImageSearch(image_t *image);
unsigned int Global_ImageSearch2(image_t *image);
void ReadINI();

/* mt19937 */
extern void init_rand(unsigned int seed);
extern unsigned int gen_rand(void);

/* util */
extern void dbg(const char *fmt, ...);
extern void dump(const void *data, unsigned int size);
extern unsigned int atoi32(const char *string);

/* wzcrypt */
extern unsigned int __fastcall wzcrypt_nextiv(unsigned int seed);
extern void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, const unsigned char *in, unsigned char *out);

/* crc32 */
extern unsigned int crcsum32(unsigned int crc, const void *data, unsigned int size);


unsigned int NpfFindAllDevices(void);
unsigned int NpfGetDeviceCount(void);
unsigned int __stdcall NpfGetDeviceInfo(unsigned int index, char *device_name, char *device_desc);
unsigned int __stdcall NpfSetDevice(const char *device_name);
unsigned int NpfStart(void);
void NpfStop(void);
void NpfCheckMessage(void);
void packet_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
void packet_handler_sub(session_t *s, unsigned char *data, unsigned int size);
unsigned int packet_parse(session_t *s, unsigned char *data, unsigned int size);
void packet_handle(unsigned char *data, unsigned int size);
void MessagePump(void);
void KillProcess();
void getuuid(char *data);
int hex2data(unsigned char *data, const unsigned char *hexstring, unsigned int len);
VOID CALLBACK FillNOP(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);
#pragma hdrstop