
#ifndef uint8_t
#define uint8_t  unsigned char
#endif

#ifdef __cplusplus
extern "C"
{
#endif


	typedef struct {
		uint8_t key[32];
		uint8_t enckey[32];
		uint8_t deckey[32];
	} aes256_context;


	void aes256_init(aes256_context *, uint8_t * /* key */);
	void aes256_encrypt_ecb(aes256_context *, uint8_t * /* plaintext */);

#ifdef __cplusplus
}
#endif