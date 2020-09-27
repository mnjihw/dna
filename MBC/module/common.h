#pragma warning(disable : 4054) /* 'type cast' : from function pointer '' to data pointer '' */
#pragma warning(disable : 4055) /* 'type cast' : from data pointer '' to function pointer '' */
#pragma warning(disable : 4100) /* '' : unreferenced formal parameter */
#pragma warning(disable : 4152) /* nonstandard extension, function/data pointer conversion in expression */
#pragma warning(disable : 4305) /* 'type cast' : truncation from '' to '' */
#pragma warning(disable : 4505) /* '' : unreferenced local function has been removed */

#define STRICT
#define _CRT_SECURE_NO_WARNINGS
#define VC_EXTRALEAN
#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <psapi.h>
#include "resource.h"
#include "VirtualizerSDK.h"

/* SOCKET */
#define WM_SOCKET (WM_USER + 1u)

/* SEND PART */
//#define S_MINIROOM 0xEBu
//#define S_MINIROOM_CREATE 0x10u
//#define S_MINIROOM_ENTER 0x13u
//#define S_MINIROOM_START2 0x1Au
//#define S_MINIROOM_LEAVE 0x1Cu
//#define S_MINIROOM_ADDITEM 0x3Cu
//#define S_MINIROOM_START1 0x50u

/* RECV PART */
//#define R_USER_MINIROOM_BALLON 0xFBu
//#define R_EMPLOYEE_LEAVE 0x1D7u

extern unsigned int S_MINIROOM, S_MINIROOM_CREATE, S_MINIROOM_ENTER, S_MINIROOM_START2, S_MINIROOM_LEAVE, S_MINIROOM_ADDITEM, S_MINIROOM_START1, R_USER_MINIROOM_BALLON, R_EMPLOYEE_LEAVE, PACKET_DENY[10];

//#define _byteswap_ushort(x) ((((x) << 8) | ((x) >> 8)) & 0xFFFF)
//#define _byteswap_ulong(x) (((x) << 24) | ((x) >> 24) | (((x) & 0xFF0000) >> 8) | (((x) & 0xFF00) << 8))

static __declspec(naked) unsigned short __fastcall byteswap_ushort(unsigned short value) {
	__asm {
		mov eax, ecx
		bswap eax
		shr eax, 10h
		retn
	}
}

static __declspec(naked) unsigned int __fastcall byteswap_ulong(unsigned int value) {
	__asm {
		mov eax, ecx
		bswap eax
		retn
	}
}

/* main */
extern unsigned int secure_value_UUID, secure_value_S;
extern void *client_base, *module_base;
extern char client_path[1024], module_path[1024];

/* client */
extern unsigned int secure_value_A;
extern HWND MainWnd;
extern WNDPROC MainWndProc;
extern void OnShopClose(unsigned int type, unsigned int id);
extern void client_hook(void);

/* hshield */
extern unsigned int secure_value_B;
extern void hshield_hook(void);

/* cque */
typedef struct cque {
	CRITICAL_SECTION cs;
	unsigned int size[2]; /* [0]=size [1]=maxsize */
	unsigned int pos[2]; /* [0]=read [1]=write */
	unsigned char data[64]; /* minimum 64 bytes (for acceptex) */
} cque_t;
extern cque_t* cque_alloc(unsigned int size);
extern void cque_free(cque_t *cque);
extern void cque_clear(cque_t *cque);
extern unsigned int cque_peek(cque_t *cque, void *data, unsigned int size);
extern unsigned int cque_read(cque_t *cque, void *data, unsigned int size);
extern unsigned int cque_write(cque_t *cque, const void *data, unsigned int size);
extern unsigned int cque_recv_prepare(cque_t *cque, WSABUF *wsabuf);
extern unsigned int cque_recv_ok(cque_t *cque, unsigned int size);
extern unsigned int cque_send_prepare(cque_t *cque, WSABUF *wsabuf);
extern unsigned int cque_send_ok(cque_t *cque, unsigned int size);

/* socket */
extern unsigned int secure_value_C;
extern unsigned int lan_mode, proxy_mode;
extern SOCKET socket_handle;
extern void send_flush(void);
extern void socket_hook(void);

/* util */
extern void dbg(const char *fmt, ...);
extern void dump(const void *data, unsigned int size);
extern void* GetModule(unsigned int hash);
extern void EnumModuleHash(void);
extern void GetExportDesc(void *addr);
extern PROC GetExportAddress(void *module, const char *name);
extern DWORD* GetExportAddressBase(void *module, const char *name);
extern PROC HookExportProc(void *module, const char *name, void *proc);
extern PROC GetImportAddress(void *module, const char *name, const char *fn);
extern DWORD_PTR* GetImportAddressBase(void *module, const char *name, const char *fn);
extern PROC HookImportProc(void *module, const char *name, const char *fn, void *proc);
extern void HookCALL(void *addr, void *fn, void *code);
extern void HookJMP(void *addr, void *fn, void *code);
extern void vmemcpy(void *addr, const void *data, unsigned int size);
extern void* _memscan(const void *image_data, unsigned int image_size, const void *pattern_data, unsigned int pattern_size);
extern void* _memscan_ptr(const void *image_data, unsigned int image_size, const void *pattern_data, unsigned int pattern_size);
extern void* memscan(void *image_base, const void *pattern_data, unsigned int pattern_size);
extern void* memscan_ptr(void *image_base, const void *pattern_data, unsigned int pattern_size);
extern char* kstrstr(const char *text, const char *pattern);
extern char* kstristr(const char *text, const char *pattern);
extern void* thiscall0(void *f, void *a1);
extern void* thiscall1(void *f, void *a1, void *a2);
extern void* thiscall2(void *f, void *a1, void *a2, void *a3);
extern void* thiscall3(void *f, void *a1, void *a2, void *a3, void *a4);
extern void* thiscall4(void *f, void *a1, void *a2, void *a3, void *a4, void *a5);
extern void* thiscall5(void *f, void *a1, void *a2, void *a3, void *a4, void *a5, void *a6);
extern void* _memmove(void *d, const void *s, unsigned int c);
extern unsigned int _wcscpy(void *dst, const void *src);

/* wzcrypt */
extern void wzcrypt_setup(void);
extern unsigned int __fastcall wzcrypt_nextiv(unsigned int seed);
extern void __fastcall wzcrypt_encrypt(unsigned int size, unsigned int iv, const void *in, void *out);
extern void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, const void *in, void *out);

/* crc32 */
extern unsigned int crcsum32(unsigned int crc, const void *data, unsigned int size);

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

/* mt19937 */
extern void init_rand(unsigned int seed);
extern unsigned int gen_rand(void);

/* uuid */
extern char* gen_uuid(char buffer[48]);

/* secure */
enum {
	/* WINMM */
	s_PlaySoundA = 0,
	s_timeBeginPeriod,
	s_timeGetDevcaps,

	/* WS2_32 */
	s_closesocket,	/* #3 */
	s_connect,		/* #4 */
	s_ioctlsocket,	/* #10 */
	s_inet_addr,	/* #11 */
	s_recv,			/* #16 */
	s_select,		/* #18 */
	s_send,			/* #20 */
	s_setsockopt,	/* #21 */
	s_shutdown,		/* #22 */
	s_WSAGetOverlappedResult,
	s_WSARecv,
	s_WSASend,
	s_WSAAsyncSelect,
	s_WSAGetLastError,
	s_WSASetLastError,

	/* KERNEL32 */
	s_GetProcAddress, // ?
	s_CloseHandle, // 84
	s_CreateFileA, // 138
	s_CreateMutexA, // 157
	s_DeviceIoControl, // 223
	s_DisableThreadLibraryCalls, // 224
	s_ExitProcess, // 283
	s_FlushInstructionCache, // 346
	s_GetCurrentProcess, // 450
	s_GetCurrentProcessId, // 451
	s_GetCurrentThread, // 454
	s_GetLastError, // 514
	s_GetLocalTime, // 515
	s_GetModuleFileNameA, // 531
	s_GetNativeSystemInfo, // 549
	s_GetProcessHeap, // 586
	s_GetSystemFirmwareTable, // 627
	s_GetSystemInfo, // 628
	s_GetTickCount, // 661
	s_GetVolumeInformationA, // 678
	s_GlobalMemoryStatusEx, // 706
	s_HeapAlloc, // 717
	s_HeapCreate, // 719
	s_HeapDestroy, // 720
	s_HeapFree, // 721
	s_HeapReAlloc, // 724
	s_LoadLibraryA, // 829
	s_OutputDebugStringA, // 905
	s_SetCurrentDirectoryA, // 1100
	s_SetLastError, // 1138
	s_SetPriorityClass, // 1148
	s_SetProcessWorkingSetSize, // 1156
	s_SetThreadPriority, // 1177
	s_SetUnhandledExceptionFilter, // 1189
	s_Sleep, // 1203
	s_SleepEx, // 1206
	s_TerminateThread, // 1216
	s_TerminateProcess, // 1217
	s_VirtualProtect, // 1264
	s_VirtualQuery, // 1266
	s_GetPrivateProfileIntA,
	s_WritePrivateProfileStringA, // 1323

	/* USER32 */
	s_ClientToScreen, // 1580
	s_CreateDialogParamW, // 1608
	s_DestroyWindow, // 1675
	s_GetCursorPos, // 1797
	s_GetDlgItemInt, // 1805
	s_GetDlgItemTextA, // 1806
	s_GetKeyState, // 1826
	s_GetSystemMenu, // 1890
	s_InsertMenuW, // 1951
	s_KillTimer, // 1992
	s_MapVirtualKeyA, // 2030
	s_MsgWaitForMultipleObjects, // 2053
	s_MsgWaitForMultipleObjectsEx, // 2054
	s_PeekMessageA, // 2075
	s_PostMessageW, // 2079
	s_RegisterClassExA, // 2101
	s_SendDlgItemMessageW, // 2140
	s_SetDlgItemInt, // 2167
	s_SetForegroundWindow, // 2172
	s_SetTimer, // 2212
	s_SetWindowTextW, // 2228
	s_SetWindowsHookExA, // 2231
	s_ShowWindow, // 2248
	s_UpdateWindow, // 2298
	s_wvsprintfA, // 2333

	/* PSAPI */
	s_GetProcessMemoryInfo,

	/* AOSSDK */
	s_Aossdk_SetAuthServerA,
	s_Aossdk_GetMkdS4Object,
	s_Aossdk_StartAosSDKA,
	s_Aossdk_TerminatePdA,
	
	/* EOL */
	s_max_apis,
};

#define SECURE_API_XORKEY 0xBADF00Du
#define SECURE_API(x) ((PROC)((DWORD_PTR)_secure_api_ptr[(x)] ^ SECURE_API_XORKEY))
#define SECURE_API_SET(x, y) (_secure_api_ptr[(x)] = (PROC)((DWORD_PTR)(y) ^ SECURE_API_XORKEY))
extern PROC _secure_api_ptr[s_max_apis];
extern int secure_setup(void);

#pragma hdrstop