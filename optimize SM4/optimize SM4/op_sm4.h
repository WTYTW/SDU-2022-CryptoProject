#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

void op_encode(u8 len, u8* key, u8* input, u8* output);  

void op_decode(u8 len, u8* key, u8* input, u8* output);  

#ifdef __cplusplus
}
#endif