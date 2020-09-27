#define _CRT_SECURE_NO_WARNINGS
#include "pcap\Packet32.h"
#include <Windows.h>
#include "resource.h"
#include "VirtualizerSDK.h"
#include "aes256.h"
#include <ShlObj.h>
#pragma comment(lib,"pcap\\lib\\packet.lib")
char* gen_uuid(char buffer[48]);
typedef struct {
	unsigned int xs, ys;
	unsigned int data[1];
} image_t;
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

/* util */
extern void dbg(const char *fmt, ...);
extern void dump(const void *data, unsigned int size);
extern void printpacketstring(const void *data, unsigned int size);
extern unsigned int atoi32(const char *string);

/* wzcrypt */
extern unsigned int __fastcall wzcrypt_nextiv(unsigned int seed);
extern void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, const void *in, void *out);

typedef NTSTATUS(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef NTSTATUS(NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
void getuuid(char *data);
unsigned int Global_ImageSearch(image_t *image);
#pragma hdrstop