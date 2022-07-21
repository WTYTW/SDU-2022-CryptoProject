#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>



void SHA256_Signature(const unsigned char* key, const unsigned char* msg, unsigned char* sig) {

    size_t num = strlen((const char*)key) + strlen((const char*)msg);
    unsigned char* buf = new unsigned char[num + 1];
    strcpy_s((char*)buf, strlen((const char*)key) + 1, (const char*)key);
    strcat_s((char*)buf, num + 1, (const char*)msg);

    SHA256_CTX ctx;

    SHA256_Init(&ctx);
    SHA256_Update(&ctx, buf, num);
    SHA256_Final(sig, &ctx);
}

bool SHA256_Verify(const unsigned char* key, const unsigned char* msg, unsigned char* sig) {
    unsigned char test_sig[SHA256_DIGEST_LENGTH];
    SHA256_Signature(key, msg, test_sig);
    if (!memcmp(test_sig, sig, SHA256_DIGEST_LENGTH))
        return true;
    return false;
}

void SHA256_LEA(size_t key_len, unsigned char* msg, unsigned char* add, unsigned char* sig, unsigned char* new_sig) {

    size_t datalen = key_len + strlen((const char*)msg);
    size_t blocks = datalen / SHA256_CBLOCK;
    datalen -= blocks * SHA256_CBLOCK;

    unsigned char* new_msg = new unsigned char[strlen((const char*)msg) + strlen((const char*)add) + SHA256_CBLOCK + 9];
    unsigned char* p = new_msg;
    memcpy(p, msg, strlen((const char*)msg));
    p = p + strlen((const char*)msg);
    memset(p, 0x80, 1);
    p = p + 1;
    if (datalen <= SHA256_CBLOCK - 9) {
        memset(p, 0x00, SHA256_CBLOCK - datalen - 9);
        p = p + SHA256_CBLOCK - datalen - 9;
    }
    else {
        memset(p, 0x00, SHA256_CBLOCK - datalen - 1);
        p = p + SHA256_CBLOCK - datalen - 1;
        memset(p, 0x00, SHA256_CBLOCK - 8);
        p = p + SHA256_CBLOCK - 8;
    }
    int org_len = (key_len + strlen((const char*)msg)) << 3;
    for (int i = 3; i >= 0; i--)
        memset(p++, (org_len >> (i << 3)) & 0xff, 1);

    SHA256_CTX new_sig_ctx;
    SHA256_Init(&new_sig_ctx);
    new_sig_ctx.Nl = (key_len + strlen((const char*)msg)) << 3;
    for (int i = 0; i < 8; i++) {
        new_sig_ctx.h[i] = 0;
        for (int j = 0; j < 4; j++) {
            new_sig_ctx.h[i] |= (sig[(i << 2) + j] << ((3 - j) << 3));
        }
    }
    SHA256_Update(&new_sig_ctx, add, strlen((const char*)add));
    SHA256_Final(new_sig, &new_sig_ctx);

}