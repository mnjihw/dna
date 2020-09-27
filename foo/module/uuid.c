#include "common.h"

#define IDE_ATA_IDENTIFY 0xEC
/*
#define SMART_GET_VERSION 0x74080
#define SMART_RCV_DRIVE_DATA 0x7C088
*/

#pragma pack(1)

/*
typedef struct _DRIVERSTATUS {
BYTE bDriverError;
BYTE bIDEError;
BYTE bReserved[2];
DWORD dwReserved[2];
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

typedef struct _GETVERSIONINPARAMS {
BYTE bVersion;
BYTE bRevision;
BYTE bReserved;
BYTE bIDEDeviceMap;
DWORD fCapabilities;
DWORD dwReserved[4];
} GETVERSIONINPARAMS, *PGETVERSIONINPARAMS, *LPGETVERSIONINPARAMS;

typedef struct _IDEREGS {
BYTE bFeaturesReg;
BYTE bSectorCountReg;
BYTE bSectorNumberReg;
BYTE bCylLowReg;
BYTE bCylHighReg;
BYTE bDriveHeadReg;
BYTE bCommandReg;
BYTE bReserved;
} IDEREGS, *PIDEREGS, *LPIDEREGS;

typedef struct _SENDCMDINPARAMS {
DWORD   cBufferSize;
IDEREGS irDriveRegs;
BYTE   bDriveNumber;
BYTE   bReserved[3];
DWORD   dwReserved[4];
BYTE   bBuffer[1];
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;

typedef struct _SENDCMDOUTPARAMS {
DWORD        cBufferSize;
DRIVERSTATUS DriverStatus;
BYTE        bBuffer[1];
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;
*/

typedef struct _IDSECTOR {
	WORD wGenConfig;
	WORD wNumCyls;
	WORD wReserved;
	WORD wNumHeads;
	WORD wBytesPerTrack;
	WORD wBytesPerSector;
	WORD wSectorsPerTrack;
	WORD wVendorUnique[3];
	BYTE sSerialNumber[20];
	WORD wBufferType;
	WORD wBufferSize;
	WORD wECCSize;
	BYTE sFirmwareRev[8];
	BYTE sModelNumber[40];
	WORD wMoreVendorUnique;
	WORD wDoubleWordIO;
	WORD wCapabilities;
	WORD wReserved1;
	WORD wPIOTiming;
	WORD wDMATiming;
	WORD wBS;
	WORD wNumCurrentCyls;
	WORD wNumCurrentHeads;
	WORD wNumCurrentSectorsPerTrack;
	DWORD ulCurrentSectorCapacity;
	WORD wMultSectorStuff;
	DWORD ulTotalAddressableSectors;
	WORD wSingleWordDMA;
	WORD wMultiWordDMA;
	BYTE bReserved[128];
} IDSECTOR, *PIDSECTOR;

typedef BYTE SMSTR;
typedef WORD SMHNDL;

typedef struct {
	BYTE Used20CallingMethod;
	BYTE SMBIOSMajorVersion;
	BYTE SMBIOSMinorVersion;
	BYTE DmiRevision;
	DWORD Length;
	BYTE SMBIOSTableData[1];
} SMBIOS;

typedef struct {
	BYTE Type;
	BYTE Length;
	WORD Handle;
} SMBIOS_Header;

typedef struct {
	SMBIOS_Header Header;
	SMSTR Vendor;
	SMSTR Version;
	WORD AddressSegment; // real-mode segment 
	SMSTR ReleaseDate;
	BYTE ROMSize; // size/64k - 1 
	BYTE Characteristics[10];
	BYTE MajorRelease;
	BYTE MinorRelease;
	BYTE FirwareMajorRelease;
	BYTE FirwareMinorRelease;
} SMBIOS_BIOS_Info; // Type 0

typedef struct {
	SMBIOS_Header Header;
	SMSTR Vendor;
	SMSTR Product;
	SMSTR Version;
	SMSTR Serial;
	BYTE UUID[16];
	BYTE WakeupType;
	SMSTR SKU;
	SMSTR Family;
} SMBIOS_System_Info; // Type 1

typedef struct {
	SMBIOS_Header Header;
	SMSTR Manufacturer;
	SMSTR Product;
	SMSTR Version;
	SMSTR Serial;
	SMSTR AssetTag;
	BYTE FeatureFlags;
	SMSTR Location;
	SMHNDL ChassisHandle;
	BYTE BoardType;
	SMHNDL ContainedHandles;
	SMHNDL Handles[1]; // open array of ContainedHandles WORDs 
} SMBIOS_BaseBoard_Info; // Type 2
#pragma pack()

int uuid_check_smbios(void)
{
	unsigned int i, j, k, n;
	char *strings[16];
	char data[16384];

	/* 일부러 완전히 초기화했음 */
	memset(data, 0, sizeof(data));


	GetSystemFirmwareTable('RSMB', 0, data, sizeof(data));
	*(int *) data = 0;

	for (i = 8; (j = ((SMBIOS_Header *) &data[i])->Length) >= sizeof(SMBIOS_Header); i += j + k) {
		n = 0;

		if (*(unsigned short *) &data[i + j]) {
			for (k = 0; data[i + j + k]; k += strlen(&data[i + j + k]) + 1u)
				if (n != sizeof(strings) / sizeof(strings[0]))
					strings[n++] = &data[i + j + k];
			++k;
		}
		else
			k = 2;

		if (((SMBIOS_Header *) &data[i])->Type == 0 || ((SMBIOS_Header *) &data[i])->Type == 1 || ((SMBIOS_Header *) &data[i])->Type == 2)
			*(int *) data += 1;
	}


	return *(int *) data;
}

__declspec(naked) void nop13()
{
	VIRTUALIZER_SHARK_WHITE_START
	{
		FILL_NOP(NOP_SIZE);
	}
	VIRTUALIZER_SHARK_WHITE_END
}

void uuid_smbios(sha1_t *sha1)
{
	unsigned int i, j, k, n;
	char *strings[16];
	char data[16384];

	/* 일부러 완전히 초기화했음 */
	memset(data, 0, sizeof(data));

	GetSystemFirmwareTable('RSMB', 0, data, sizeof(data));

	for (i = 8; (j = ((SMBIOS_Header *) &data[i])->Length) >= sizeof(SMBIOS_Header); i += j + k) {
		n = 0;

		if (*(unsigned short *) &data[i + j]) {
			for (k = 0; data[i + j + k]; k += strlen(&data[i + j + k]) + 1u)
				if (n != sizeof(strings) / sizeof(strings[0]))
					strings[n++] = &data[i + j + k];
			++k;
		}
		else
			k = 2;

		if (((SMBIOS_Header *) &data[i])->Type == 0) {
			if (((SMBIOS_BIOS_Info *) &data[i])->Vendor && ((SMBIOS_BIOS_Info *) &data[i])->Vendor <= n)
				sha1_update(sha1, strings[((SMBIOS_BIOS_Info *) &data[i])->Vendor - 1u], strlen(strings[((SMBIOS_BIOS_Info *) &data[i])->Vendor - 1u]));
			sha1->state[0] ^= ((SMBIOS_BIOS_Info *) &data[i])->ROMSize;
			sha1->state[1] ^= ((SMBIOS_BIOS_Info *) &data[i])->ROMSize;
			sha1->state[2] ^= ((SMBIOS_BIOS_Info *) &data[i])->ROMSize;
			sha1->state[3] ^= ((SMBIOS_BIOS_Info *) &data[i])->ROMSize;
			sha1->state[4] ^= ((SMBIOS_BIOS_Info *) &data[i])->ROMSize;
			break;
		}
	}

	for (i = 8; (j = ((SMBIOS_Header *) &data[i])->Length) >= sizeof(SMBIOS_Header); i += j + k) {
		n = 0;

		if (*(unsigned short *) &data[i + j]) {
			for (k = 0; data[i + j + k]; k += strlen(&data[i + j + k]) + 1u)
				if (n != sizeof(strings) / sizeof(strings[0]))
					strings[n++] = &data[i + j + k];
			++k;
		}
		else
			k = 2;

		if (((SMBIOS_Header *) &data[i])->Type == 1) {
			if (((SMBIOS_System_Info *) &data[i])->Vendor && ((SMBIOS_System_Info *) &data[i])->Vendor <= n)
				sha1_update(sha1, strings[((SMBIOS_System_Info *) &data[i])->Vendor - 1u], strlen(strings[((SMBIOS_System_Info *) &data[i])->Vendor - 1u]));
			if (((SMBIOS_System_Info *) &data[i])->Product && ((SMBIOS_System_Info *) &data[i])->Product <= n)
				sha1_update(sha1, strings[((SMBIOS_System_Info *) &data[i])->Product - 1u], strlen(strings[((SMBIOS_System_Info *) &data[i])->Product - 1u]));
			if (((SMBIOS_System_Info *) &data[i])->Serial && ((SMBIOS_System_Info *) &data[i])->Serial <= n)
				sha1_update(sha1, strings[((SMBIOS_System_Info *) &data[i])->Serial - 1u], strlen(strings[((SMBIOS_System_Info *) &data[i])->Serial - 1u]));
			sha1_update(sha1, ((SMBIOS_System_Info *) &data[i])->UUID, sizeof(((SMBIOS_System_Info *) &data[i])->UUID));
			break;
		}
	}

	for (i = 8; (j = ((SMBIOS_Header *) &data[i])->Length) >= sizeof(SMBIOS_Header); i += j + k) {
		n = 0;

		if (*(unsigned short *) &data[i + j]) {
			for (k = 0; data[i + j + k]; k += strlen(&data[i + j + k]) + 1u)
				if (n != sizeof(strings) / sizeof(strings[0]))
					strings[n++] = &data[i + j + k];
			++k;
		}
		else
			k = 2;

		if (((SMBIOS_Header *) &data[i])->Type == 2) {
			if (((SMBIOS_BaseBoard_Info *) &data[i])->Manufacturer && ((SMBIOS_BaseBoard_Info *) &data[i])->Manufacturer <= n)
				sha1_update(sha1, strings[((SMBIOS_BaseBoard_Info *) &data[i])->Manufacturer - 1u], strlen(strings[((SMBIOS_BaseBoard_Info *) &data[i])->Manufacturer - 1u]));
			if (((SMBIOS_BaseBoard_Info *) &data[i])->Product && ((SMBIOS_BaseBoard_Info *) &data[i])->Product <= n)
				sha1_update(sha1, strings[((SMBIOS_BaseBoard_Info *) &data[i])->Product - 1u], strlen(strings[((SMBIOS_BaseBoard_Info *) &data[i])->Product - 1u]));
			if (((SMBIOS_BaseBoard_Info *) &data[i])->Serial && ((SMBIOS_BaseBoard_Info *) &data[i])->Serial <= n)
				sha1_update(sha1, strings[((SMBIOS_BaseBoard_Info *) &data[i])->Serial - 1u], strlen(strings[((SMBIOS_BaseBoard_Info *) &data[i])->Serial - 1u]));
			break;
		}
	}

}



__declspec(naked) void __cdecl cpuid(void *data, unsigned int code)
{
	__asm {

		push ebp
			push ebx
			mov eax, [esp + 10h]
			cpuid
			mov ebp, [esp + 0Ch]
			mov[ebp + 0], eax
			mov[ebp + 4], ebx
			mov[ebp + 8], ecx
			mov[ebp + 0Ch], edx
			pop ebx
			pop ebp

			ret
	}
}




char* gen_uuid(char buffer[48])
{
	sha1_t sha1;
	char data[64];

	VIRTUALIZER_TIGER_BLACK_START
	{
		*buffer = '\0';
		if (uuid_check_smbios() == 3)
		{
			memset(&sha1, 0, sizeof(sha1));
			sha1_init(&sha1);
			sha1_update(&sha1, &sha1, sizeof(sha1));
			sha1_update(&sha1, &sha1, sizeof(sha1));
			sha1_final(&sha1);


			if (crcsum32(0xBADF00Du, &sha1, sizeof(sha1)) == 0x33999385u)
			{

				memcpy(data, sha1.state, sizeof(sha1.state));
				sha1_init(&sha1);
				sha1_update(&sha1, data, sizeof(sha1.state));
				sha1_update(&sha1, &sha1, sizeof(sha1));
				sha1_final(&sha1);

				if (crcsum32(0xBADF00Du, &sha1, sizeof(sha1)) == 0xC5C6A8BFu)
				{

					sha1_init(&sha1);
					__cpuid((memset(&data, 0, 16), data), 0);
					sha1_update(&sha1, data, 16);
					__cpuid((memset(&data, 0, 16), data), 0x80000002u);
					sha1_update(&sha1, data, 16);
					__cpuid((memset(&data, 0, 16), data), 0x80000003u);
					sha1_update(&sha1, data, 16);
					__cpuid((memset(&data, 0, 16), data), 0x80000004u);
					sha1_update(&sha1, data, 16);
					GlobalMemoryStatusEx((memset(data, 0, sizeof(MEMORYSTATUSEX)), ((MEMORYSTATUSEX *) data)->dwLength = sizeof(MEMORYSTATUSEX), (MEMORYSTATUSEX *) data));
					sha1_update(&sha1, &((MEMORYSTATUSEX *) data)->ullTotalPhys, 8u);
					GetNativeSystemInfo((memset(data, 0, sizeof(SYSTEM_INFO)), (SYSTEM_INFO *) data));
					sha1_update(&sha1, data, sizeof(SYSTEM_INFO));
					uuid_smbios(&sha1);
					sha1_final(&sha1);
					*(int *) &data[0] = 'DCBA'; /* customized hex string */
					*(int *) &data[4] = '10FE';
					*(int *) &data[8] = '5432';
					*(int *) &data[12] = '9876';
					buffer[0x00] = data[(sha1.state[0] >> 0) & 15];
					buffer[0x01] = data[(sha1.state[0] >> 4) & 15];
					buffer[0x02] = data[(sha1.state[0] >> 8) & 15];
					buffer[0x03] = data[(sha1.state[0] >> 12) & 15];
					buffer[0x04] = data[(sha1.state[0] >> 16) & 15];
					buffer[0x05] = data[(sha1.state[0] >> 20) & 15];
					buffer[0x06] = data[(sha1.state[0] >> 24) & 15];
					buffer[0x07] = data[(sha1.state[0] >> 28) & 15];
					buffer[0x08] = data[(sha1.state[1] >> 0) & 15];
					buffer[0x09] = data[(sha1.state[1] >> 4) & 15];
					buffer[0x0A] = data[(sha1.state[1] >> 8) & 15];
					buffer[0x0B] = data[(sha1.state[1] >> 12) & 15];
					buffer[0x0C] = data[(sha1.state[1] >> 16) & 15];
					buffer[0x0D] = data[(sha1.state[1] >> 20) & 15];
					buffer[0x0E] = data[(sha1.state[1] >> 24) & 15];
					buffer[0x0F] = data[(sha1.state[1] >> 28) & 15];
					buffer[0x10] = data[(sha1.state[2] >> 0) & 15];
					buffer[0x11] = data[(sha1.state[2] >> 4) & 15];
					buffer[0x12] = data[(sha1.state[2] >> 8) & 15];
					buffer[0x13] = data[(sha1.state[2] >> 12) & 15];
					buffer[0x14] = data[(sha1.state[2] >> 16) & 15];
					buffer[0x15] = data[(sha1.state[2] >> 20) & 15];
					buffer[0x16] = data[(sha1.state[2] >> 24) & 15];
					buffer[0x17] = data[(sha1.state[2] >> 28) & 15];
					buffer[0x18] = data[(sha1.state[3] >> 0) & 15];
					buffer[0x19] = data[(sha1.state[3] >> 4) & 15];
					buffer[0x1A] = data[(sha1.state[3] >> 8) & 15];
					buffer[0x1B] = data[(sha1.state[3] >> 12) & 15];
					buffer[0x1C] = data[(sha1.state[3] >> 16) & 15];
					buffer[0x1D] = data[(sha1.state[3] >> 20) & 15];
					buffer[0x1E] = data[(sha1.state[3] >> 24) & 15];
					buffer[0x1F] = data[(sha1.state[3] >> 28) & 15];
					buffer[0x20] = data[(sha1.state[4] >> 0) & 15];
					buffer[0x21] = data[(sha1.state[4] >> 4) & 15];
					buffer[0x22] = data[(sha1.state[4] >> 8) & 15];
					buffer[0x23] = data[(sha1.state[4] >> 12) & 15];
					buffer[0x24] = data[(sha1.state[4] >> 16) & 15];
					buffer[0x25] = data[(sha1.state[4] >> 20) & 15];
					buffer[0x26] = data[(sha1.state[4] >> 24) & 15];
					buffer[0x27] = data[(sha1.state[4] >> 28) & 15];
					buffer[0x28] = '\0';
				}
			}
		}
	}
		VIRTUALIZER_TIGER_BLACK_END

		return buffer;
} 