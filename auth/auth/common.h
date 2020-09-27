#include <WinSock2.h>
#include <stdio.h>
#include <ShlObj.h>
#include <VirtualizerSDK.h>
#include <TlHelp32.h>
#pragma comment(lib, "ws2_32.lib")



/* SHA1 */
typedef struct 
{
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

/* crc32 */
extern unsigned int crcsum32(unsigned int crc, const void *data, unsigned int size);

void RFCrypt(int size, const void *in, void *out);
SOCKET ConnectToServer();
SOCKET _ConnectToServer();
#pragma hdrstop