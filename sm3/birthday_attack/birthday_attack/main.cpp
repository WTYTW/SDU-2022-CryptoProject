#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/sm3.h>

using namespace std;

const int COLLSION_LEN = 24;    //Åö×²µÄbitÊý
const int COLLSION_BYTES = COLLSION_LEN >> 3;

void SM3_hash(const unsigned char* msg, size_t msglen, unsigned char dgst[SM3_DIGEST_LENGTH]) {
	sm3_ctx_t ctx;

	sm3_init(&ctx);
	sm3_update(&ctx, msg, msglen);
	sm3_final(&ctx, dgst);

	memset(&ctx, 0, sizeof(sm3_ctx_t));
}

void print_dgst(const uint8_t str[SM3_DIGEST_LENGTH]) {
	for (int i = 0; i < SM3_DIGEST_LENGTH; i++) {
		printf("%02x", str[i]);
	}
	printf("\n");
}

int main(int argc, char** argv) {

	unsigned char plain[] = "naive birthday attack for SM3";

	uint8_t dgst[SM3_DIGEST_LENGTH];
	SM3_hash(plain, sizeof(plain), dgst);
	printf("digest of goal: ");
	print_dgst(dgst);

	uint8_t attack_array[32] = { 0 };
	uint8_t col_dgst[SM3_DIGEST_LENGTH];
	uint64_t cnt = 0;

	for (int i = 0;pow(2, COLLSION_LEN/2) ; i++) {
		SM3_hash(attack_array, 32, col_dgst);
		int flag = memcmp(dgst, col_dgst, COLLSION_BYTES);
		if (flag == 0) {
			printf("digest of coll: ");
			print_dgst(col_dgst);
			cout << "attack succeed!" << endl;
			break;
		}
		*((uint64_t*)attack_array) += 1;
	}

	return 0;
}