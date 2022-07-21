#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/sm3.h>

void SM3_Signature(const unsigned char* key, const unsigned char* msg, unsigned char* sig) {

    size_t num = strlen((const char*)key) + strlen((const char*)msg);
    unsigned char* buf = new unsigned char[num + 1];
    strcpy_s((char*)buf, strlen((const char*)key) + 1, (const char*)key);
    strcat_s((char*)buf, num + 1, (const char*)msg);

    sm3_ctx_t ctx;

    sm3_init(&ctx);
    sm3_update(&ctx, buf, num);
    sm3_final(&ctx,sig);
}

bool SM3_Verify(const unsigned char* key, const unsigned char* msg, unsigned char* sig) {
    unsigned char test_sig[SM3_DIGEST_LENGTH];
    SM3_Signature(key, msg, test_sig);
    if (!memcmp(test_sig, sig, SM3_DIGEST_LENGTH))
        return true;
    return false;
}

void SM3_LEA(size_t key_len, unsigned char* msg, unsigned char* add, unsigned char* sig, unsigned char* new_sig) {

    size_t datalen = key_len + strlen((const char*)msg);
    size_t blocks = datalen / SM3_BLOCK_SIZE;
    datalen -= blocks * SM3_BLOCK_SIZE;

    sm3_ctx_t new_sig_ctx;
    sm3_init(&new_sig_ctx);
    memcpy(&(new_sig_ctx.digest), sig, SM3_DIGEST_LENGTH);
    new_sig_ctx.nblocks = (key_len + strlen((const char*)msg)) / SM3_BLOCK_SIZE;

    unsigned char* new_msg = new unsigned char[strlen((const char*)msg) + strlen((const char*)add) + SM3_BLOCK_SIZE + 9];
    unsigned char* p = new_msg;
    memcpy(p, msg, strlen((const char*)msg));
    p = p + strlen((const char*)msg);
    memset(p, 0x80, 1);
    p = p + 1;
    if (datalen <= SM3_BLOCK_SIZE - 9) {
        memset(p, 0x00, SM3_BLOCK_SIZE - datalen - 9);
        p = p + SM3_BLOCK_SIZE - datalen - 9;
    }
    else {
        memset(p, 0x00, SM3_BLOCK_SIZE - datalen - 1);
        p = p + SM3_BLOCK_SIZE - datalen - 1;
        memset(p, 0x00, SM3_BLOCK_SIZE - 8);
        p = p + SM3_BLOCK_SIZE - 8;
        new_sig_ctx.nblocks++;
    }
    int org_len = (key_len + strlen((const char*)msg)) << 3;
    for (int i = 3; i >= 0; i--)
        memset(p++, (org_len >> (i << 3)) & 0xff, 1);
    
    sm3_update(&new_sig_ctx, add, strlen((const char*)add));
    sm3_final(&new_sig_ctx, new_sig);

}