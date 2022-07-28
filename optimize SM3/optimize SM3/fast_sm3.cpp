#include <string.h>
#include "immintrin.h"
#include "byteorder.h"
#include "fast_sm3.h"


void fast_sm3_init(fastsm3_ctx_t* ctx)
{
	ctx->digest[0] = 0x7380166F;
	ctx->digest[1] = 0x4914B2B9;
	ctx->digest[2] = 0x172442D7;
	ctx->digest[3] = 0xDA8A0600;
	ctx->digest[4] = 0xA96F30BC;
	ctx->digest[5] = 0x163138AA;
	ctx->digest[6] = 0xE38DEE4D;
	ctx->digest[7] = 0xB0FB0E4E;

	ctx->nblocks = 0;
	ctx->num = 0;
}

void fast_sm3_update(fastsm3_ctx_t* ctx, const unsigned char* data, size_t data_len)
{
	if (ctx->num) {
		unsigned int left = SM3_BLOCK_SIZE - ctx->num;
		if (data_len < left) {
			memcpy(ctx->block + ctx->num, data, data_len);
			ctx->num += data_len;
			return;
		}
		else {
			memcpy(ctx->block + ctx->num, data, left);
			fast_sm3_compress(ctx->digest, ctx->block);
			ctx->nblocks++;
			data += left;
			data_len -= left;
		}
	}
	while (data_len >= SM3_BLOCK_SIZE) {
		fast_sm3_compress(ctx->digest, data);
		ctx->nblocks++;
		data += SM3_BLOCK_SIZE;
		data_len -= SM3_BLOCK_SIZE;
	}
	ctx->num = data_len;
	if (data_len) {
		memcpy(ctx->block, data, data_len);
	}
}

void fast_sm3_final(fastsm3_ctx_t* ctx, unsigned char* digest)
{
	int i;
	uint32_t* pdigest = (uint32_t*)digest;
	uint32_t* count = (uint32_t*)(ctx->block + SM3_BLOCK_SIZE - 8);

	ctx->block[ctx->num] = 0x80;

	if (ctx->num + 9 <= SM3_BLOCK_SIZE) {
		memset(ctx->block + ctx->num + 1, 0, SM3_BLOCK_SIZE - ctx->num - 9);
	}
	else {
		memset(ctx->block + ctx->num + 1, 0, SM3_BLOCK_SIZE - ctx->num - 1);
		fast_sm3_compress(ctx->digest, ctx->block);
		memset(ctx->block, 0, SM3_BLOCK_SIZE - 8);
	}

	count[0] = cpu_to_be32((ctx->nblocks) >> 23);
	count[1] = cpu_to_be32((ctx->nblocks << 9) + (ctx->num << 3));

	fast_sm3_compress(ctx->digest, ctx->block);
	for (i = 0; i < sizeof(ctx->digest) / sizeof(ctx->digest[0]); i++) {
		pdigest[i] = cpu_to_be32(ctx->digest[i]);
	}
}

#define ROTATELEFT(X,n)  (((X)<<(n)) | ((X)>>(32-(n))))

#define P0(x) ((x) ^  ROTATELEFT((x),9)  ^ ROTATELEFT((x),17))
#define P1(x) ((x) ^  ROTATELEFT((x),15) ^ ROTATELEFT((x),23))

#define FF0(x,y,z) ( (x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

#define GG0(x,y,z) ( (x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )


void fast_sm3_compress(uint32_t digest[8], const unsigned char block[64])
{
	int j;
	uint32_t W[68], W1[64];
	const uint32_t* pblock = (const uint32_t*)block;

	uint32_t A = digest[0];
	uint32_t B = digest[1];
	uint32_t C = digest[2];
	uint32_t D = digest[3];
	uint32_t E = digest[4];
	uint32_t F = digest[5];
	uint32_t G = digest[6];
	uint32_t H = digest[7];
	uint32_t SS1, SS2, TT1, TT2, T[64];


	for (j = 0; j < 2; j++) {
		W[0 + j * 8] = cpu_to_be32(pblock[0 + j * 8]);
		W[1 + j * 8] = cpu_to_be32(pblock[1 + j * 8]);
		W[2 + j * 8] = cpu_to_be32(pblock[2 + j * 8]);
		W[3 + j * 8] = cpu_to_be32(pblock[3 + j * 8]);
		W[4 + j * 8] = cpu_to_be32(pblock[4 + j * 8]);
		W[5 + j * 8] = cpu_to_be32(pblock[5 + j * 8]);
		W[6 + j * 8] = cpu_to_be32(pblock[6 + j * 8]);
		W[7 + j * 8] = cpu_to_be32(pblock[7 + j * 8]);
	}
	for (j = 0; j < 6; j++) {
		W[16 + 8 * j] = P1(W[16 + 8 * j - 16] ^ W[16 + 8 * j - 9] ^ ROTATELEFT(W[16 + 8 * j - 3], 15)) ^ ROTATELEFT(W[16 + 8 * j - 13], 7) ^ W[16 + 8 * j - 6];
		W[17 + 8 * j] = P1(W[17 + 8 * j - 16] ^ W[17 + 8 * j - 9] ^ ROTATELEFT(W[17 + 8 * j - 3], 15)) ^ ROTATELEFT(W[17 + 8 * j - 13], 7) ^ W[17 + 8 * j - 6];
		W[18 + 8 * j] = P1(W[18 + 8 * j - 16] ^ W[18 + 8 * j - 9] ^ ROTATELEFT(W[18 + 8 * j - 3], 15)) ^ ROTATELEFT(W[18 + 8 * j - 13], 7) ^ W[18 + 8 * j - 6];
		W[19 + 8 * j] = P1(W[19 + 8 * j - 16] ^ W[19 + 8 * j - 9] ^ ROTATELEFT(W[19 + 8 * j - 3], 15)) ^ ROTATELEFT(W[19 + 8 * j - 13], 7) ^ W[19 + 8 * j - 6];
		W[20 + 8 * j] = P1(W[20 + 8 * j - 16] ^ W[20 + 8 * j - 9] ^ ROTATELEFT(W[20 + 8 * j - 3], 15)) ^ ROTATELEFT(W[20 + 8 * j - 13], 7) ^ W[20 + 8 * j - 6];
		W[21 + 8 * j] = P1(W[21 + 8 * j - 16] ^ W[21 + 8 * j - 9] ^ ROTATELEFT(W[21 + 8 * j - 3], 15)) ^ ROTATELEFT(W[21 + 8 * j - 13], 7) ^ W[21 + 8 * j - 6];
		W[22 + 8 * j] = P1(W[22 + 8 * j - 16] ^ W[22 + 8 * j - 9] ^ ROTATELEFT(W[22 + 8 * j - 3], 15)) ^ ROTATELEFT(W[22 + 8 * j - 13], 7) ^ W[22 + 8 * j - 6];
		W[23 + 8 * j] = P1(W[23 + 8 * j - 16] ^ W[23 + 8 * j - 9] ^ ROTATELEFT(W[23 + 8 * j - 3], 15)) ^ ROTATELEFT(W[23 + 8 * j - 13], 7) ^ W[23 + 8 * j - 6];
	}

	W[64] = P1(W[48] ^ W[55] ^ ROTATELEFT(W[61], 15)) ^ ROTATELEFT(W[51], 7) ^ W[58];
	W[65] = P1(W[49] ^ W[56] ^ ROTATELEFT(W[62], 15)) ^ ROTATELEFT(W[52], 7) ^ W[59];
	W[66] = P1(W[50] ^ W[57] ^ ROTATELEFT(W[63], 15)) ^ ROTATELEFT(W[53], 7) ^ W[60];
	W[67] = P1(W[51] ^ W[58] ^ ROTATELEFT(W[64], 15)) ^ ROTATELEFT(W[54], 7) ^ W[61];

	for (j = 0; j < 2; j++) {
		__m256i a1 = _mm256_loadu_epi32(&W[0 + 32 * j]);
		__m256i a2 = _mm256_loadu_epi32(&W[8 + 32 * j]);
		__m256i a3 = _mm256_loadu_epi32(&W[16 + 32 * j]);
		__m256i a4 = _mm256_loadu_epi32(&W[24 + 32 * j]);

		__m256i b1 = _mm256_loadu_epi32(&W[4 + 32 * j]);
		__m256i b2 = _mm256_loadu_epi32(&W[12 + 32 * j]);
		__m256i b3 = _mm256_loadu_epi32(&W[20 + 32 * j]);
		__m256i b4 = _mm256_loadu_epi32(&W[28 + 32 * j]);

		__m256i c1 = _mm256_xor_si256(a1, b1);
		__m256i c2 = _mm256_xor_si256(a2, b2);
		__m256i c3 = _mm256_xor_si256(a3, b3);
		__m256i c4 = _mm256_xor_si256(a4, b4);

		_mm256_storeu_epi32(&W1[0 + 32 * j], c1);
		_mm256_storeu_epi32(&W1[8 + 32 * j], c2);
		_mm256_storeu_epi32(&W1[16 + 32 * j], c3);
		_mm256_storeu_epi32(&W1[24 + 32 * j], c4);
	}


	for (j = 0; j < 16; j++) { 
		SS1 = ROTATELEFT((ROTATELEFT(A, 12) + E + ROTATELEFT(0x79CC4519, j)), 7);
		SS2 = SS1 ^ ROTATELEFT(A, 12);
		TT1 = FF0(A, B, C) + D + SS2 + W1[j];
		TT2 = GG0(E, F, G) + H + SS1 + W[j];
		D = C;
		C = ROTATELEFT(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = ROTATELEFT(F, 19);
		F = E;
		E = P0(TT2);
	}

	for (j = 16; j < 64; j++) {
		SS1 = ROTATELEFT((ROTATELEFT(A, 12) + E + ROTATELEFT(0x7A879D8A, j)), 7);
		SS2 = SS1 ^ ROTATELEFT(A, 12);
		TT1 = FF1(A, B, C) + D + SS2 + W1[j];
		TT2 = GG1(E, F, G) + H + SS1 + W[j];
		D = C;
		C = ROTATELEFT(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = ROTATELEFT(F, 19);
		F = E;
		E = P0(TT2);
	}

	digest[0] ^= A;
	digest[1] ^= B;
	digest[2] ^= C;
	digest[3] ^= D;
	digest[4] ^= E;
	digest[5] ^= F;
	digest[6] ^= G;
	digest[7] ^= H;
	
}

void fast_sm3(const unsigned char* msg, size_t msglen,
	unsigned char dgst[SM3_DIGEST_LENGTH])
{
	fastsm3_ctx_t ctx;

	fast_sm3_init(&ctx);
	fast_sm3_update(&ctx, msg, msglen);
	fast_sm3_final(&ctx, dgst);

	memset(&ctx, 0, sizeof(fastsm3_ctx_t));
}