### Implement SM2 with RFC6979

GmSSL中的SM2中K原本为随机生成，存在安全问题。因此依照RFC6979规定的流程替换随机生成部分。

函数BN_rand即为K生成函数

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_09-49-33.png" alt="Snipaste_2022-07-30_09-49-33" style="zoom:80%;" />

替换到图中调用位置

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_09-51-26.png)

SM2功能验证如下：

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_09-53-29.png" alt="Snipaste_2022-07-30_09-53-29" style="zoom:80%;" />

