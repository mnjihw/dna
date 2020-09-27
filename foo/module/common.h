#pragma warning(disable : 4090)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <psapi.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <fwpmu.h>
#include <ntddndis.h>
#include <VirtualizerSDK.h>
#include <TlHelp32.h>
#include <stdbool.h>
#include "resource.h"
#include "aes256.h"
#include "pcap\Packet32.h"
#pragma comment(lib,"pcap\\lib\\packet.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Fwpuclnt.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib, "winmm.lib")

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
void nop11();
void nop12();
void nop13();


typedef struct 
{
	int v, c;
} pos_t;

typedef struct
{
	char nick[13];
	unsigned char data[512];
	unsigned int size;
	unsigned int id;
}warp_t;

typedef struct server
{
	char name[16];
	char value;
}server_t;

typedef struct
{
	unsigned int id;
	unsigned int shopid;
	char nick[13];
}employeeinfo_t;

typedef struct
{
	unsigned int id;
	char nick[13];
}charinfo_t;


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
	unsigned int auth, ver, size;
	unsigned char data[128 * 1024];
} session_t;



ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];

typedef int(__stdcall *PFCONNECT)(SOCKET, const struct sockaddr*, int);
PFCONNECT g_pfnOldconnect;
typedef ATOM(__stdcall *PFREGISTERCLASSEXA)(WNDCLASSEX*);
PFREGISTERCLASSEXA g_pfnOldRegisterClassExA;



#define RVA2OFFSET(TYPE, BASEADDR, RVA) ((TYPE)((DWORD)(BASEADDR) + (DWORD)(RVA)))
#define MAKEPTR RVA2OFFSET


typedef struct 
{
	unsigned int size;
	unsigned char data[60];
} packet_t;



/* main */
extern void *client_base, *module_base;
extern char client_path[1024], module_path[1024];


/* util */
extern void dbg(const char *fmt, ...);
extern void dump(const void *data, unsigned int size);
extern void* GetModule(unsigned int hash);
extern PROC GetExportAddress(void *module, const char *name);
extern void HookJMP(void *addr, void *fn, void *code);
extern void vmemcpy(void *addr, const void *data, unsigned int size);
extern void* _memscan(const void *image_data, unsigned int image_size, const void *pattern_data, unsigned int pattern_size);
extern void* _memscan_ptr(const void *image_data, unsigned int image_size, const void *pattern_data, unsigned int pattern_size);
extern void* memscan(void *image_base, const void *pattern_data, unsigned int pattern_size);
extern void* memscan_ptr(void *image_base, const void *pattern_data, unsigned int pattern_size);
extern char* kstrstr(const char *text, const char *pattern);
extern char* kstristr(const char *text, const char *pattern);
extern void* _memmove(void *d, const void *s, unsigned int c);
extern unsigned int _wcscpy(void *dst, const void *src);
BOOL HookExportedProc(HMODULE hModule, LPCSTR lpProcName, LPVOID* ppOldProcAddress, LPVOID pNewProcAddress);

/* wzcrypt */
extern unsigned int __fastcall wzcrypt_nextiv(unsigned int seed);
extern void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, unsigned char *in, unsigned char *out);



/* SHA1 */
typedef struct {
	unsigned char buffer[64];
	unsigned __int64 bytes;
	unsigned int state[5];
} sha1_t;
extern void sha1_init(sha1_t *sha1);
extern void sha1_update(sha1_t *sha1, const void *data, unsigned int size);
extern void sha1_final(sha1_t *sha1);
extern char* sha1_tostring(sha1_t *sha1, char *string);
extern char* sha1(char *string, const void *data, unsigned int size);

/* uuid */
extern char* gen_uuid(char buffer[48]);


/* mt19937 */
extern unsigned int gen_rand(void);
extern void init_rand(unsigned int seed);

unsigned int NpfFindAllDevices(void);
unsigned int NpfGetDeviceCount(void);
unsigned int __stdcall NpfGetDeviceInfo(unsigned int index, char *device_name, char *device_desc);
unsigned int __stdcall NpfSetDevice(const char *device_name);
unsigned int NpfStart(void);
void NpfStop(void);
void NpfCheckMessage(void);
void packet_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
void packet_handler_sub(session_t *s, unsigned char *data, unsigned int size, unsigned int inbound);
unsigned int packet_parse(session_t *s, unsigned char *data, unsigned int size, unsigned int inbound);
void packet_handle(unsigned char *data, unsigned int size);


INT_PTR CALLBACK DlgProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
unsigned int atoi32(const char *string);
INT_PTR CALLBACK DlgPacketProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void 상점개설();
void MouseClick(unsigned int x, unsigned int y, unsigned int k);
void MouseMove(unsigned int x, unsigned int y);
unsigned int GetMapleResolution(HWND hwnd);
void client_loading_done();
void Clipboard(char *source);
void 일고랜();
void CALLBACK 와리(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
unsigned int crcsum32(unsigned int crc, const void *data, unsigned int size);
INT_PTR CALLBACK SnifferDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
void DisplayPacket(const void *data, unsigned int size, HWND hwnd, unsigned int idx);
void RFCrypt(int size, const void *in, void *out);
void getuuid(char *data);
void send_packet(const void *data, unsigned int size);
void* thiscall1(void *f, void *a1, void *a2);
void* thiscall2(void *f, void *a1, void *a2, void *a3);
int hex2data(unsigned char *data, const unsigned char *hexstring, unsigned int len);
bool ConnectToServer(const char *host, int port);
void SendData(const char *fmt, ...);
VOID CALLBACK FillNOP(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);
#pragma hdrstop