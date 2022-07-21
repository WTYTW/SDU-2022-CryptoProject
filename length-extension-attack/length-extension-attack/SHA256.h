#pragma once


void SHA256_Signature(const unsigned char* key, const unsigned char* msg, unsigned char* sig);
bool SHA256_Verify(const unsigned char* key, const unsigned char* msg, unsigned char* sig);
void SHA256_LEA(size_t key_len, unsigned char* msg, unsigned char* add, unsigned char* sig, unsigned char* new_sig);