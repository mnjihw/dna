#define _CRT_SECURE_NO_WARNINGS

#define BLACK_ALL

#include "common.h"
#define R_SERVERINFO 0x154
#define R_EMPLOYEE_LEAVE 0x307
#define R_CHARACTER_APPEAR 0x19C
#define R_BLACKLIST_RESPONSE 0x415
#define S_MINIROOM 0x168



typedef struct session {
	unsigned int auth, recv_iv, send_iv, ver, size;
	unsigned char data[128 * 1024];
} session_t;
 
const unsigned char *aeskey = "\x88\x00\x00\x00\x6B\x00\x00\x00\xF9\x00\x00\x00\x71\x00\x00\x00\x0D\x00\x00\x00\x86\x00\x00\x00\xDB\x00\x00\x00\x4F\x00\x00\x00";
extern aes256_context ctx;
ADAPTER *npf_adapter;
char npf_device[64], npf_devicelist[4096];
session_t *session[65536];
HWND g_hSender, g_hMP;
unsigned int deletesel;

typedef struct
{
	unsigned int id;
	char nick[13];
}charinfo_t;


charinfo_t character_list[20];
unsigned int shopid[40];
char blacklist[20][13];


typedef struct {
	unsigned int size;
	unsigned char data[60];
} packet_t;

packet_t packet[4];



void MouseClick(unsigned int x, unsigned int y, unsigned int k)
{
	POINT xy = { x, y };
	ClientToScreen(g_hMP, &xy);
	SetCursorPos(xy.x, xy.y);
	Sleep(10);
	for (unsigned int i = 0; i < k; ++i)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
}



void PrepareShopPacket(void)
{
	//enter
	packet[0].size = 9;
	*(unsigned short*) &packet[0].data[0] = S_MINIROOM;
	packet[0].data[2] = 0x13;
	*(unsigned int*) &packet[0].data[3] = 0;
	*(unsigned short*) &packet[0].data[7] = 0;
	 
	//get blacklist
	packet[1].size = 3;
	*(unsigned short*) &packet[1].data[0] = S_MINIROOM;
	packet[1].data[2] = 0x38;

	//add blacklist
	*(unsigned short*) &packet[2].data[0] = S_MINIROOM;
	packet[2].data[2] = 0x39;

	///delete blacklist
	*(unsigned short*) &packet[3].data[0] = S_MINIROOM;
	packet[3].data[2] = 0x3A;
	  
	
}


void SendPacket(const void *data, unsigned int size)
{
	COPYDATASTRUCT cds;


	if (IsWindow(g_hSender) == FALSE)
		g_hSender = FindWindow("MapleStoryClass", NULL);
	if (g_hSender)
	{
		cds.dwData = 0x02100713;
		cds.cbData = size;
		cds.lpData = (void *) data;
		SendMessage(g_hSender, WM_COPYDATA, 0, (LPARAM) &cds);
	}
}


static __declspec(naked) unsigned int __fastcall byteswap_ulong(unsigned int value)
{
	__asm 
	{
		mov eax, ecx
		bswap eax
		retn
	}
}



void packet_handle(unsigned char *data, unsigned int size)
{
	static char buf[128];
	static char mynick[13];
	unsigned int i, j, count;
	static int flip, lastidx;
	
	
	if (*(unsigned short*) data == R_SERVERINFO) //serverinfo
	{
		memset(&character_list, 0, sizeof(character_list));
		memset(shopid, 0, sizeof(shopid));
		flip = 0;
		dbg("\n");
		Sleep(2000);
		
		if (*(unsigned short*) &data[11] == 1 && size >= 70)
		{
			for (i = 0; data[70 + i] != 0; ++i);
			for (i; data[70 + i] == 0; ++i);
			strcpy(mynick, &data[70 + i]);

		}
	
	}
	else if (*(unsigned short*) data == R_EMPLOYEE_LEAVE - 1) //EMPLOYEE_LEAVE - 1
	{
		j = *(unsigned short*) &data[16]; //닉네임 사이즈
		for (i = 0; i < _countof(shopid); ++i)
		{
			if (shopid[i] == 0)
			{
				shopid[i] = *(unsigned int*) &data[16 + 1 + j + 2];
				break;
			}
		}
		if (deletesel == 1) //블추만하기
		{
			*(unsigned int*) &packet[0].data[3] = shopid[i];
			SendPacket(packet[0].data, packet[0].size);
			Sleep(150);
#ifdef BLACK_ALL
			for (i = 0; i < _countof(character_list); ++i)
			{
				if (character_list[i].id != 0)
				{
					*(unsigned short*) &packet[2].data[3] = j = strlen(character_list[i].nick);
					packet[2].size = j + 5;
					memcpy(&packet[2].data[5], character_list[i].nick, j);
					SendPacket(packet[2].data, packet[2].size); //블추 함
					Sleep(20);
				}
			}
#endif
#ifndef BLACK_ALL
			for (i = 0; i < _countof(blacklist) && blacklist[i][0] != 0; ++i)
			{
				*(unsigned short*) &packet[2].data[3] = j = (unsigned short)strlen(blacklist[i]);
				packet[2].size = j + 5;
				memcpy(&packet[2].data[5], blacklist[i], j);
				SendPacket(packet[2].data, packet[2].size);
				Sleep(30);
			}
#endif
			
			Sleep(20);
			PostMessage(g_hMP, WM_KEYDOWN, VK_ESCAPE, 0);
			Sleep(20);
		}
		else
		{
			if (flip == 0)
			{
				flip = 1;
				*(unsigned int*) &packet[0].data[3] = shopid[i];
				++lastidx;
				SendPacket(packet[0].data, packet[0].size); //상점들어감
				Sleep(50);
				SendPacket(packet[1].data, packet[1].size); //블추 리스트 오게함
				Sleep(50);
			}
		}

	}
	else if (*(unsigned short*)data == R_BLACKLIST_RESPONSE) //블추 리스트 response
	{
		if (data[2] == 0x38)
		{
			if (deletesel == 1)
				return;
			for (i = 0; i < 3; ++i)
			{
				j = 5;
				for (count = 0; count < *(unsigned short*) &data[3]; ++count)
				{
					packet[3].data[3] = i = *(unsigned short*) &data[j];//닉사이즈
					packet[3].size = i + 5;
					memcpy(&packet[3].data[5], &data[j + 2], i);
					SendPacket(packet[3].data, packet[3].size); //블추삭제
					j += i + 2;
					Sleep(30);
				}
			}
			Sleep(100);
			if (deletesel == 3)
			{
#ifdef BLACK_ALL
				for (i = 0; i < _countof(character_list); ++i)
				{
					if (character_list[i].id != 0)
					{
						*(unsigned short*) &packet[2].data[3] = j = strlen(character_list[i].nick);
						packet[2].size = j + 5;
						memcpy(&packet[2].data[5], character_list[i].nick, j);
						SendPacket(packet[2].data, packet[2].size); //블추 함
						Sleep(50);
					}
				}
#endif
#ifndef BLACK_ALL
				for (i = 0; i < _countof(blacklist) && blacklist[i][0] != 0; ++i)
				{
					*(unsigned short*) &packet[2].data[3] = j = (unsigned short)strlen(blacklist[i]);
					packet[2].size = j + 5;
					memcpy(&packet[2].data[5], blacklist[i], j);
					SendPacket(packet[2].data, packet[2].size);
					Sleep(50);
				}
#endif
			}
			Sleep(50);
			MouseClick(403, 134, 1);
			Sleep(50);
			
			if (lastidx == _countof(shopid))
				return;
			for (i = lastidx;i < _countof(shopid) && shopid[i] == 0; ++i)
				if (i == _countof(shopid) - 1 && shopid[i] == 0)
					return;
			lastidx = i + 1;
			*(unsigned int*) &packet[0].data[3] = shopid[i];
			SendPacket(packet[0].data, packet[0].size); //상점 들어감
			Sleep(50);
			SendPacket(packet[1].data, packet[1].size); //블추 리스트 오게함
			Sleep(50);
		}
	}
	else if (*(unsigned short*) data == R_CHARACTER_APPEAR)
	{
		if (character_list[_countof(character_list) - 1].id != 0)
			memset(&character_list, 0, sizeof(character_list));
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == 0)
			{
				j = *(unsigned short*) &data[7];
				if (memcmp(&data[9], mynick, j) == 0) //내 캐릭은 블추 안함
					continue;
				memcpy(character_list[i].nick, &data[9], j);
				character_list[i].id = *(unsigned short*) &data[2];
				character_list[i].nick[j] = '\0';
				dbg("닉네임: %s\n", character_list[i].nick);
				break;
			}
		}
	}
	else if (*(unsigned short*) data == R_CHARACTER_APPEAR + 1)
	{
		for (i = 0; i < _countof(character_list); ++i)
		{
			if (character_list[i].id == *(unsigned int*) &data[2])
			{
				memset(&character_list[i], 0, sizeof(character_list[i]));
				break;
			}
		}
	}
	
}


unsigned int packet_parse(session_t *s, unsigned char *data, unsigned int size)
{
	unsigned int i, j, k;
	unsigned char pkt[65536];

	if (s->auth == 0u)
	{
		if ((j = *(unsigned __int16 *) data) + 2u > size)
			return 0;
		i = j + 2u;
		j = *(unsigned __int16 *) &data[4];
		s->send_iv = *(unsigned __int32 *) &data[j + 6u];
		s->recv_iv = *(unsigned __int32 *) &data[j + 10u];
		s->ver = atoi32((data[j + 6u] = 0u, (char *) &data[6]));
		s->auth = ~0u;
		//dbg("S=%08X R=%08X v%u\n", s->send_iv, s->recv_iv, s->ver & 65535);


		//dump(data, i);
	}
	else
		i = 0;

	k = s->recv_iv;

	for (; (size - i) >= (j = *(unsigned __int16 *) &data[i] ^ *(unsigned __int16 *) &data[i + 2u]) + 4u; i += j + 4u)
	{
		//dbg("%u bytes HEAD=%04X, EXPECTED=%04X\n", j, *(unsigned __int16 *)&data[i], (k >> 16) ^ (65535 & ~s->ver));
		wzcrypt_decrypt(j, k, &data[i + 4u], pkt); //size iv in out

		packet_handle(pkt, j);
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

	ih = (iphdr_t *) (pkt_data + 14u);
	th = (tcphdr_t *) (pkt_data + 14u + (ih->ihl << 2));

	if (th->fin != 0u || th->rst != 0u) {
		if ((s = session[th->dport]) != NULL) {
			session[th->dport] = NULL;
			HeapFree(GetProcessHeap(), 0u, s);
			dbg("%p %s\n", s, "인식종료");
		}
	}
	else {
		if (th->syn != 0u) {
			if ((s = session[th->dport]) == NULL) {
				if ((s = HeapAlloc(GetProcessHeap(), 0u, sizeof(session_t))) == NULL)
					__asm int 3;
				session[th->dport] = s;
			}
			dbg("%p %s\n", s, "인식시작");
			s->auth = 0u;
			s->size = 0u;
		}
		else if ((s = session[th->dport]) == NULL)
			return;
		if (i = ih->tot_len, (i = (((i & 0xFF) << 8) | (i >> 8)) - (j = (ih->ihl << 2) + (th->doff << 2))) != 0)
			packet_handler_sub(s, (char *) pkt_data + j + 14u, i);
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

	for (i = 0; *name; ++i)
	{
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
	static struct bpf_program bpf_code =
	{
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

	if ((adapter = PacketOpenAdapter(npf_device)) != NULL)
	{
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
	if (npf_adapter)
	{
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
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	NpfStop();

	return FALSE;
}

char* gen_uuid(char buffer[48]);

void getuuid(char *data)
{
	VIRTUALIZER3_START
	{
		if (*gen_uuid(data))
		{
			*(int *) &data[128] = ~_byteswap_ulong(((int *) data)[0]);
			*(int *) &data[132] = ~_byteswap_ulong(((int *) data)[1]);
			*(int *) &data[136] = ~_byteswap_ulong(((int *) data)[2]);
			*(int *) &data[140] = ~_byteswap_ulong(((int *) data)[3]);
			*(int *) &data[144] = ~_byteswap_ulong(((int *) data)[4]);
			*(int *) &data[148] = ~_byteswap_ulong(((int *) data)[5]);
			*(int *) &data[152] = ~_byteswap_ulong(((int *) data)[6]);
			*(int *) &data[156] = ~_byteswap_ulong(((int *) data)[7]);
			*(int *) &data[160] = ~_byteswap_ulong(((int *) data)[8]);
			*(int *) &data[164] = ~_byteswap_ulong(((int *) data)[9]);
		}
	}
	VIRTUALIZER3_END
	
}

int auth(char *data)
{
	int ret = 0;
	VIRTUALIZER2_START
	{
		if (strcmp(data, "6628FAC83923D1C364DB1C20F5E866EE9C1C7E6F") == 0 ||
		strcmp(data, "4890172D3C4845FDBF14F57349130264F5875DEB"/*채창병*/) == 0 || 
		strcmp(data, "F6A3ACD2982253E3CFEADA7B1C63DFC635939715"/*조커 유재석*/) == 0 ||
		strcmp(data, "A482613BABFD7D49F089299E356187D81FE08F22") == 0/*이뉴*/ ||
			strcmp(data, "8FC2BA0A6AC027A64AA1C3DD18E920B45FB18D0D") == 0)
			ret = 1;
	}
	VIRTUALIZER2_END
	return ret;
}

void main(void)
{
	char data[2048];
	char name[128];
	char desc[128];
	int sel;
	FILE *fp;

	g_hMP = FindWindow("MapleStoryClass", NULL);
	SetConsoleTitle("리듬 고상 블추하기");

	fp = fopen("블랙리스트.txt", "rt");
	if (fp == NULL)
	{
		printf("블랙리스트.txt 찾지 못함!");
		getchar();
		return;
	}
	for (int i = 0; i < _countof(blacklist) && fscanf(fp, "%s", blacklist[i]) != EOF; ++i);
	fclose(fp);
	
	 
	VIRTUALIZER_START
	{
		getuuid(data);
		if (auth(data) == 0)
			ExitProcess(0);
	}
	VIRTUALIZER_END

	SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE);

	int count = NpfFindAllDevices();

	PrepareShopPacket();

	dbg("1. 블추만 하기\n2. 블추 다 지우기\n3. 블추 다 지우고 블추하기\n");
	scanf("%d", &deletesel);


	for (int i = 1; i <= count; ++i)
	{
		NpfGetDeviceInfo(i, name, desc);
		dbg("%d. %s\n", i, desc);
	}
	dbg("입력: ");
	scanf("%d", &sel);
	NpfGetDeviceInfo(sel, name, desc);
	NpfSetDevice(name);

	aes256_init(&ctx, aeskey);

	if (NpfStart())
	{
		while (1)
			NpfCheckMessage();
	}

}

