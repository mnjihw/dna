#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <fwpmu.h>
#include <ShlObj.h>
#include <stdio.h>
#include "resource.h"
#include <VirtualizerSDK.h>
#include <TlHelp32.h>
#include <stdbool.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Fwpuclnt.lib")
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

typedef struct
{
	unsigned char buffer[64];
	unsigned __int64 bytes;
	unsigned int state[5];
} sha1_t;

extern void sha1_init(sha1_t *sha1);
extern void sha1_update(sha1_t *sha1, const void *data, unsigned int size);
extern void sha1_final(sha1_t *sha1);

typedef struct
{
	unsigned int xs, ys;
	unsigned int data[1];
} image_t;


/* mt19937 */
extern void init_rand(unsigned int seed);
extern unsigned int gen_rand(void);

/* util */
extern void dbg(const char *fmt, ...);
extern void dump(const void *data, unsigned int size);
extern unsigned int atoi32(const char *string);

/* crc32 */
extern unsigned int crcsum32(unsigned int crc, const void *data, unsigned int size);


char* gen_uuid(char buffer[48]);
DWORD WINAPI ImageSearchLoop(LPVOID *arg);
void RFCrypt(int size, const void *in, void *out);
image_t* get_screen(HWND hwnd);
void 상점개설(int shopSel);
void MouseMove(int x, int y);
void MouseClick(unsigned int x, unsigned int y, unsigned int k);
unsigned int Global_ImageSearch(image_t *image);
void MessagePump(void);
void KillProcess();
void getuuid(char *data);
VOID CALLBACK FillNOP(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);
#pragma hdrstop