#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define MAX_STACK_SIZE 256
#include "pcap\Packet32.h"
#include "aes256.h"
#include "resource.h"
#include <VirtualizerSDK.h>
#include <ShlObj.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <stdbool.h>
#pragma comment(lib,"pcap\\lib\\packet.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4090)

#define WM_SOCKET (WM_USER + 1)

typedef struct
{
	int top;
	int data[MAX_STACK_SIZE];
} cstack_int_t;


typedef struct
{
	int top;
	double data[MAX_STACK_SIZE];
} cstack_double_t;


typedef struct
{
	unsigned char buffer[64];
	unsigned __int64 bytes;
	unsigned int state[5];
} sha1_t;

extern void sha1_init(sha1_t *sha1);
extern void sha1_update(sha1_t *sha1, const void *data, unsigned int size);
extern void sha1_final(sha1_t *sha1);




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
void RFCrypt(int size, const void *in, void *out);
void text_process(const char *data);
void _text_process(const char *data);
void Clipboard(char *source);
void ReadINI();
unsigned int count_digit(const char *data);
void _sethangul(char *p, unsigned int count, unsigned int sel);

/* util */
extern void dbg(const char *fmt, ...);
extern void dump(const void *data, unsigned int size);
extern unsigned int atoi32(const char *string);

/* mt19937 */
extern void init_rand(unsigned int seed);
extern unsigned int gen_rand(void);

/* wzcrypt */
extern unsigned int __fastcall wzcrypt_nextiv(unsigned int seed);
extern void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, unsigned char *in, unsigned char *out);

/* cstack */
extern cstack_int_t* cstack_alloc();
extern void cstack_free(cstack_int_t *s);
extern void cstack_push_int(cstack_int_t *s, int value);
extern void cstack_push_double(cstack_double_t *s, double value);
extern int cstack_pop_int(cstack_int_t *s);
extern double cstack_pop_double(cstack_double_t *s);
extern int cstack_peek_int(cstack_int_t *s);
extern double cstack_peek_double(cstack_double_t *s);
extern int cstack_isfull(cstack_int_t *s);
extern int cstack_isempty(cstack_int_t *s);

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
void KillProcess();
bool ConnectToServer(const char *host, int port);
void getuuid(char *data);
void MessagePump(void);
int hex2data(unsigned char *data, const unsigned char *hexstring, unsigned int len);

#pragma hdrstop