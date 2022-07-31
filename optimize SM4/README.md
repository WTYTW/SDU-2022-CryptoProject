### optimize SM4

在Gmssl SM4源码的基础上进行加速，采用循环展开以及SIMD进行加速，部分代码展示如下：

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-27-36.png" alt="Snipaste_2022-07-30_00-27-36" style="zoom:80%;" />
使用原代码以及加速后代码进行加解密十万次后的时间对比如下：

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-27-57.png)

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-25-44.png" alt="Snipaste_2022-07-30_00-25-44" style="zoom:80%;" />

- 原    时    间：4703ms
- 加速后时间：3277ms
- 加解密结果相同

