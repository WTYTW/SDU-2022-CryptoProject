#pragma once
void SM3_Signature(const unsigned char* key, const unsigned char* msg, unsigned char* sig);
bool SM3_Verify(const unsigned char* key, const unsigned char* msg, unsigned char* sig);
void SM3_LEA(size_t key_len, unsigned char* msg, unsigned char* add, unsigned char* sig, unsigned char* new_sig);