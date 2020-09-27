#include "common.h"

aes256_context ctx;


static const unsigned char wzkey_table[256] =
{
	0xEC, 0x3F, 0x77, 0xA4, 0x45, 0xD0, 0x71, 0xBF, 0xB7, 0x98, 0x20, 0xFC, 0x4B, 0xE9, 0xB3, 0xE1, // 0
	0x5C, 0x22, 0xF7, 0x0C, 0x44, 0x1B, 0x81, 0xBD, 0x63, 0x8D, 0xD4, 0xC3, 0xF2, 0x10, 0x19, 0xE0, // 16
	0xFB, 0xA1, 0x6E, 0x66, 0xEA, 0xAE, 0xD6, 0xCE, 0x06, 0x18, 0x4E, 0xEB, 0x78, 0x95, 0xDB, 0xBA, // 32
	0xB6, 0x42, 0x7A, 0x2A, 0x83, 0x0B, 0x54, 0x67, 0x6D, 0xE8, 0x65, 0xE7, 0x2F, 0x07, 0xF3, 0xAA, // 48
	0x27, 0x7B, 0x85, 0xB0, 0x26, 0xFD, 0x8B, 0xA9, 0xFA, 0xBE, 0xA8, 0xD7, 0xCB, 0xCC, 0x92, 0xDA, // 64
	0xF9, 0x93, 0x60, 0x2D, 0xDD, 0xD2, 0xA2, 0x9B, 0x39, 0x5F, 0x82, 0x21, 0x4C, 0x69, 0xF8, 0x31, // 80
	0x87, 0xEE, 0x8E, 0xAD, 0x8C, 0x6A, 0xBC, 0xB5, 0x6B, 0x59, 0x13, 0xF1, 0x04, 0x00, 0xF6, 0x5A, // 96
	0x35, 0x79, 0x48, 0x8F, 0x15, 0xCD, 0x97, 0x57, 0x12, 0x3E, 0x37, 0xFF, 0x9D, 0x4F, 0x51, 0xF5, // 112
	0xA3, 0x70, 0xBB, 0x14, 0x75, 0xC2, 0xB8, 0x72, 0xC0, 0xED, 0x7D, 0x68, 0xC9, 0x2E, 0x0D, 0x62, // 128
	0x46, 0x17, 0x11, 0x4D, 0x6C, 0xC4, 0x7E, 0x53, 0xC1, 0x25, 0xC7, 0x9A, 0x1C, 0x88, 0x58, 0x2C, // 144
	0x89, 0xDC, 0x02, 0x64, 0x40, 0x01, 0x5D, 0x38, 0xA5, 0xE2, 0xAF, 0x55, 0xD5, 0xEF, 0x1A, 0x7C, // 160
	0xA7, 0x5B, 0xA6, 0x6F, 0x86, 0x9F, 0x73, 0xE6, 0x0A, 0xDE, 0x2B, 0x99, 0x4A, 0x47, 0x9C, 0xDF, // 176
	0x09, 0x76, 0x9E, 0x30, 0x0E, 0xE4, 0xB2, 0x94, 0xA0, 0x3B, 0x34, 0x1D, 0x28, 0x0F, 0x36, 0xE3, // 192
	0x23, 0xB4, 0x03, 0xD8, 0x90, 0xC8, 0x3C, 0xFE, 0x5E, 0x32, 0x24, 0x50, 0x1F, 0x3A, 0x43, 0x8A, // 224
	0x96, 0x41, 0x74, 0xAC, 0x52, 0x33, 0xF0, 0xD9, 0x29, 0x80, 0xB1, 0x16, 0xD3, 0xAB, 0x91, 0xB9, // 240
	0x84, 0x7F, 0x61, 0x1E, 0xCF, 0xC5, 0xD1, 0x56, 0x3D, 0xCA, 0xF4, 0x05, 0xC6, 0xE5, 0x08, 0x49, // 256
};

__declspec(naked) unsigned int __fastcall wzcrypt_nextiv(unsigned int seed /* ecx */)
{
	__asm {
		push edx;
		mov eax, 0xC650531F;

		// LOOP #1
		sub al, cl;
		ror eax, 8;
		movzx edx, cl;
		mov dl, [wzkey_table + edx];
		xor dl, 0x50;
		sub al, dl;
		ror eax, 8;
		mov edx, 0xB2;
		add dl, cl;
		xor al, dl;
		movzx edx, cl;
		mov dl, [wzkey_table + edx];
		shr ecx, 8;
		ror eax, 8;
		sub al, ah;
		add al, dl;
		ror eax, 5;

		// LOOP #2
		movzx edx, ah;
		sub al, cl;
		add al, [wzkey_table + edx];
		ror eax, 8;
		movzx edx, cl;
		mov dl, [wzkey_table + edx];
		xor dl, ah;
		sub al, dl;
		ror eax, 8;
		movzx edx, ah;
		mov dl, [wzkey_table + edx];
		add dl, cl;
		xor al, dl;
		ror eax, 8;
		sub al, ah;
		movzx edx, cl;
		add al, [wzkey_table + edx];
		shr ecx, 8;
		ror eax, 5;

		// LOOP #3
		movzx edx, ah;
		sub al, cl;
		add al, [wzkey_table + edx];
		ror eax, 8;
		movzx edx, cl;
		mov dl, [wzkey_table + edx];
		xor dl, ah;
		sub al, dl;
		ror eax, 8;
		movzx edx, ah;
		mov dl, [wzkey_table + edx];
		add dl, cl;
		xor al, dl;
		ror eax, 8;
		sub al, ah;
		movzx edx, cl;
		add al, [wzkey_table + edx];
		shr ecx, 8;
		ror eax, 5;

		// LOOP #4
		movzx edx, ah;
		sub al, cl;
		add al, [wzkey_table + edx];
		ror eax, 8;
		movzx edx, cl;
		mov dl, [wzkey_table + edx];
		xor dl, ah;
		sub al, dl;
		ror eax, 8;
		movzx edx, ah;
		mov dl, [wzkey_table + edx];
		add dl, cl;
		xor al, dl;
		ror eax, 8;
		sub al, ah;
		movzx edx, cl;
		add al, [wzkey_table + edx];
		ror eax, 5;

		pop edx;
		ret;
	}
}





void __fastcall wzcrypt_decrypt(unsigned int size, unsigned int iv, const unsigned char *in, unsigned char *out)
{
	unsigned char IV[4];
	unsigned int remaining = size;
	unsigned int length, idx;
	unsigned int start;
	unsigned char realIV[16];


	if (size == 0)
		return;
	*(unsigned int*) IV = iv;
	length = 0x5B0;
	start = 0;

	while (remaining > 0)
	{
		for (idx = 0; idx < sizeof(realIV); ++idx)
			realIV[idx] = IV[idx & 3];

		if (remaining < length)
			length = remaining;
		for (idx = start; idx < (start + length); ++idx)
		{
			if (((idx - start) & (sizeof(realIV) - 1)) == 0)
				aes256_encrypt_ecb(&ctx, realIV);
			out[idx] = in[idx] ^ realIV[(idx - start) & 15];
		}
		start += length;
		remaining -= length;
		length = 0x5B4;
	}

}

#if 0
__declspec(naked) void __fastcall wzcrypt_decrypt(unsigned int size /* ecx */, unsigned int iv /* edx */, const void *in, void *out)
{
	__asm
	{
		jecxz short $EXIT; //ecx가 0일때 exit으로 점프, 근데 size이므로 사이즈가 0일때 exit




	$CRYPT:
		//cmp 

		loop short $CRYPT;
	$EXIT:
		retn 8;
	}
}

#endif
#if 0
__declspec(naked) void __fastcall wzcrypt_decrypt(unsigned int size /* ecx */, unsigned int iv /* edx */, const void *in, void *out)
{


	__asm
	{
		jecxz short $EXIT; //ecx가 0일때 exit으로 점프, 근데 size이므로 사이즈가 0일때 exit

		push esi;
		mov esi, [esp + 4 + 4];


		push edi;
		mov edi, [esp + 8 + 8];
		push ebx;
		cld;

	$CRYPT:
		lodsb;
		movzx ebx, dl; //이게대충 iv iv를 ebx에 넣음
		xor al, [wzkey_table + ebx]; // byte v7 = (byte)(pBuffer[i] ^ sShiftKey[mIV[0]]);
		ror al, 4;
		mov ah, al;
		mov bl, al;
		shr al, 1;
		shl bl, 1;
		and bl, 0xAA;
		and al, 0x55;
		or al, bl;
		stosb;

		movzx ebx, dh;
		add dl, [wzkey_table + ebx];
		sub dl, al;
		ror edx, 8;
		movzx ebx, al;
		mov bl, [wzkey_table + ebx];
		xor bl, dh;
		sub dl, bl;
		ror edx, 8;
		movzx ebx, dh;
		mov bl, [wzkey_table + ebx];
		add bl, al;
		xor dl, bl;
		ror edx, 8;
		sub dl, dh;
		movzx ebx, al;
		add dl, [wzkey_table + ebx];
		ror edx, 5;
		loop short $CRYPT; //사이즈만큼 루프돎

		pop ebx;
		pop edi;
		pop esi;
	$EXIT:
		retn 8;
	}
}
#endif