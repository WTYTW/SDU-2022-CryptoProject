#include <iostream>
#include <stdio.h>
#include <time.h>

#include "sm3.h"
#include "fast_sm3.h"

using namespace std;


int main() {
	unsigned char msg[] = "optimize SM3";
	size_t msg_len = strlen((const char*)msg);
	unsigned char dgst1[SM3_DIGEST_LENGTH];
	unsigned char dgst2[SM3_DIGEST_LENGTH];

	clock_t start, end;
	start = clock();
	for (int i = 0; i < 1000000; i++)
		sm3(msg, msg_len, dgst1);

	end = clock();
	printf("%f\n", double(end - start));

	start = clock();
	for (int i = 0; i < 1000000; i++)
		fast_sm3(msg, msg_len, dgst2);

	end = clock();
	printf("%f\n", double(end - start));

	for (int i = 0; i < SM3_DIGEST_LENGTH; i++)
		printf("%02x", dgst1[i]);
	printf("\n");
	for (int i = 0; i < SM3_DIGEST_LENGTH; i++)
		printf("%02x", dgst2[i]);
}