#define SM3_DIGEST_LENGTH	32
#define SM3_BLOCK_SIZE		64
#define SM3_HMAC_SIZE		(SM3_DIGEST_LENGTH)

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


	typedef struct {
		uint32_t digest[8];
		int nblocks;
		unsigned char block[64];
		int num;
	} fastsm3_ctx_t;

	void fast_sm3_init(fastsm3_ctx_t* ctx);
	void fast_sm3_update(fastsm3_ctx_t* ctx, const unsigned char* data, size_t data_len);
	void fast_sm3_final(fastsm3_ctx_t* ctx, unsigned char digest[SM3_DIGEST_LENGTH]);
	void fast_sm3_compress(uint32_t digest[8], const unsigned char block[SM3_BLOCK_SIZE]);
	void fast_sm3(const unsigned char* data, size_t datalen,
		unsigned char digest[SM3_DIGEST_LENGTH]);

#ifdef __cplusplus
}
#endif


