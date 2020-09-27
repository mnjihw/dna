#include "pcap\Packet32.h"
#include <Windows.h>
#pragma comment(lib,"pcap\\lib\\packet.lib")
#include "resource.h"
#pragma comment(lib,"winmm.lib")

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
extern void __fastcall wzcrypt_encrypt(unsigned int size, unsigned int iv, const void *in, void *out);
extern void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, const void *in, void *out);

#pragma hdrstop