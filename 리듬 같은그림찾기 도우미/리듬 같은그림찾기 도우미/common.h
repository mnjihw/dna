#pragma warning(disable : 4090)
#define _CRT_SECURE_NO_WARNINGS
#include "pcap\Packet32.h"
#include <Windows.h>
#include <ShlObj.h>
#include "resource.h"
#include "aes256.h"
#include <TlHelp32.h>
#pragma comment(lib,"pcap\\lib\\packet.lib")


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




/* wzcrypt */
extern unsigned int __fastcall wzcrypt_nextiv(unsigned int seed);
extern void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, const void *in, void *out);





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
void packet_handler_sub(session_t *s, unsigned char *data, unsigned int size);
unsigned int packet_parse(session_t *s, unsigned char *data, unsigned int size);
void packet_handle(unsigned char *data, unsigned int size);


INT_PTR CALLBACK DlgProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
unsigned int GetMapleResolution(HWND hwnd);
INT_PTR CALLBACK TransparentDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
#pragma hdrstop