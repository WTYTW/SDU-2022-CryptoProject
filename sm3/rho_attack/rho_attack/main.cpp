#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <openssl/sm3.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

const int COLLSION_LEN = 24;    //Åö×²bitÊý
const int COLLSION_BYTES = COLLSION_LEN >> 3;
const size_t Rho_length = 5;

void sm3(const unsigned char* msg, size_t msglen, unsigned char dgst[SM3_DIGEST_LENGTH]) {
	sm3_ctx_t ctx;

	sm3_init(&ctx);
	sm3_update(&ctx, msg, msglen);
	sm3_final(&ctx, dgst);

	memset(&ctx, 0, sizeof(sm3_ctx_t));
}


int main(int argc, char** argv)
{
	unsigned char msg[] = "Rho attack for SM3";

	unsigned char** dgst = new unsigned char*[Rho_length+1];
	for (int i = 0; i < Rho_length+1; i++)
		dgst[i] = new unsigned char[SM3_DIGEST_LENGTH];

	sm3(msg, strlen((const char*)msg), dgst[0]);
	size_t i = 0;
	while (memcmp(dgst[i], dgst[(i + 1) % (Rho_length+1)], COLLSION_BYTES) != 0) {
		sm3(dgst[i], SM3_DIGEST_LENGTH, dgst[(i + 1) % (Rho_length+1)]);
		i = (i + 1) % (Rho_length+1);
	}
	i = (i + 1) % (Rho_length+1);
	for (int j = 0; j < (Rho_length+1); j++) {
		for (int k = 0; k < SM3_DIGEST_LENGTH; k++)
			printf("%02x", dgst[i][k]);
		printf("\n");
		i = (i + 1) % (Rho_length+1);
	}
	return 0;
}