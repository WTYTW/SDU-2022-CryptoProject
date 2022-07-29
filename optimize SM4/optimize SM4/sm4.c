#include "sm4.h"

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
	for (i = 0; i < 4; i++)
	{
		ucSboxValueList[i] = TBL_SBOX[ucIndexList[i]];
	}
	four_uCh2uLong(ucSboxValueList, &ulTmp);
	ulTmp = ulTmp ^ move(ulTmp, 13) ^ move(ulTmp, 23);

	return ulTmp;
}

//加解密数据处理函数,先使用Sbox进行非线性变化，再进行线性变换L
u32 func_data(u32 input)
{
	int i = 0;
	u32 ulTmp = 0;
	u8 ucIndexList[4] = { 0 };
	u8 ucSboxValueList[4] = { 0 };
	uLong2four_uCh(input, ucIndexList);
	for (i = 0; i < 4; i++)
	{
		ucSboxValueList[i] = TBL_SBOX[ucIndexList[i]];
	}
	four_uCh2uLong(ucSboxValueList, &ulTmp);
	ulTmp = ulTmp ^ move(ulTmp, 2) ^ move(ulTmp, 10) ^ move(ulTmp, 18) ^ move(ulTmp, 24);

	return ulTmp;
}

//加密函数（可以加密任意长度数据，16字节为一次循环，不足部分补0凑齐16字节的整数倍）
//len:数据长度(任意长度数据) key:密钥（16字节） input:输入的原始数据 output:加密后输出数据
void encode_fun(u8 len, u8* key, u8* input, u8* output)
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

	ulKeyList[0] = ulKeyTmpList[0] ^ TBL_SYS_PARAMS[0];
	ulKeyList[1] = ulKeyTmpList[1] ^ TBL_SYS_PARAMS[1];
	ulKeyList[2] = ulKeyTmpList[2] ^ TBL_SYS_PARAMS[2];
	ulKeyList[3] = ulKeyTmpList[3] ^ TBL_SYS_PARAMS[3];

	for (i = 0; i < 32; i++)             //32次循环迭代运算
	{
		//5-36为32个子秘钥
		ulKeyList[i + 4] = ulKeyList[i] ^ func_key(ulKeyList[i + 1] ^ ulKeyList[i + 2] ^ ulKeyList[i + 3] ^ TBL_FIX_PARAMS[i]);
	}
	/***********************************生成32轮32位长子秘钥结束**********************************/

	for (i = 0; i < len; i++)        //将输入数据存放在p缓存区
		*(p + i) = *(input + i);
	for (i = 0; i < 16 - len % 16; i++)//将不足16位补0凑齐16的整数倍
		*(p + len + i) = 0;

	for (j = 0; j < len / 16 + ((len % 16) ? 1 : 0); j++)  //进行循环加密,并将加密后数据保存（可以看出此处是以16字节为一次加密，进行循环，即若16字节则进行一次，17字节补0至32字节后进行加密两次，以此类推）
	{
		/*开始处理加密数据*/
		four_uCh2uLong(p + 16 * j, &(ulDataList[0]));
		four_uCh2uLong(p + 16 * j + 4, &(ulDataList[1]));
		four_uCh2uLong(p + 16 * j + 8, &(ulDataList[2]));
		four_uCh2uLong(p + 16 * j + 12, &(ulDataList[3]));
		//加密
		for (i = 0; i < 32; i++)
		{
			ulDataList[i + 4] = ulDataList[i] ^ func_data(ulDataList[i + 1] ^ ulDataList[i + 2] ^ ulDataList[i + 3] ^ ulKeyList[i + 4]);
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
void decode_fun(u8 len, u8* key, u8* input, u8* output)
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

	ulKeyList[0] = ulKeyTmpList[0] ^ TBL_SYS_PARAMS[0];
	ulKeyList[1] = ulKeyTmpList[1] ^ TBL_SYS_PARAMS[1];
	ulKeyList[2] = ulKeyTmpList[2] ^ TBL_SYS_PARAMS[2];
	ulKeyList[3] = ulKeyTmpList[3] ^ TBL_SYS_PARAMS[3];

	for (i = 0; i < 32; i++)             //32次循环迭代运算
	{
		//5-36为32个子秘钥
		ulKeyList[i + 4] = ulKeyList[i] ^ func_key(ulKeyList[i + 1] ^ ulKeyList[i + 2] ^ ulKeyList[i + 3] ^ TBL_FIX_PARAMS[i]);
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
		for (i = 0; i < 32; i++)
		{
			ulDataList[i + 4] = ulDataList[i] ^ func_data(ulDataList[i + 1] ^ ulDataList[i + 2] ^ ulDataList[i + 3] ^ ulKeyList[35 - i]);//与加密唯一不同的就是轮密钥的使用顺序
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



