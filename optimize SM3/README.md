### optimize SM3

在Gmssl SM3源码的基础上进行加速，采用循环展开以及SIMD进行加速，部分代码展示如下：

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-08-48.png" alt="Snipaste_2022-07-30_00-08-48" style="zoom:67%;" />

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-09-04.png" alt="Snipaste_2022-07-30_00-09-04" style="zoom:67%;" />

使用原代码以及加速后代码运算一百万次后的时间对比如下：

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-12-32.png)

- 原    时    间：3431ms
- 加速后时间：1878ms
- 哈希值相同
