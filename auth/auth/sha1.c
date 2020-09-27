#include "common.h"

#define ROTATE(x, n) (_rotl((x), (n))) /*(((x) << (n)) | ((x) >> (32u - (n))))*/
#define R1(x, y, z) ((((y) ^ (z)) & (x)) ^ (z))
#define R2(x, y, z) ((x) ^ (y) ^ (z))
#define R3(x, y, z) ((((x) | (y)) & (z)) |((x) & (y)))
#define R4(x, y, z) ((x) ^ (y) ^ (z))
#define STEP(f, a, b, c, d, e, x, ac) (e) += f((b), (c), (d)) + (x) + (ac) + ROTATE((a), 5), (b) = ROTATE((b), 30)

//#define blk(i) (b##i = _byteswap_ulong(p[i]))
#define blk00 (b00 = ROTATE(b13 ^ b08 ^ b02 ^ b00, 1))
#define blk01 (b01 = ROTATE(b14 ^ b09 ^ b03 ^ b01, 1))
#define blk02 (b02 = ROTATE(b15 ^ b10 ^ b04 ^ b02, 1))
#define blk03 (b03 = ROTATE(b00 ^ b11 ^ b05 ^ b03, 1))
#define blk04 (b04 = ROTATE(b01 ^ b12 ^ b06 ^ b04, 1))
#define blk05 (b05 = ROTATE(b02 ^ b13 ^ b07 ^ b05, 1))
#define blk06 (b06 = ROTATE(b03 ^ b14 ^ b08 ^ b06, 1))
#define blk07 (b07 = ROTATE(b04 ^ b15 ^ b09 ^ b07, 1))
#define blk08 (b08 = ROTATE(b05 ^ b00 ^ b10 ^ b08, 1))
#define blk09 (b09 = ROTATE(b06 ^ b01 ^ b11 ^ b09, 1))
#define blk10 (b10 = ROTATE(b07 ^ b02 ^ b12 ^ b10, 1))
#define blk11 (b11 = ROTATE(b08 ^ b03 ^ b13 ^ b11, 1))
#define blk12 (b12 = ROTATE(b09 ^ b04 ^ b14 ^ b12, 1))
#define blk13 (b13 = ROTATE(b10 ^ b05 ^ b15 ^ b13, 1))
#define blk14 (b14 = ROTATE(b11 ^ b06 ^ b00 ^ b14, 1))
#define blk15 (b15 = ROTATE(b12 ^ b07 ^ b01 ^ b15, 1))

void* sha1_body(sha1_t *sha1, const void *data, unsigned int size)
{
	unsigned int *p, a, b, c, d, e, aa, bb, cc, dd, ee, b00, b01, b02, b03, b04, b05, b06, b07, b08, b09, b10, b11, b12, b13, b14, b15;

	VIRTUALIZER_TIGER_BLACK_START
	{
		p = (unsigned int *) data;
		a = sha1->state[0];
		b = sha1->state[1];
		c = sha1->state[2];
		d = sha1->state[3];
		e = sha1->state[4];
	}
		VIRTUALIZER_TIGER_BLACK_END

		do {
			aa = a, bb = b, cc = c, dd = d, ee = e;
			STEP(R1, a, b, c, d, e, b00 = _byteswap_ulong(p[0]), 0x5A827999u);
			STEP(R1, e, a, b, c, d, b01 = _byteswap_ulong(p[1]), 0x5A827999u);
			STEP(R1, d, e, a, b, c, b02 = _byteswap_ulong(p[2]), 0x5A827999u);
			STEP(R1, c, d, e, a, b, b03 = _byteswap_ulong(p[3]), 0x5A827999u);
			STEP(R1, b, c, d, e, a, b04 = _byteswap_ulong(p[4]), 0x5A827999u);
			STEP(R1, a, b, c, d, e, b05 = _byteswap_ulong(p[5]), 0x5A827999u);
			STEP(R1, e, a, b, c, d, b06 = _byteswap_ulong(p[6]), 0x5A827999u);
			STEP(R1, d, e, a, b, c, b07 = _byteswap_ulong(p[7]), 0x5A827999u);
			STEP(R1, c, d, e, a, b, b08 = _byteswap_ulong(p[8]), 0x5A827999u);
			STEP(R1, b, c, d, e, a, b09 = _byteswap_ulong(p[9]), 0x5A827999u);
			STEP(R1, a, b, c, d, e, b10 = _byteswap_ulong(p[10]), 0x5A827999u);
			STEP(R1, e, a, b, c, d, b11 = _byteswap_ulong(p[11]), 0x5A827999u);
			STEP(R1, d, e, a, b, c, b12 = _byteswap_ulong(p[12]), 0x5A827999u);
			STEP(R1, c, d, e, a, b, b13 = _byteswap_ulong(p[13]), 0x5A827999u);
			STEP(R1, b, c, d, e, a, b14 = _byteswap_ulong(p[14]), 0x5A827999u);
			STEP(R1, a, b, c, d, e, b15 = _byteswap_ulong(p[15]), 0x5A827999u);
			STEP(R1, e, a, b, c, d, blk00, 0x5A827999u);
			STEP(R1, d, e, a, b, c, blk01, 0x5A827999u);
			STEP(R1, c, d, e, a, b, blk02, 0x5A827999u);
			STEP(R1, b, c, d, e, a, blk03, 0x5A827999u);
			STEP(R2, a, b, c, d, e, blk04, 0x6ED9EBA1u);
			STEP(R2, e, a, b, c, d, blk05, 0x6ED9EBA1u);
			STEP(R2, d, e, a, b, c, blk06, 0x6ED9EBA1u);
			STEP(R2, c, d, e, a, b, blk07, 0x6ED9EBA1u);
			STEP(R2, b, c, d, e, a, blk08, 0x6ED9EBA1u);
			STEP(R2, a, b, c, d, e, blk09, 0x6ED9EBA1u);
			STEP(R2, e, a, b, c, d, blk10, 0x6ED9EBA1u);
			STEP(R2, d, e, a, b, c, blk11, 0x6ED9EBA1u);
			STEP(R2, c, d, e, a, b, blk12, 0x6ED9EBA1u);
			STEP(R2, b, c, d, e, a, blk13, 0x6ED9EBA1u);
			STEP(R2, a, b, c, d, e, blk14, 0x6ED9EBA1u);
			STEP(R2, e, a, b, c, d, blk15, 0x6ED9EBA1u);
			STEP(R2, d, e, a, b, c, blk00, 0x6ED9EBA1u);
			STEP(R2, c, d, e, a, b, blk01, 0x6ED9EBA1u);
			STEP(R2, b, c, d, e, a, blk02, 0x6ED9EBA1u);
			STEP(R2, a, b, c, d, e, blk03, 0x6ED9EBA1u);
			STEP(R2, e, a, b, c, d, blk04, 0x6ED9EBA1u);
			STEP(R2, d, e, a, b, c, blk05, 0x6ED9EBA1u);
			STEP(R2, c, d, e, a, b, blk06, 0x6ED9EBA1u);
			STEP(R2, b, c, d, e, a, blk07, 0x6ED9EBA1u);
			STEP(R3, a, b, c, d, e, blk08, 0x8F1BBCDCu);
			STEP(R3, e, a, b, c, d, blk09, 0x8F1BBCDCu);
			STEP(R3, d, e, a, b, c, blk10, 0x8F1BBCDCu);
			STEP(R3, c, d, e, a, b, blk11, 0x8F1BBCDCu);
			STEP(R3, b, c, d, e, a, blk12, 0x8F1BBCDCu);
			STEP(R3, a, b, c, d, e, blk13, 0x8F1BBCDCu);
			STEP(R3, e, a, b, c, d, blk14, 0x8F1BBCDCu);
			STEP(R3, d, e, a, b, c, blk15, 0x8F1BBCDCu);
			STEP(R3, c, d, e, a, b, blk00, 0x8F1BBCDCu);
			STEP(R3, b, c, d, e, a, blk01, 0x8F1BBCDCu);
			STEP(R3, a, b, c, d, e, blk02, 0x8F1BBCDCu);
			STEP(R3, e, a, b, c, d, blk03, 0x8F1BBCDCu);
			STEP(R3, d, e, a, b, c, blk04, 0x8F1BBCDCu);
			STEP(R3, c, d, e, a, b, blk05, 0x8F1BBCDCu);
			STEP(R3, b, c, d, e, a, blk06, 0x8F1BBCDCu);
			STEP(R3, a, b, c, d, e, blk07, 0x8F1BBCDCu);
			STEP(R3, e, a, b, c, d, blk08, 0x8F1BBCDCu);
			STEP(R3, d, e, a, b, c, blk09, 0x8F1BBCDCu);
			STEP(R3, c, d, e, a, b, blk10, 0x8F1BBCDCu);
			STEP(R3, b, c, d, e, a, blk11, 0x8F1BBCDCu);
			STEP(R4, a, b, c, d, e, blk12, 0xCA62C1D6u);
			STEP(R4, e, a, b, c, d, blk13, 0xCA62C1D6u);
			STEP(R4, d, e, a, b, c, blk14, 0xCA62C1D6u);
			STEP(R4, c, d, e, a, b, blk15, 0xCA62C1D6u);
			STEP(R4, b, c, d, e, a, blk00, 0xCA62C1D6u);
			STEP(R4, a, b, c, d, e, blk01, 0xCA62C1D6u);
			STEP(R4, e, a, b, c, d, blk02, 0xCA62C1D6u);
			STEP(R4, d, e, a, b, c, blk03, 0xCA62C1D6u);
			STEP(R4, c, d, e, a, b, blk04, 0xCA62C1D6u);
			STEP(R4, b, c, d, e, a, blk05, 0xCA62C1D6u);
			STEP(R4, a, b, c, d, e, blk06, 0xCA62C1D6u);
			STEP(R4, e, a, b, c, d, blk07, 0xCA62C1D6u);
			STEP(R4, d, e, a, b, c, blk08, 0xCA62C1D6u);
			STEP(R4, c, d, e, a, b, blk09, 0xCA62C1D6u);
			STEP(R4, b, c, d, e, a, blk10, 0xCA62C1D6u);
			STEP(R4, a, b, c, d, e, blk11, 0xCA62C1D6u);
			STEP(R4, e, a, b, c, d, blk12, 0xCA62C1D6u);
			STEP(R4, d, e, a, b, c, blk13, 0xCA62C1D6u);
			STEP(R4, c, d, e, a, b, blk14, 0xCA62C1D6u);
			STEP(R4, b, c, d, e, a, blk15, 0xCA62C1D6u);
			a += aa, b += bb, c += cc, d += dd, e += ee, p += 16;
		} while (size -= 64u);

		VIRTUALIZER_TIGER_BLACK_START
		{
			sha1->state[0] = a;
			sha1->state[1] = b;
			sha1->state[2] = c;
			sha1->state[3] = d;
			sha1->state[4] = e;
		}
			VIRTUALIZER_TIGER_BLACK_END
			return p;
}

void sha1_init(sha1_t *sha1)
{

	sha1->state[0] = 0x67452301;
	sha1->state[1] = 0xEFCDAB89;
	sha1->state[2] = 0x98BADCFE;
	sha1->state[3] = 0x10325476;
	sha1->state[4] = 0xC3D2E1F0;
	sha1->bytes = 0;

}

void sha1_update(sha1_t *sha1, const void *data, unsigned int size)
{
	unsigned int i, j;

	VIRTUALIZER_TIGER_BLACK_START
	{
		if (i = (unsigned int) sha1->bytes & 63, sha1->bytes += size, i) {
			if (size >= (j = 64u - i)) {
				memcpy(&sha1->buffer[i], data, j);
				sha1_body(sha1, sha1->buffer, 64);
				data = (void *) &((char *) data)[j];
				size -= j;
				if ((i = size & ~63) != 0) {
					sha1_body(sha1, data, i);
					memcpy(sha1->buffer, (void *) &((char *) data)[i], size - i);
				}
				else
					memcpy(sha1->buffer, data, size);
			}
			else
				memcpy(&sha1->buffer[i], data, size);
		}
		else if ((i = size & ~63) != 0) {
			sha1_body(sha1, data, i);
			memcpy(sha1->buffer, (void *) &((char *) data)[i], size - i);
		}
		else
			memcpy(sha1->buffer, data, size);
	}
		VIRTUALIZER_TIGER_BLACK_END
}

void sha1_final(sha1_t *sha1)
{
	unsigned int i, j;

	VIRTUALIZER_TIGER_BLACK_START
	{
		i = (unsigned int) sha1->bytes & 63;
		sha1->buffer[i] = 128u;
		j = 64u - ++i;
		memset(&sha1->buffer[i], 0, j);

		if (j < 8u) {
			sha1_body(sha1, sha1->buffer, 64);
			memset(sha1->buffer, 0, 56);
		}

		sha1->bytes <<= 3;
		*(unsigned int *) &sha1->buffer[56] = (i = (unsigned int) (sha1->bytes >> 32), _byteswap_ulong(i));
		*(unsigned int *) &sha1->buffer[60] = (i = (unsigned int) sha1->bytes, _byteswap_ulong(i));
		sha1_body(sha1, sha1->buffer, 64);
	}
		VIRTUALIZER_TIGER_BLACK_END
}

char* sha1_tostring(sha1_t *sha1, char *string)
{
	string[0x00] = "0123456789ABCDEF"[(sha1->state[0] >> 28) & 15];
	string[0x01] = "0123456789ABCDEF"[(sha1->state[0] >> 24) & 15];
	string[0x02] = "0123456789ABCDEF"[(sha1->state[0] >> 20) & 15];
	string[0x03] = "0123456789ABCDEF"[(sha1->state[0] >> 16) & 15];
	string[0x04] = "0123456789ABCDEF"[(sha1->state[0] >> 12) & 15];
	string[0x05] = "0123456789ABCDEF"[(sha1->state[0] >> 8) & 15];
	string[0x06] = "0123456789ABCDEF"[(sha1->state[0] >> 4) & 15];
	string[0x07] = "0123456789ABCDEF"[(sha1->state[0] >> 0) & 15];
	string[0x08] = "0123456789ABCDEF"[(sha1->state[1] >> 28) & 15];
	string[0x09] = "0123456789ABCDEF"[(sha1->state[1] >> 24) & 15];
	string[0x0a] = "0123456789ABCDEF"[(sha1->state[1] >> 20) & 15];
	string[0x0b] = "0123456789ABCDEF"[(sha1->state[1] >> 16) & 15];
	string[0x0c] = "0123456789ABCDEF"[(sha1->state[1] >> 12) & 15];
	string[0x0d] = "0123456789ABCDEF"[(sha1->state[1] >> 8) & 15];
	string[0x0e] = "0123456789ABCDEF"[(sha1->state[1] >> 4) & 15];
	string[0x0f] = "0123456789ABCDEF"[(sha1->state[1] >> 0) & 15];
	string[0x10] = "0123456789ABCDEF"[(sha1->state[2] >> 28) & 15];
	string[0x11] = "0123456789ABCDEF"[(sha1->state[2] >> 24) & 15];
	string[0x12] = "0123456789ABCDEF"[(sha1->state[2] >> 20) & 15];
	string[0x13] = "0123456789ABCDEF"[(sha1->state[2] >> 16) & 15];
	string[0x14] = "0123456789ABCDEF"[(sha1->state[2] >> 12) & 15];
	string[0x15] = "0123456789ABCDEF"[(sha1->state[2] >> 8) & 15];
	string[0x16] = "0123456789ABCDEF"[(sha1->state[2] >> 4) & 15];
	string[0x17] = "0123456789ABCDEF"[(sha1->state[2] >> 0) & 15];
	string[0x18] = "0123456789ABCDEF"[(sha1->state[3] >> 28) & 15];
	string[0x19] = "0123456789ABCDEF"[(sha1->state[3] >> 24) & 15];
	string[0x1a] = "0123456789ABCDEF"[(sha1->state[3] >> 20) & 15];
	string[0x1b] = "0123456789ABCDEF"[(sha1->state[3] >> 16) & 15];
	string[0x1c] = "0123456789ABCDEF"[(sha1->state[3] >> 12) & 15];
	string[0x1d] = "0123456789ABCDEF"[(sha1->state[3] >> 8) & 15];
	string[0x1e] = "0123456789ABCDEF"[(sha1->state[3] >> 4) & 15];
	string[0x1f] = "0123456789ABCDEF"[(sha1->state[3] >> 0) & 15];
	string[0x20] = "0123456789ABCDEF"[(sha1->state[4] >> 28) & 15];
	string[0x21] = "0123456789ABCDEF"[(sha1->state[4] >> 24) & 15];
	string[0x22] = "0123456789ABCDEF"[(sha1->state[4] >> 20) & 15];
	string[0x23] = "0123456789ABCDEF"[(sha1->state[4] >> 16) & 15];
	string[0x24] = "0123456789ABCDEF"[(sha1->state[4] >> 12) & 15];
	string[0x25] = "0123456789ABCDEF"[(sha1->state[4] >> 8) & 15];
	string[0x26] = "0123456789ABCDEF"[(sha1->state[4] >> 4) & 15];
	string[0x27] = "0123456789ABCDEF"[(sha1->state[4] >> 0) & 15];
	string[0x28] = '\0';
	return string;
}

char* sha1(char *string, const void *data, unsigned int size)
{
	sha1_t sha1;
	sha1_init(&sha1);
	sha1_update(&sha1, data, size);
	sha1_final(&sha1);
	return sha1_tostring(&sha1, string);
}