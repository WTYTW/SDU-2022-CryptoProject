#ifndef _SM4_H_
#define _SM4_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

void encode_fun(u8 len, u8* key, u8* input, u8* output);   //���ܺ���

void decode_fun(u8 len, u8* key, u8* input, u8* output);   //���ܺ���

#ifdef __cplusplus
}
#endif

#endif
