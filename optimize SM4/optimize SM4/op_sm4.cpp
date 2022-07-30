#include "op_sm4.h"
#include "immintrin.h"

//4字节无符号数组转无符号long型
void four_uCh2uLong(u8* in, u32* out)
{
	int i = 0;
	*out = 0;
	for (i = 0; i < 4; i++)
		*out = ((u32)in[i] << (24 - i * 8)) ^ *out;
}

//无符号long型转4字节无符号数组
void uLong2four_uCh(u32 in, u8* out)
{
	int i = 0;
	//从32位unsigned long的高位开始取
	for (i = 0; i < 4; i++)
		*(out + i) = (u32)(in >> (24 - i * 8));
}

//左移，保留丢弃位放置尾部
u32 move(u32 data, int length)
{
	u32 result = 0;
	result = (data << length) ^ (data >> (32 - length));

	return result;
}

//秘钥处理函数,先使用Sbox进行非线性变化，再将线性变换L置换为L'
u32 func_key(u32 input)
{
	int i = 0;
	u32 ulTmp = 0;
	u8 ucIndexList[4] = { 0 };
	u8 ucSboxValueList[4] = { 0 };
	uLong2four_uCh(input, ucIndexList);

	ucSboxValueList[0] = TBL_SBOX[ucIndexList[0]];
	ucSboxValueList[1] = TBL_SBOX[ucIndexList[1]];
	ucSboxValueList[2] = TBL_SBOX[ucIndexList[2]];
	ucSboxValueList[3] = TBL_SBOX[ucIndexList[3]];
	
	four_uCh2uLong(ucSboxValueList, &ulTmp);
	ulTmp = ulTmp ^ move(ulTmp, 13) ^ move(ulTmp, 23);

	return ulTmp;
}


u32 func_data(u32 input)
{
	int i = 0;
	u32 ulTmp = 0;
	u8 ucIndexList[4] = { 0 };
	u8 ucSboxValueList[4] = { 0 };
	uLong2four_uCh(input, ucIndexList);
	ucSboxValueList[0] = TBL_SBOX[ucIndexList[0]];
	ucSboxValueList[1] = TBL_SBOX[ucIndexList[1]];
	ucSboxValueList[2] = TBL_SBOX[ucIndexList[2]];
	ucSboxValueList[3] = TBL_SBOX[ucIndexList[3]];

	four_uCh2uLong(ucSboxValueList, &ulTmp);
	ulTmp = ulTmp ^ move(ulTmp, 2) ^ move(ulTmp, 10) ^ move(ulTmp, 18) ^ move(ulTmp, 24);

	return ulTmp;
}

//加密函数（可以加密任意长度数据，16字节为一次循环，不足部分补0凑齐16字节的整数倍）
//len:数据长度(任意长度数据) key:密钥（16字节） input:输入的原始数据 output:加密后输出数据
void op_encode(u8 len, u8* key, u8* input, u8* output)
{
	int i = 0, j = 0;
	u8* p = (u8*)malloc(50);      //定义一个50字节缓存区
	u32 ulKeyTmpList[4] = { 0 };   //存储密钥的u32数据
	u32 ulKeyList[36] = { 0 };     //用于密钥扩展算法与系统参数FK运算后的结果存储
	u32 ulDataList[36] = { 0 };    //用于存放加密数据

	/***************************开始生成子秘钥********************************************/
	four_uCh2uLong(key, &(ulKeyTmpList[0]));
	four_uCh2uLong(key + 4, &(ulKeyTmpList[1]));
	four_uCh2uLong(key + 8, &(ulKeyTmpList[2]));
	four_uCh2uLong(key + 12, &(ulKeyTmpList[3]));

	__m128i a = _mm_loadu_epi32(&ulKeyTmpList[0]);
	__m128i b = _mm_loadu_epi32(&TBL_SYS_PARAMS[0]);
	a = _mm_xor_si128(a, b);
	_mm_storeu_epi32(&ulKeyList[0], a);




	for (i = 0; i < 4; i++)             //32次循环迭代运算
	{
		ulKeyList[i * 8 + 4] = ulKeyList[0 + 8 * i] ^ func_key(ulKeyList[1 + 8 * i] ^ ulKeyList[8 * i + 2] ^ ulKeyList[8 * i + 3] ^ TBL_FIX_PARAMS[8 * i + 0]);
		ulKeyList[i * 8 + 5] = ulKeyList[1 + 8 * i] ^ func_key(ulKeyList[2 + 8 * i] ^ ulKeyList[8 * i + 3] ^ ulKeyList[8 * i + 4] ^ TBL_FIX_PARAMS[8 * i + 1]);
		ulKeyList[i * 8 + 6] = ulKeyList[2 + 8 * i] ^ func_key(ulKeyList[3 + 8 * i] ^ ulKeyList[8 * i + 4] ^ ulKeyList[8 * i + 5] ^ TBL_FIX_PARAMS[8 * i + 2]);
		ulKeyList[i * 8 + 7] = ulKeyList[3 + 8 * i] ^ func_key(ulKeyList[4 + 8 * i] ^ ulKeyList[8 * i + 5] ^ ulKeyList[8 * i + 6] ^ TBL_FIX_PARAMS[8 * i + 3]);
		ulKeyList[i * 8 + 8] = ulKeyList[4 + 8 * i] ^ func_key(ulKeyList[5 + 8 * i] ^ ulKeyList[8 * i + 6] ^ ulKeyList[8 * i + 7] ^ TBL_FIX_PARAMS[8 * i + 4]);
		ulKeyList[i * 8 + 9] = ulKeyList[5 + 8 * i] ^ func_key(ulKeyList[6 + 8 * i] ^ ulKeyList[8 * i + 7] ^ ulKeyList[8 * i + 8] ^ TBL_FIX_PARAMS[8 * i + 5]);
		ulKeyList[i * 8 + 10] = ulKeyList[6 + 8 * i] ^ func_key(ulKeyList[7 + 8 * i] ^ ulKeyList[8 * i + 8] ^ ulKeyList[8 * i + 9] ^ TBL_FIX_PARAMS[8 * i + 6]);
		ulKeyList[i * 8 + 11] = ulKeyList[7 + 8 * i] ^ func_key(ulKeyList[8 + 8 * i] ^ ulKeyList[8 * i + 9] ^ ulKeyList[8 * i + 10] ^ TBL_FIX_PARAMS[8 * i + 7]);
	}
	/***********************************生成32轮32位长子秘钥结束**********************************/

	memcpy(p, input, len);
	memset(p + len, 0, 16 - len % 16);

	for (j = 0; j < len / 16 + ((len % 16) ? 1 : 0); j++)  //进行循环加密,并将加密后数据保存（可以看出此处是以16字节为一次加密，进行循环，即若16字节则进行一次，17字节补0至32字节后进行加密两次，以此类推）
	{
		/*开始处理加密数据*/
		four_uCh2uLong(p + 16 * j, &(ulDataList[0]));
		four_uCh2uLong(p + 16 * j + 4, &(ulDataList[1]));
		four_uCh2uLong(p + 16 * j + 8, &(ulDataList[2]));
		four_uCh2uLong(p + 16 * j + 12, &(ulDataList[3]));
		//加密
		for (i = 0; i < 4; i++)
		{
			ulDataList[i * 8 + 4] = ulDataList[i * 8] ^ func_data(ulDataList[i * 8 + 1] ^ ulDataList[i * 8 + 2] ^ ulDataList[i * 8 + 3] ^ ulKeyList[i * 8 + 4]);
			ulDataList[i * 8 + 5] = ulDataList[i * 8+1] ^ func_data(ulDataList[i * 8 + 2] ^ ulDataList[i * 8 + 3] ^ ulDataList[i * 8 + 4] ^ ulKeyList[i * 8 + 5]);
			ulDataList[i * 8 + 6] = ulDataList[i * 8+2] ^ func_data(ulDataList[i * 8 + 3] ^ ulDataList[i * 8 + 4] ^ ulDataList[i * 8 + 5] ^ ulKeyList[i * 8 + 6]);
			ulDataList[i * 8 + 7] = ulDataList[i * 8+3] ^ func_data(ulDataList[i * 8 + 4] ^ ulDataList[i * 8 + 5] ^ ulDataList[i * 8 + 6] ^ ulKeyList[i * 8 + 7]);
			ulDataList[i * 8 + 8] = ulDataList[i * 8+4] ^ func_data(ulDataList[i * 8 + 5] ^ ulDataList[i * 8 + 6] ^ ulDataList[i * 8 + 7] ^ ulKeyList[i * 8 + 8]);
			ulDataList[i * 8 + 9] = ulDataList[i * 8+5] ^ func_data(ulDataList[i * 8 + 6] ^ ulDataList[i * 8 + 7] ^ ulDataList[i * 8 + 8] ^ ulKeyList[i * 8 + 9]);
			ulDataList[i * 8 + 10] = ulDataList[i * 8+6] ^ func_data(ulDataList[i * 8 + 7] ^ ulDataList[i * 8 + 8] ^ ulDataList[i * 8 + 9] ^ ulKeyList[i * 8 + 10]);
			ulDataList[i * 8 + 11] = ulDataList[i * 8+7] ^ func_data(ulDataList[i * 8 + 8] ^ ulDataList[i * 8 + 9] ^ ulDataList[i * 8 + 10] ^ ulKeyList[i * 8 + 11]);
		}
		/*将加密后数据输出*/
		uLong2four_uCh(ulDataList[35], output + 16 * j);
		uLong2four_uCh(ulDataList[34], output + 16 * j + 4);
		uLong2four_uCh(ulDataList[33], output + 16 * j + 8);
		uLong2four_uCh(ulDataList[32], output + 16 * j + 12);
	}
	free(p);
}

//解密函数（与加密函数基本一致，只是秘钥使用的顺序不同，即把钥匙反着用就是解密）
//len:数据长度 key:密钥 input:输入的加密后数据 output:输出的解密后数据
void op_decode(u8 len, u8* key, u8* input, u8* output)
{
	int i = 0, j = 0;
	u32 ulKeyTmpList[4] = { 0 };//存储密钥的u32数据
	u32 ulKeyList[36] = { 0 };  //用于密钥扩展算法与系统参数FK运算后的结果存储
	u32 ulDataList[36] = { 0 }; //用于存放加密数据

	/*开始生成子秘钥*/
	four_uCh2uLong(key, &(ulKeyTmpList[0]));
	four_uCh2uLong(key + 4, &(ulKeyTmpList[1]));
	four_uCh2uLong(key + 8, &(ulKeyTmpList[2]));
	four_uCh2uLong(key + 12, &(ulKeyTmpList[3]));

	__m128i a = _mm_loadu_epi32(&ulKeyTmpList[0]);
	__m128i b = _mm_loadu_epi32(&TBL_SYS_PARAMS[0]);
	a = _mm_xor_si128(a, b);
	_mm_storeu_epi32(&ulKeyList[0], a);



	for (i = 0; i < 4; i++)             
	{
		ulKeyList[i*8 + 4] = ulKeyList[i*8] ^ func_key(ulKeyList[i*8 + 1] ^ ulKeyList[i*8 + 2] ^ ulKeyList[i*8 + 3] ^ TBL_FIX_PARAMS[i*8]);
		ulKeyList[i*8 + 5] = ulKeyList[i*8+1] ^ func_key(ulKeyList[i*8 + 2] ^ ulKeyList[i*8 + 3] ^ ulKeyList[i*8 + 4] ^ TBL_FIX_PARAMS[i*8+1]);
		ulKeyList[i*8 + 6] = ulKeyList[i*8+2] ^ func_key(ulKeyList[i*8 + 3] ^ ulKeyList[i*8 + 4] ^ ulKeyList[i*8 + 5] ^ TBL_FIX_PARAMS[i*8+2]);
		ulKeyList[i*8 + 7] = ulKeyList[i*8+3] ^ func_key(ulKeyList[i*8 + 4] ^ ulKeyList[i*8 + 5] ^ ulKeyList[i*8 + 6] ^ TBL_FIX_PARAMS[i*8+3]);
		ulKeyList[i*8 + 8] = ulKeyList[i*8+4] ^ func_key(ulKeyList[i*8 + 5] ^ ulKeyList[i*8 + 6] ^ ulKeyList[i*8 + 7] ^ TBL_FIX_PARAMS[i*8+4]);
		ulKeyList[i*8 + 9] = ulKeyList[i*8+5] ^ func_key(ulKeyList[i*8 + 6] ^ ulKeyList[i*8 + 7] ^ ulKeyList[i*8 + 8] ^ TBL_FIX_PARAMS[i*8+5]);
		ulKeyList[i*8 + 10] = ulKeyList[i*8+6] ^ func_key(ulKeyList[i*8 + 7] ^ ulKeyList[i*8 + 8] ^ ulKeyList[i*8 + 9] ^ TBL_FIX_PARAMS[i*8+6]);
		ulKeyList[i*8 + 11] = ulKeyList[i*8+7] ^ func_key(ulKeyList[i*8 + 8] ^ ulKeyList[i*8 + 9] ^ ulKeyList[i*8 + 10] ^ TBL_FIX_PARAMS[i*8+7]);
		
	}
	/*生成32轮32位长子秘钥结束*/

	for (j = 0; j < len / 16; j++)  //进行循环加密,并将加密后数据保存
	{
		/*开始处理解密数据*/
		four_uCh2uLong(input + 16 * j, &(ulDataList[0]));
		four_uCh2uLong(input + 16 * j + 4, &(ulDataList[1]));
		four_uCh2uLong(input + 16 * j + 8, &(ulDataList[2]));
		four_uCh2uLong(input + 16 * j + 12, &(ulDataList[3]));

		//解密
		for (i = 0; i < 4; i++)
		{
			ulDataList[i * 8 + 4] = ulDataList[i * 8] ^ func_data(ulDataList[i * 8 + 1] ^ ulDataList[i * 8 + 2] ^ ulDataList[i * 8 + 3] ^ ulKeyList[35-i * 8 ]);
			ulDataList[i * 8 + 5] = ulDataList[i * 8 + 1] ^ func_data(ulDataList[i * 8 + 2] ^ ulDataList[i * 8 + 3] ^ ulDataList[i * 8 + 4] ^ ulKeyList[35 - i * 8 ]);
			ulDataList[i * 8 + 6] = ulDataList[i * 8 + 2] ^ func_data(ulDataList[i * 8 + 3] ^ ulDataList[i * 8 + 4] ^ ulDataList[i * 8 + 5] ^ ulKeyList[35 - i * 8 ]);
			ulDataList[i * 8 + 7] = ulDataList[i * 8 + 3] ^ func_data(ulDataList[i * 8 + 4] ^ ulDataList[i * 8 + 5] ^ ulDataList[i * 8 + 6] ^ ulKeyList[35 - i * 8 ]);
			ulDataList[i * 8 + 8] = ulDataList[i * 8 + 4] ^ func_data(ulDataList[i * 8 + 5] ^ ulDataList[i * 8 + 6] ^ ulDataList[i * 8 + 7] ^ ulKeyList[35 - i * 8 ]);
			ulDataList[i * 8 + 9] = ulDataList[i * 8 + 5] ^ func_data(ulDataList[i * 8 + 6] ^ ulDataList[i * 8 + 7] ^ ulDataList[i * 8 + 8] ^ ulKeyList[35 - i * 8 ]);
			ulDataList[i * 8 + 10] = ulDataList[i * 8 + 6] ^ func_data(ulDataList[i * 8 + 7] ^ ulDataList[i * 8 + 8] ^ ulDataList[i * 8 + 9] ^ ulKeyList[35 - i * 8 ]);
			ulDataList[i * 8 + 11] = ulDataList[i * 8 + 7] ^ func_data(ulDataList[i * 8 + 8] ^ ulDataList[i * 8 + 9] ^ ulDataList[i * 8 + 10] ^ ulKeyList[35 - i * 8 ]);
		}
		/*将解密后数据输出*/
		uLong2four_uCh(ulDataList[35], output + 16 * j);
		uLong2four_uCh(ulDataList[34], output + 16 * j + 4);
		uLong2four_uCh(ulDataList[33], output + 16 * j + 8);
		uLong2four_uCh(ulDataList[32], output + 16 * j + 12);
	}
}

//无符号字符数组转16进制打印
void print_hex(u8* data, int len)
{
	int i = 0;
	char alTmp[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	for (i = 0; i < len; i++)
	{
		printf("%c", alTmp[data[i] / 16]);
		printf("%c", alTmp[data[i] % 16]);
		putchar(' ');
	}
	putchar('\n');
}
