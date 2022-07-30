#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "SHA256.h"
#include "SM3.h"

#define DIGEST_LENGTH 32

void print_dgst(const uint8_t str[DIGEST_LENGTH]) {
    for (int i = 0; i < DIGEST_LENGTH; i++)
        printf("%x", str[i]);
    printf("\n");
}

int main(int argc, char** argv)
{
    unsigned char key[] = "password";
    unsigned char msg[] = "length extension attack";
    unsigned char extension[] = "sha256&sm3";

    unsigned char sig[DIGEST_LENGTH];


    printf("-----------SHA256 length extension attack--------------\n");
    SHA256_Signature(key, msg, sig);
    if (!SHA256_Verify(key, msg, sig))
        goto err;

    unsigned char new_sig[DIGEST_LENGTH];

    SHA256_LEA(strlen((const char*)key), msg, extension, sig, new_sig);
    printf("new_sig:");
    print_dgst(new_sig);

    printf("\n");
    printf("-----------SM3 length extension attack--------------\n");
    SM3_Signature(key, msg, sig);
    if (!SM3_Verify(key, msg, sig))
        goto err;

    SM3_LEA(strlen((const char*)key), msg, extension, sig, new_sig);
    printf("new_sig:");
    print_dgst(new_sig);

    return 0;

err:
    fprintf(stderr, "Fatal EVP error!\n");
    return 1;
}