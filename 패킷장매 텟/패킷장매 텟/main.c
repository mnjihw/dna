#include "common.h"

#ifdef _WIN64
typedef int64_t int_t;
typedef uint64_t uint_t;
#else
typedef int32_t int_t;
typedef uint32_t uint_t;
#endif

typedef uint_t bool_t;
#define false	(0u != 0u)
#define true	(0u == 0u)

static unsigned char soundHeaderMask[] = { 0x02, 0x83, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70, 0x8B, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11, 0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70, 0x00, 0x01, 0x81, 0x9F, 0x58, 0x05, 0x56, 0xC3, 0xCE, 0x11, 0xBF, 0x01, 0x00, 0xAA, 0x00, 0x55, 0x59, 0x5A, 0x1E, 0x55, 0x00, 0x02, 0x00,/*FRQ 56*/0xAA, 0xBB, 0xCC, 0xDD/*/FRQ 59*/, 0x10, 0x27, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0A, 0x02, 0x01, 0x00, 0x00, 0x00 };
const char *getitemname(struct wz *wz, unsigned int itemid);
unsigned int AccessoryMaxIdx, AndroidMaxIdx, CapMaxIdx, CapeMaxIdx, CoatMaxIdx, GloveMaxIdx, LongcoatMaxIdx, PantsMaxIdx, RingMaxIdx, ShieldMaxIdx, ShoesMaxIdx, WeaponMaxIdx;
struct wzdata *Accessory, *Android, *Cap, *Cape, *Coat, *Glove, *Longcoat, *Pants, *Ring, *Shield, *Shoes, *Weapon;

const unsigned char *aeskey = "\xCA\x00\x00\x00\x89\x00\x00\x00\xFD\x00\x00\x00\xF6\x00\x00\x00\x99\x00\x00\x00\xF0\x00\x00\x00\xA6\x00\x00\x00\x0B\x00\x00\x00";
extern aes256_context ctx;

struct wznode {
	struct wznode *next, *prev, *child;
	char *name, *ref;
	void *data;
	uint32_t size;
};

struct wz {
	struct wznode *root;
	uint32_t offset, hash;
	uint8_t *data;
	uint64_t size;
};
struct wzdata
{
	char itemName[48];
	unsigned int itemid;
};



struct wz *wz;

typedef struct session {
	unsigned int auth, recv_iv, send_iv, ver, size;
	unsigned char data[128 * 1024];
} session_t;

ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];

void mouse_input2(HWND hwnd, int x, int y)
{
	POINT pt;

	ClientToScreen(hwnd, ((pt.x = x, pt.y = y), &pt));
	SetCursorPos(pt.x, pt.y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}


void Clipboard(char *source)
{
	HGLOBAL clipbuffer;
	char * buffer;
	int srclen;
	int ok = OpenClipboard(NULL);
	if (!ok)
		return;



	EmptyClipboard();
	srclen = strlen(source) + 1;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, srclen);
	buffer = (char*)GlobalLock(clipbuffer);
	strcpy(buffer, source);
	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();

}

void packet_handle(session_t *s, unsigned char *data, unsigned int size)
{
	unsigned int i;

	//dump(data, size);
	/*
	03:33:32.503 14 bytes
	[42 01] [F8 89 28 01] [00] [02 00] [B8 BB 00 00] [FF]       B...(....말...
	*/

	switch (*(unsigned short *)data)
	{

		case 0x2FF:
		{
			if (*(unsigned short*)&data[2] == 0x0415)//누군가가 교환 걸음
			{
				unsigned int nicksize = *(unsigned short*)&data[4];
				char nickname[13];

				memcpy(nickname, &data[6], nicksize);
				nickname[nicksize] = '\0'; // NULL
				SetForegroundWindow(FindWindow("MapleStoryClass", NULL));
				PostMessage(FindWindow("MapleStoryClass", NULL), WM_KEYDOWN, VK_RETURN, 0);
				Sleep(100);
				mouse_input2(FindWindow("MapleStoryClass", NULL), 584, 667);
				printf("%s님이 교환 걸음\n", nickname);
			}
			else if (*(unsigned short*)&data[2] == 0x0100)
			{
				//printf("%s 올림\n", getitemname(wz, *(unsigned int*)&data[6]));
				//Clipboard(getitemname(wz, *(unsigned int*)&data[6]));
				for (i = 0; i < AccessoryMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Accessory[i].itemid)
					{
						printf("%s 올림\n", Accessory[i].itemName);
						Clipboard(Accessory[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < AndroidMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Android[i].itemid)
					{
						printf("%s 올림\n", Android[i].itemName);
						Clipboard(Android[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < CapMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Cap[i].itemid)
					{
						printf("%s 올림\n", Cap[i].itemName);
						Clipboard(Cap[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < CapeMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Cape[i].itemid)
					{
						printf("%s 올림\n", Cape[i].itemName);
						Clipboard(Cape[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < CoatMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Coat[i].itemid)
					{
						printf("%s 올림\n", Coat[i].itemName);
						Clipboard(Coat[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < GloveMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Glove[i].itemid)
					{
						printf("%s 올림\n", Glove[i].itemName);
						Clipboard(Glove[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < LongcoatMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Longcoat[i].itemid)
					{
						printf("%s 올림\n", Longcoat[i].itemName);
						Clipboard(Longcoat[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < PantsMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Pants[i].itemid)
					{
						printf("%s 올림\n", Pants[i].itemName);
						Clipboard(Pants[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < RingMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Ring[i].itemid)
					{
						printf("%s 올림\n", Ring[i].itemName);
						Clipboard(Ring[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < ShieldMaxIdx; ++i)
				{
					if (*(unsigned int*)&data[6] == Shield[i].itemid)
					{
						printf("%s 올림\n", Shield[i].itemName);
						Clipboard(Shield[i].itemName);
						goto $END;
					}
				}
				for (i = 0; i < WeaponMaxIdx; ++i)
				{
					 if (*(unsigned int*)&data[6] == Weapon[i].itemid)
					{
						printf("%s 올림\n", Weapon[i].itemName);
						Clipboard(Weapon[i].itemName);
						goto $END;
					}
				}
				$END:
				Sleep(10);
				keybd_event(VK_CONTROL, 0, 0, 0);
				Sleep(50);
				//PostMessage(FindWindow("MapleStoryClass", NULL), WM_KEYDOWN, 0x56, 0);
				keybd_event(0x56, 0, 0, 0);
				Sleep(50);
				keybd_event(0x56, 0, 2, 0);
				//PostMessage(FindWindow("MapleStoryClass", NULL), WM_KEYUP, 0x56, 0);
				Sleep(50);
				keybd_event(VK_CONTROL, 0, 2, 0);
				PostMessage(FindWindow("MapleStoryClass", NULL), WM_KEYDOWN, VK_RETURN, 0);
				
			}
			else if (*(unsigned char*)&data[2] == 0x08)
			{
				mouse_input2(FindWindow("MapleStoryClass", NULL), 555, 222);
				Sleep(50);
				PostMessage(FindWindow("MapleStoryClass", NULL), WM_KEYDOWN, VK_RETURN, 0);
			}
			else if (*(unsigned char*)&data[2] == 0x1C)
			{
				if (*(unsigned char*)&data[4] == 0x2)
					PostMessage(FindWindow("MapleStoryClass", NULL), WM_KEYDOWN, VK_RETURN, 0);
			}
			else if (*(unsigned char*)&data[2] == 0x14)
				PostMessage(FindWindow("MapleStoryClass", NULL), WM_KEYDOWN, VK_RETURN, 0);
		}
		break;
		case 0x142: //누군가가 모두에게로 말함
		{
						unsigned int userID = *(unsigned int *)&data[2];
						unsigned int textSize = *(unsigned short *)&data[7];
						char textData[1024];

						memcpy(textData, &data[9], textSize);
						textData[textSize] = '\0';
						if (userID == 0x012CDC04)
						{
							return;
							//if (strstr
						}
						printf("아이디 %p인놈이 [%s]라 말함\n", userID, textData);
						Clipboard(textData);
		}
			break;
	}
}

unsigned int packet_parse(session_t *s, unsigned char *data, unsigned int size)
{
	unsigned int i, j, k;
	unsigned char pkt[65536];

	if (s->auth == 0u) {
		if ((j = *(unsigned __int16 *)data) + 2u > size)
			return 0;
		i = j + 2u;
		j = *(unsigned __int16 *)&data[4];
		s->send_iv = *(unsigned __int32 *)&data[j + 6u];
		s->recv_iv = *(unsigned __int32 *)&data[j + 10u];
		s->ver = atoi32((data[j + 6u] = 0u, (char *)&data[6]));
		s->auth = ~0u;
		/*dbg("S=%08X R=%08X v%u\n", s->send_iv, s->recv_iv, s->ver & 65535);
		dump(data, i);*/
	}
	else
		i = 0;

	k = s->recv_iv;

	for (; (size - i) >= (j = *(unsigned __int16 *)&data[i] ^ *(unsigned __int16 *)&data[i + 2u]) + 4u; i += j + 4u) {
		/*dbg("%u bytes HEAD=%04X, EXPECTED=%04X\n", j, *(unsigned __int16 *)&data[i], (k >> 16) ^ (65535 & ~s->ver));*/

		wzcrypt_decrypt(j, k, &data[i + 4u], pkt);
		packet_handle(s, pkt, j);
		k = wzcrypt_nextiv(k);
	}

	s->recv_iv = k;
	return i;
}

void packet_handler_sub(session_t *s, unsigned char *data, unsigned int size)
{
	unsigned int i;

	if (s->size) { /* fragmented */
		if (size > sizeof(s->data) - s->size)
			__asm int 3;
		memcpy(&s->data[s->size], data, size);
		if ((i = packet_parse(s, s->data, s->size += size)) != 0)
		if ((s->size -= i) != 0)
			memcpy(s->data, &s->data[i], s->size);
	}
	else if ((i = packet_parse(s, data, size)) != size)
		memcpy(s->data, &data[i], s->size = size - i);
}

void packet_handler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
	unsigned int i, j;
	session_t *s;
	iphdr_t *ih;
	tcphdr_t *th;

	ih = (iphdr_t *)(pkt_data + 14u);
	th = (tcphdr_t *)(pkt_data + 14u + (ih->ihl << 2));

	if (th->fin != 0u || th->rst != 0u) {
		if ((s = session[th->dport]) != NULL) {
			session[th->dport] = NULL;
			HeapFree(GetProcessHeap(), 0u, s);
			//dbg("%p %s\n", s, "인식종료");
		}
	}
	else {
		if (th->syn != 0u) {
			if ((s = session[th->dport]) == NULL) {
				if ((s = HeapAlloc(GetProcessHeap(), 0u, sizeof(session_t))) == NULL)
					__asm int 3;
				session[th->dport] = s;
			}
			//dbg("%p %s\n", s, "인식시작");
			s->auth = 0u;
			s->size = 0u;
		}
		else if ((s = session[th->dport]) == NULL)
			return;
		if (i = ih->tot_len, (i = (((i & 0xFF) << 8) | (i >> 8)) - (j = (ih->ihl << 2) + (th->doff << 2))) != 0)
			packet_handler_sub(s, (char *)pkt_data + j + 14u, i);
	}
}

unsigned int NpfFindAllDevices(void)
{
	unsigned int i, size;
	char *name, *desc;

	memset(npf_devicelist, 0, sizeof(npf_devicelist));

	if ((size = sizeof(npf_devicelist), PacketGetAdapterNames(npf_devicelist, &size) == FALSE) || size == 0)
		return 0;

	for (name = npf_devicelist, desc = &npf_devicelist[2]; desc[-2] || desc[-1]; ++desc);

	for (i = 0; *name; ++i) {
		name += strlen(name) + 1u;
		desc += strlen(desc) + 1u;
	}

	strcpy(npf_device, npf_devicelist);
	return i;
}

unsigned int NpfGetDeviceCount(void)
{
	unsigned int i;
	char *name, *desc;

	for (name = npf_devicelist, desc = &npf_devicelist[2]; desc[-2] || desc[-1]; ++desc);

	for (i = 0; *name; ++i) {
		name += strlen(name) + 1u;
		desc += strlen(desc) + 1u;
	}

	return i;
}

unsigned int __stdcall NpfGetDeviceInfo(unsigned int index, char *device_name, char *device_desc)
{
	unsigned int i;
	char *name, *desc;

	for (name = npf_devicelist, desc = &npf_devicelist[2]; desc[-2] || desc[-1]; ++desc);

	for (i = 1; *name; ++i) {
		if (i == index) {
			if (device_name)
				strcpy(device_name, name);
			if (device_desc)
				strcpy(device_desc, desc);
			return i;
		}
		name += strlen(name) + 1u;
		desc += strlen(desc) + 1u;
	}

	return 0;
}

unsigned int __stdcall NpfSetDevice(const char *device_name)
{
	unsigned int i;
	char *name;

	for (i = 1, name = npf_devicelist; *name; ++i) {
		if (strcmp(name, device_name) == 0) {
			strcpy(npf_device, name);
			return i;
		}
		name += strlen(name) + 1u;
	}

	return 0;
}

unsigned int NpfStart(void)
{
	static struct bpf_program bpf_code = {
		17, (struct bpf_insn *) /* tcp && src portrange 8585-8589 */
		"\x28\x00\x00\x00\x0C\x00\x00\x00\x15\x00\x00\x04\xDD\x86\x00\x00"
		"\x30\x00\x00\x00\x14\x00\x00\x00\x15\x00\x00\x0C\x06\x00\x00\x00"
		"\x28\x00\x00\x00\x36\x00\x00\x00\x35\x00\x08\x0A\x89\x21\x00\x00"
		"\x15\x00\x00\x09\x00\x08\x00\x00\x30\x00\x00\x00\x17\x00\x00\x00"
		"\x15\x00\x00\x07\x06\x00\x00\x00\x28\x00\x00\x00\x14\x00\x00\x00"
		"\x45\x00\x05\x00\xFF\x1F\x00\x00\xB1\x00\x00\x00\x0E\x00\x00\x00"
		"\x48\x00\x00\x00\x0E\x00\x00\x00\x35\x00\x00\x02\x89\x21\x00\x00"
		"\x25\x00\x01\x00\x8D\x21\x00\x00\x06\x00\x00\x00\x00\x00\x01\x00"
		"\x06\x00\x00\x00\x00\x00\x00\x00"
	};
	ADAPTER *adapter;

	if ((adapter = PacketOpenAdapter(npf_device)) != NULL) {
		if (adapter->Flags == INFO_FLAG_NDIS_ADAPTER)
		if (PacketSetBpf(adapter, &bpf_code) != FALSE)
		if (PacketSetBuff(adapter, 1048576u) != FALSE)
		if (PacketSetHwFilter(adapter, 1/*NDIS_PACKET_TYPE_DIRECTED*/) != FALSE) {
			npf_adapter = adapter;
			return 1u;
		}
		PacketCloseAdapter(adapter);
	}

	return 0u;
}

void NpfStop(void)
{
	if (npf_adapter) {
		PacketCloseAdapter(npf_adapter);
		npf_adapter = NULL;
	}
}

void NpfCheckMessage(void)
{
	static unsigned int pkt_size;
	static unsigned char pkt_data[65536];
	unsigned int i, j;
	ADAPTER *adapter;

	pkt_size = 0u;

	if ((adapter = npf_adapter) != NULL)
	if (WaitForSingleObject(adapter->ReadEvent, 1) == WAIT_OBJECT_0)
	if (ReadFile(adapter->hFile, pkt_data, sizeof(pkt_data), &pkt_size, NULL))
	for (i = 0, j = pkt_size; i < j; i += (((struct bpf_hdr *)&pkt_data[i])->bh_caplen + ((struct bpf_hdr *)&pkt_data[i])->bh_hdrlen + 3u) & ~3)
		packet_handler(NULL, (struct pcap_pkthdr *)&pkt_data[i], &pkt_data[i + ((struct bpf_hdr *)&pkt_data[i])->bh_hdrlen]);
}




extern char* DecodeString(char *lpString1, const char *lpString2);

char* string(char *string)
{
	uint_t size = strlen(string);
	return memcpy(LocalAlloc(LMEM_FIXED, size + 1u), string, size + 1u);
}


void dbg2(const char ch)
{
	DWORD bytes;
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), &ch, 1, &bytes, NULL);
}


const char* print32(unsigned value)
{
	static char buf[64];
	char *p;

	p = &buf[sizeof(buf)];
	*--p = '\0';

	do {
		/*if (((unsigned)(p - buf) & 3) == 0)
		*--p = ',';*/
		*--p = (char)(value % 10u) + '0';
	} while (value /= 10u);

	return p;
}

#define read1(p) ( (void *)(p) = (void *)((unsigned)(p) + 1u), *(unsigned __int8 *)((unsigned)(p) - 1u) )
#define read2(p) ( (void *)(p) = (void *)((unsigned)(p) + 2u), *(unsigned __int16 *)((unsigned)(p) - 2u) )
#define read4(p) ( (void *)(p) = (void *)((unsigned)(p) + 4u), *(unsigned __int32 *)((unsigned)(p) - 4u) )
#define read8(p) ( (void *)(p) = (void *)((unsigned)(p) + 8u), *(unsigned __int64 *)((unsigned)(p) - 8u) )
#define readF(p) ( (void *)(p) = (void *)((unsigned)(p) + 4u), *(float *)((unsigned)(p) - 4u) )
#define readD(p) ( (void *)(p) = (void *)((unsigned)(p) + 8u), *(double *)((unsigned)(p) - 8u) )
#define readN(p) ( (*(unsigned __int8 *)(p) == 0x80) ? ((void *)(p) = (void *)((unsigned)(p) + 5u), *(unsigned __int32 *)((unsigned)(p) - 4u)) : ((void *)(p) = (void *)((unsigned)(p) + 1u), *(unsigned __int8 *)((unsigned)(p) - 1u)) )

#define write1(p, x) ( (void *)(p) = (void *)((unsigned)(p) + 1u), *(unsigned __int8 *)((unsigned)(p) - 1u) = (unsigned __int8)(x) )
#define write2(p, x) ( (void *)(p) = (void *)((unsigned)(p) + 2u), *(unsigned __int16 *)((unsigned)(p) - 2u) = (unsigned __int16)(x) )
#define write4(p, x) ( (void *)(p) = (void *)((unsigned)(p) + 4u), *(unsigned __int32 *)((unsigned)(p) - 4u) = (unsigned __int32)(x) )
#define write8(p, x) ( (void *)(p) = (void *)((unsigned)(p) + 8u), *(unsigned __int64 *)((unsigned)(p) - 8u) = (unsigned __int64)(x) )
#define writeF(p, x) ( (void *)(p) = (void *)((unsigned)(p) + 4u), *(float *)((unsigned)(p) - 4u) = (float)(x) )
#define writeD(p, x) ( (void *)(p) = (void *)((unsigned)(p) + 8u), *(double *)((unsigned)(p) - 8u) = (double)(x) )
#define writeN(p, x) ( ((int)(x) < 0 || (int)(x) > 127) ? ((void *)(p) = (void *)((unsigned)(p) + 5u), *(unsigned __int8 *)((unsigned)(p) - 5u) = 0x80, *(unsigned __int32 *)((unsigned)(p) - 4u) = (unsigned __int32)(x)) : ((void *)(p) = (void *)((unsigned)(p) + 1u), *(unsigned __int8 *)((unsigned)(p) - 1u) = (unsigned __int8)(x)) )

unsigned minimap_mode;
unsigned char wz_key[65536];

uint32_t wz_crcsum(const void *data, uint_t size)
{
	uint8_t *p = (uint8_t *)data;
	uint32_t result = 0;

	while (size)
		result += *p++, --size;

	return result;
}

void wz_genkey(void)
{
	static unsigned key[60] = { /* "\x13\x00\x00\x00\x08\x00\x00\x00\x06\x00\x00\x00\xB4\x00\x00\x00\x1B\x00\x00\x00\x0F\x00\x00\x00\x33\x00\x00\x00\x52\x00\x00\x00" */
		0x13000000, 0x08000000, 0x06000000, 0xB4000000, 0x1B000000, 0x0F000000, 0x33000000, 0x52000000,
		0x71636300, 0x79636300, 0x7F636300, 0xCB636300, 0x04FBFB63, 0x0BFBFB63, 0x38FBFB63, 0x6AFBFB63,
		0x7C6C9802, 0x050FFB02, 0x7A6C9802, 0xB10FFB02, 0xCC8DF414, 0xC7760F77, 0xFF8DF414, 0x95760F77,
		0x401A6D28, 0x4515962A, 0x3F790E28, 0x8E76F52A, 0xD5B512F1, 0x12C31D86, 0xED4EE992, 0x7838E6E5,
		0x4F94B494, 0x0A8122BE, 0x35F82C96, 0xBB8ED9BC, 0x3FAC2794, 0x2D6F3A12, 0xC021D380, 0xB8193565,
		0x8B02F9F8, 0x8183DB46, 0xB47BF7D0, 0x0FF52E6C, 0x494A16C4, 0x64252CD6, 0xA404FF56, 0x1C1DCA33,
		0x0F763A64, 0x8EF5E122, 0x3A8E16F2, 0x357B389E, 0xDF6B11CF, 0xBB4E3D19, 0x1F4AC24F, 0x0357087C,
		0x14462A1F, 0x9AB3CB3D, 0xA03DDDCF, 0x9546E551,
	};

	unsigned i;

	/* MSEA IV => 0xE9637DB9 */
	aes256_encrypt(key, "\xB9\x7D\x63\xE9\xB9\x7D\x63\xE9\xB9\x7D\x63\xE9\xB9\x7D\x63\xE9", wz_key);

	for (i = 16u; i < 65536u; i += 16u)
		aes256_encrypt(key, &wz_key[i - 16u], &wz_key[i]);
}

unsigned char* wz_readstring(unsigned char *p, void *string)
{
	unsigned i, j, k;
	unsigned short temp[1024];

	j = read1(p);

	if ((j & 0x80) != 0) {
		j = ((j &= 0x7F) != 0) ? (128u - j) : read4(p);
		if (string) {
			for (k = 0xAA, i = 0; i < j && i < 1023u; ++k, ++i)
				((unsigned char *)string)[i] = ((unsigned char *)p)[i] ^ ((unsigned char *)wz_key)[i] ^ k;
			((unsigned char *)string)[j] = 0;
		}
		p += j;
	}
	else {
		if (j == 0x7F)
			j = read4(p);
		if (string) {
			for (k = 0xAAAA, i = 0; i < j && i < 1023u; ++k, ++i)
				((unsigned short *)temp)[i] = ((unsigned short *)p)[i] ^ ((unsigned short *)wz_key)[i] ^ k;
			((unsigned short *)temp)[j] = 0;
			WideCharToMultiByte(CP_ACP, 0, temp, -1, string, 1024, NULL, NULL);
		}
		p += (j << 1);
	}

	return p;
}

unsigned char* wz_readstring2(unsigned char *p, unsigned char *base, void *string)
{
	unsigned i;

	i = read1(p);

	if (i == 0 || i == 0x73)
		return wz_readstring(p, string);

	if (i == 1 || i == 0x1B) {
		wz_readstring(base + read4(p), string);
		return p;
	}

	return NULL;
}

unsigned char* wz_parseprop(struct wz *wz, struct wznode **root, uint8_t *data, uint8_t *p);

unsigned char* wz_parseprop2(struct wz *wz, struct wznode **root, uint8_t *base, uint8_t *p)
{
	struct wznode *node;

	uint_t i;
	uint32_t size;
	char name[1024];

	switch (read1(p)) {
	case 0x1B:
		wz_readstring(base + read4(p), name);
		break;

	case 0x73:
		p = wz_readstring(p, name);
		break;

	default:
		dbg("%s\n", "wz_parseprop2 #1");
		return NULL;
	}

	if (strcmp(name, "Property") == 0) { /* WzSubProperty */
		if ((p = wz_parseprop(wz, root, base, p)) == NULL) {
			dbg("%s\n", "wz_parseprop2 #2");
			return NULL;
		}
	}
	else {
		node = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(struct wznode));
		node->name = string(name);
		if ((*root) == NULL)
			(*root) = node;
		else {
			node->next = (*root);
			(*root)->prev = node;
			(*root) = node;
		}
		if (strcmp(name, "Canvas") == 0) { /* WzCanvasProperty */
			read1(p); /* dummy */
			if (read1(p) == 1)
			if ((p = wz_parseprop(wz, &node->child, base, p)) == NULL) { /* canvasProp */
				dbg("%s\n", "wz_parseprop2 #3");
				return NULL;
			}
			readN(p); /* width */
			readN(p); /* height */
			readN(p); /* format */
			read1(p); /* format2 */
			read4(p); /* dummy */
			p += read4(p); /* size */
		}
		else if (strcmp(name, "Shape2D#Vector2D") == 0) { /* WzVectorProperty */
			readN(p); /* X */
			readN(p); /* Y */
		}
		else if (strcmp(name, "Shape2D#Convex2D") == 0) { /* WzConvexProperty */
			for (i = readN(p); i; --i)
			if ((p = wz_parseprop2(wz, &node->child, base, p)) == NULL) {
				dbg("%s\n", "wz_parseprop2 #4");
				return NULL;
			}
		}
		else if (strcmp(name, "Sound_DX8") == 0) { /* WzSoundProperty */
			read1(p); /* dummy */
			size = readN(p); /* size */
			readN(p); /* milliseconds */
			node->data = p;
			node->size = size;
			// FIXME: 여기 제대로 안됫음
			p += size;
		}
		else if (strcmp(name, "UOL") == 0) { /* WzUOLProperty */
			read1(p); /* dummy */
			switch (read1(p)) {
			case 0:
				p = wz_readstring(p, name);
				node->ref = string(name);
				break;

			case 1:
				wz_readstring(base + read4(p), name);
				node->ref = string(name);
				break;

			default:
				dbg("%s\n", "wz_parseprop2 #5");
				return NULL;
			}
		}
		else {
			dbg("%s\n", "wz_parseprop2 #6");
			p = NULL;
		}
	}

	return p;
}

unsigned char* wz_parseprop(struct wz *wz, struct wznode **root, uint8_t *data, uint8_t *p)
{
	struct wznode *node;
	uint_t i, j;
	uint32_t size;
	char name[1024];
	char stringValue[1024];

	read2(p); /* dummy */

	for (i = readN(p); i; --i) {
		if ((p = wz_readstring2(p, data, name)) == NULL) {
			dbg("%s\n", "wz_parseprop #1");
			return NULL;
		}

		node = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(struct wznode));
		node->name = string(name);
		if ((*root) == NULL)
			(*root) = node;
		else {
			node->next = (*root);
			(*root)->prev = node;
			(*root) = node;
		}

		switch (read1(p)) {
		case 0: /* WzNullProperty */
			break;

		case 2: /* WzUnsignedShortProperty */
			read2(p);
			break;

		case 3: /* WzCompressedIntProperty */
			readN(p);
			break;

		case 4: /* WzByteFloatProperty */
			j = read1(p);
			if (j == 0x80)
				readF(p);
			else if (j == 0x00)
				; /* 0.0f */
			else {
				dbg("%s\n", "wz_parseprop #6");
				return NULL;
			}
			break;

		case 5: /* WzDoubleProperty */
			readD(p);
			break;

		case 8: /* WzStringProperty */
			// 0,1 타입만 있는듯
			*stringValue = '\0';
			if ((p = wz_readstring2(p, data, stringValue)) == NULL) {
				dbg("%s\n", "wz_parseprop #2");
				return NULL;
			}
			if (*stringValue)
				node->data = _strdup(stringValue);
			break;

		case 9: /* WzImgProperty */
			size = read4(p); /* size */
			if (wz_parseprop2(wz, &node->child, data, p) == NULL) {
				dbg("%s\n", "wz_parseprop #3");
				return NULL;
			}
			p += size;
			break;

		case 11: /* WzUnsignedShortProperty */
			read2(p);
			break;

		case 20: /* character.wz incPAD였나 암튼 새로생긴거같은뎅 */
			read1(p);
			break;

		default:
			dbg("%s\n", "wz_parseprop #5");
			return NULL;
		}
	}

	return p;
}

bool_t wz_parseimg(struct wz *wz, struct wznode **root, uint8_t *data, uint32_t size)
{
	uint8_t *p = data;
	char name[1024];

	if (read1(p) != 0x73) {
		dbg("%s\n", "wz_parseimg #1");
		return false;
	}

	p = wz_readstring(p, name);

	if (strcmp(name, "Property")) {
		dbg("%s\n", "wz_parseimg #2");
		return false;
	}

	if (wz_parseprop(wz, root, data, p) == NULL) {
		dbg("%s\n", "wz_parseimg #3");
		return false;
	}

	return true;
}

bool_t wz_parsedir(struct wz *wz, struct wznode **root, uint8_t *data, uint8_t *p)
{
	uint_t i, j;
	struct wznode *node;
	uint32_t size, offset;
	char name[1024];

	for (i = readN(p); i; --i) {
		j = read1(p);

		if (j == 2u) {
			uint8_t *pp = &data[read4(p)];
			j = read1(pp);
			wz_readstring(pp, name);
		}
		else if (j == 3u || j == 4u)
			p = wz_readstring(p, name);

		if (j != 3u && j != 4u) {
			dbg("%s\n", "wz_parsedir #1");
			return false;
		}

		node = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(struct wznode));
		node->name = string(name);
		if ((*root) == NULL)
			(*root) = node;
		else {
			node->next = (*root);
			(*root)->prev = node;
			(*root) = node;
		}

		size = readN(p); /* size */
		readN(p); /* crcsum */
		offset = wz->hash * (~(uint32_t)(p - data)) - 0x581C3F6Du;
		offset = (_rotl(offset, offset & 31) ^ read4(p)) + wz->offset;

		if (j == 3u) {
			if (wz_parsedir(wz, &node->child, data, data + offset) == false) {
				dbg("%s\n", "wz_parsedir #2");
				return false;
			}
		}
		else if (j == 4u) {
			if (wz_parseimg(wz, &node->child, data + offset, size) == false) {
				dbg("%s\n", "wz_parsedir #3");
				return false;
			}
		}
	}

	return true;
}

bool_t wz_parsesub(struct wz *wz, uint8_t *data, uint32_t size)
{
	uint8_t *p = data;
	uint_t i, j, k;
	uint32_t a, b, c;

	k = read2(p); /* version hash */

	if (readN(p) == 0 || (i = read1(p), (i != 3u && i != 4u)))
		return false;

	p = wz_readstring(p, NULL);
	readN(p); /* size */
	readN(p); /* crcsum */
	a = (uint32_t)(p - data);
	b = read4(p);

	for (i = 0; i != 1024u; ++i) {
		for (j = 0, p = (uint8_t *)print32(i); *p; ++p)
			j = (j << 5) + *p + 1u;
		if (k == ((j ^ (j >> 8) ^ (j >> 16) ^ (j >> 24) ^ 255) & 255)) {
			c = (j * ~a) - 0x581C3F6Du;
			c = (_rotl(c, c & 31) ^ b) + wz->offset;
			if ((uint64_t)c < size && data[c] == 0x73u) {
				wz->hash = j;
				dbg("v%u hash = %08X\n", i, j);
				return true;
			}
		}
	}

	return false;
}

bool_t wz_parse(struct wz *wz)
{
	if (wz->size < 16u || *(uint32_t *)wz->data != (uint32_t)'1GKP' || *(uint64_t *)&wz->data[4] > wz->size || (wz->offset = *(uint32_t *)&wz->data[12]) < 16u || (uint64_t)wz->offset > wz->size)
		return false;

	return wz_parsesub(wz, wz->data + wz->offset, (uint32_t)(wz->size - wz->offset)) && wz_parsedir(wz, &wz->root, wz->data + wz->offset, wz->data + wz->offset + 2);
}

struct wz* wz_open(const char *file)
{
	struct wz *wz;
	HANDLE handle1, handle2;

	if ((handle1 = CreateFileA(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE) {
		if ((handle2 = CreateFileMappingA(handle1, NULL, PAGE_READONLY, 0, 0, NULL)) != NULL) {
			if ((wz = LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(struct wz))) != NULL) {
				if ((wz->data = MapViewOfFile(handle2, FILE_MAP_READ, 0, 0, 0)) != NULL) {
					if (GetFileSizeEx(handle1, (LARGE_INTEGER *)&wz->size))
					if (wz_parse(wz) == true)
						return wz;
					UnmapViewOfFile(wz->data);
				}
				LocalFree(wz);
			}
			CloseHandle(handle2);
		}
		CloseHandle(handle1);
	}

	return NULL;
}

void wz_close(struct wz *wz)
{
	if (wz) {
		// FIXME: 노드 순회하면서 지워야됨
		UnmapViewOfFile(wz->data);
		LocalFree(wz);
	}
}

uint_t parse_args(char *p, char **a, uint_t n)
{
	uint_t i = 0u;

	while (i != n) {
		if (*p == '\0')
			break;
		a[i++] = p;
		for (;;) {
			while ((*p & 0x80) && p[1])
				p += 2u;
			if (*p == '\0')
				break;
			if (*p == '/') {
				*p++ = '\0';
				break;
			}
			++p;
		}
	}

	return i;
}


struct wznode* wz_get(struct wz *wz, const char *path)
{
	struct wznode *node = wz->root;
	char buffer[4096];
	char *a[64];
	uint_t i, j;

	j = parse_args(strcpy(buffer, path), a, sizeof(a) / sizeof(a[0]));

	for (i = 0; i < j; ++i) {
		while (node) {
			if (node->name && strcmp(node->name, a[i]) == 0) {
				if (i + 1 != j)
					node = node->child;
				break;
			}
			node = node->next;
		}
		if (node == NULL)
			break;
	}

	return node;
}


void r(struct wznode *node, uint_t depth)
{
	uint_t i;




	if (node == NULL)
		return;

	for (i = depth; i; --i)
		dbg("%s", "\t");

	dbg("%s\n", node->name);



	if (node->child)
		r(node->child, depth + 1);

	if (node->next)
		r(node->next, depth);

}

const char *getitemname(struct wz *wz, unsigned int itemid)//풀경로인자로받기
{
	struct wznode *node;
	char path[1024];

	wsprintf(path, "Eqp.img/Eqp/Weapon/%u/name",itemid);
	
	node = wz_get(wz, path);
	if (!node)
		return "";
	return node->data;
}

unsigned int searchitemid(struct wz *wz, char *itemName)
{
	unsigned int itemid = 0, i;
	char path[1024];
	//Eqp->Accessory Android Cap Cape Coat Face Glove Longcoat Pants Ring Shield Shoes Weapon
	
	/*Eqp part*/
	//wsprintf(path, "Eqp.img/Eqp/Android");
	wsprintf(path, "Cash.img");
	for (i = getitemcount(wz, path); i >= 0;--i)
	{
		wsprintf(path, "Cash.img/%d", i);
	}

	return itemid;
}

unsigned int getitemcount(struct wz *wz, char *path)
{
	unsigned int count = 1;
	struct wznode *node;

	node = wz_get(wz, path);
	node = node->child;
	while (node->next)
	{
		++count;
		node = node->next;
	}
	return count;
}

void main()
{
	struct wznode *node;
	unsigned int size = 0,itemid = 0;
	int i = 0;
	char buf[128],input[128];

	wz_genkey();
	wz = wz_open("C:\\Nexon\\Maple\\String.wz");

	SetConsoleTitle("패킷장매 텟");


	printf("아이템 이름 입력 : ");
	gets(input);
	
	printf("%d",searchitemid(wz, buf));
	return;
	for (i = getitemcount(wz, "Eqp.img/Eqp/Cap"); i >= 0; --i)
	{
		if (getitemname(wz,i))
		if (strstr(buf, getitemname(wz, i + 1000000)) == 0)
		{
			printf("찾았음 ㅋㅋ itemid : %d\n", i + 1000000);
			break;
		}

	}
	
	getchar();
	return;
	///////////////////// 시작
	node = wz_get(wz, "Eqp.img/Eqp/Accessory");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Accessory");
	Accessory = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	AccessoryMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Accessory[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Accessory/%u/name", Accessory[i - 1].itemid);
		wsprintf(Accessory[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Accessory[i - 1].itemid, i);
		//printf("%s\n", Accessory[i - 1].itemName);

		
		if (node->next)
			node = node->next;

	}
	for (i = 1; i<size; ++i)
	{
		if (strstr(Accessory[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Accessory[i - 1].itemName, Accessory[i - 1].itemid, i);
			itemid = Accessory[i - 1].itemid;
			break;
		}
	}
	
	///////////////////////////  끝
	node = wz_get(wz, "Eqp.img/Eqp/Android");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Android");

	Android = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	AndroidMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{
		Android[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Android/%u/name", Android[i - 1].itemid);
		wsprintf(Android[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Android[i - 1].itemid, i);
		//printf("%s\n", Android[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Android[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Android[i - 1].itemName, Android[i - 1].itemid, i);
			itemid = Android[i - 1].itemid;
			break;
		}

	}
	
	//끝 

	node = wz_get(wz, "Eqp.img/Eqp/Cap");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Cap");
	Cap = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	CapMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Cap[i - 1].itemid = atoi(node->name); //여기서터짐
		wsprintf(buf, "Eqp.img/Eqp/Cap/%u/name", Cap[i - 1].itemid);
		wsprintf(Cap[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Cap[i - 1].itemid, i);
		//printf("%s\n", Cap[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Cap[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Cap[i - 1].itemName, Cap[i - 1].itemid, i);
			itemid = Cap[i - 1].itemid;
			break;
		}
	}

	//끝

	node = wz_get(wz, "Eqp.img/Eqp/Cape");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Cape");
	Cape = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	CapeMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Cape[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Cape/%u/name", Cape[i - 1].itemid);
		wsprintf(Cape[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Cape[i - 1].itemid, i);
		//printf("%s\n", Cape[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Cape[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Cape[i - 1].itemName, Cape[i - 1].itemid, i);
			itemid = Cape[i - 1].itemid;
			break;
		}
	}
	//끝
	node = wz_get(wz, "Eqp.img/Eqp/Coat");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Coat");
	Coat = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	CoatMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Coat[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Coat/%u/name", Coat[i - 1].itemid);
		wsprintf(Coat[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Coat[i - 1].itemid, i);
		//printf("%s\n", Coat[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Coat[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Coat[i - 1].itemName, Coat[i - 1].itemid, i);
			itemid = Coat[i - 1].itemid;
			break;
		}
	}
	
	//끝

	node = wz_get(wz, "Eqp.img/Eqp/Glove");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Glove");
	Glove = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	GloveMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Glove[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Glove/%u/name", Glove[i - 1].itemid);
		wsprintf(Glove[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Glove[i - 1].itemid, i);
		//printf("%s\n", Glove[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Glove[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Glove[i - 1].itemName, Glove[i - 1].itemid, i);
			itemid = Glove[i - 1].itemid;
			break;
		}
	}
	//끝
	
	node = wz_get(wz, "Eqp.img/Eqp/Longcoat");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Longcoat");
	Longcoat = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	LongcoatMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Longcoat[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Longcoat/%u/name", Longcoat[i - 1].itemid);
		wsprintf(Longcoat[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Longcoat[i - 1].itemid, i);
		//printf("%s\n", Longcoat[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Longcoat[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Longcoat[i - 1].itemName, Longcoat[i - 1].itemid, i);
			itemid = Longcoat[i - 1].itemid;
			break;
		}
	}

	//끝

	node = wz_get(wz, "Eqp.img/Eqp/Pants");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Pants");
	Pants = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	PantsMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Pants[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Pants/%u/name", Pants[i - 1].itemid);
		wsprintf(Pants[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Pants[i - 1].itemid, i);
		//printf("%s\n", Pants[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Pants[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Pants[i - 1].itemName, Pants[i - 1].itemid, i);
			itemid = Pants[i - 1].itemid;
			break;
		}
	}

	//끝
	node = wz_get(wz, "Eqp.img/Eqp/Ring");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Ring");
	Ring = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	RingMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Ring[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Ring/%u/name", Ring[i - 1].itemid);
		wsprintf(Ring[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Ring[i - 1].itemid, i);
		//printf("%s\n", Ring[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Ring[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Ring[i - 1].itemName, Ring[i - 1].itemid, i);
			itemid = Ring[i - 1].itemid;
			break;
		}
	}
	//끝
	node = wz_get(wz, "Eqp.img/Eqp/Shield");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Shield");
	Shield = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	ShieldMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Shield[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Shield/%u/name", Shield[i - 1].itemid);
		wsprintf(Shield[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Shield[i - 1].itemid, i);
		//printf("%s\n", Shield[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Shield[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Shield[i - 1].itemName, Shield[i - 1].itemid, i);
			itemid = Shield[i - 1].itemid;
			break;
		}
	}
	//끝
	node = wz_get(wz, "Eqp.img/Eqp/Shoes");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Shoes");
	Shoes = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	ShoesMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Shoes[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Shoes/%u/name", Shoes[i - 1].itemid);
		wsprintf(Shoes[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Shoes[i - 1].itemid, i);
		//printf("%s\n", Shoes[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Shoes[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Shoes[i - 1].itemName, Shoes[i - 1].itemid, i);
			itemid = Shoes[i - 1].itemid;
			break;
		}
	}
	//끝

	node = wz_get(wz, "Eqp.img/Eqp/Weapon");
	node = node->child;
	size = getitemcount(wz, "Eqp.img/Eqp/Weapon");
	Weapon = (struct wzdata*)calloc(size, sizeof(struct wzdata));
	WeaponMaxIdx = size - 1;

	for (i = 1; i < size; ++i)
	{

		Weapon[i - 1].itemid = atoi(node->name);
		wsprintf(buf, "Eqp.img/Eqp/Weapon/%u/name", Weapon[i - 1].itemid);
		wsprintf(Weapon[i - 1].itemName, "%s", wz_get(wz, buf)->data);
		//printf("%u %d", Weapon[i - 1].itemid, i);
		//printf("%s\n", Weapon[i - 1].itemName);
		if (node->next)
			node = node->next;

	}
	for (i = 1; i < size; ++i)
	{
		if (strstr(Weapon[i - 1].itemName, input))
		{
			printf("찾앗다 itemName : %s itemid : %u i : %d\n", Weapon[i - 1].itemName, Weapon[i - 1].itemid, i);
			itemid = Weapon[i - 1].itemid;
			break;
		}
	}
	//끝


	NpfFindAllDevices();
	if (NpfStart()) {
		for (;;)
			NpfCheckMessage();
		NpfStop();
	}
	ExitProcess(0);

	//printf("%s\n", wz_get(wz, "Eqp.img/Eqp/Cap"));
	//	r(wz_get(wz, "Eqp.img/Eqp/Cap"), 0);

	//dump(wz_get(wz, "Bgm11.img/Aquarium/Sound_DX8")->data, 16);
	//dump(wz_get(wz, "Eqp.img/Eqp/Cap")->ref, 16);
	getchar();
	/*free(Accessory);
	free(Android);
	free(Cap);
	free(Cape);
	free(Coat);
	free(Glove);
	free(Longcoat);
	free(Pants);
	free(Ring);
	free(Shield);
	free(Shoes);
	free(Weapon);
	ZeroMemory(&Accessory, sizeof(struct wzdata));
	ZeroMemory(&Android, sizeof(struct wzdata));
	ZeroMemory(&Cap, sizeof(struct wzdata));
	ZeroMemory(&Cape, sizeof(struct wzdata));
	ZeroMemory(&Coat, sizeof(struct wzdata));
	ZeroMemory(&Glove, sizeof(struct wzdata));
	ZeroMemory(&Longcoat, sizeof(struct wzdata));
	ZeroMemory(&Pants, sizeof(struct wzdata));
	ZeroMemory(&Ring, sizeof(struct wzdata));
	ZeroMemory(&Shield, sizeof(struct wzdata));
	ZeroMemory(&Shoes, sizeof(struct wzdata));
	ZeroMemory(&Weapon, sizeof(struct wzdata));*/
	wz_close(wz);

	return;
}
