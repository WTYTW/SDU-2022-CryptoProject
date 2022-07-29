#include <iostream>
#include <stdio.h>
#include <time.h>

#include "sm4.h"
#include "op_sm4.h"


int main(void)
{
	u8 encode_Result1[50] = { 0 };    
	u8 decode_Result1[50] = { 0 };    
	u8 encode_Result2[50] = { 0 };
	u8 decode_Result2[50] = { 0 };

	u8 key[16] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10 };       //����16�ֽڵ���Կ
	u8 Data_plain[18] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,0x01,0x23 };//����18�ֽڵ�ԭʼ�������ݣ������ã�
	
	u8 len = 16 * (sizeof(Data_plain) / 16) + 16 * ((sizeof(Data_plain) % 16) ? 1 : 0);//�õ��������ֽ��������ܺ������õ���

	clock_t start, end;
	start = clock();
	for (int i = 0; i < 100000; i++) {
		encode_fun(sizeof(Data_plain), key, Data_plain, encode_Result1);     
		decode_fun(len, key, encode_Result1, decode_Result1);
	}
	printf("���ܺ������ǣ�\n");
	for (int i = 0; i < len; i++)
		printf("%02x", *(encode_Result1 + i));
	printf("\n");
	printf("���ܺ������ǣ�\n");
	for (int i = 0; i < len; i++)
		printf("%02x", *(decode_Result1 + i));
	printf("\n");
	end = clock();
	printf("%f\n\n", double(end - start));

	start = clock();
	for (int i = 0; i < 100000; i++) {
		op_encode(sizeof(Data_plain), key, Data_plain, encode_Result2);
		op_decode(len, key, encode_Result1, decode_Result2);
	}

	printf("���ܺ������ǣ�\n");
	for (int i = 0; i < len; i++)
		printf("%02x", *(encode_Result2 + i));
	printf("\n");
	printf("���ܺ������ǣ�\n");
	for (int i = 0; i < len; i++)
		printf("%02x", *(decode_Result2 + i));
	printf("\n");
	end = clock();
	printf("%f\n", double(end - start));

	return 0;
}