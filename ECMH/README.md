### Implement ECMH scheme

把集合中的元素哈希映射成椭圆曲线上的点，再利用ECC的加法将元素求和输出，使得

$Hash(a,b)=Hash(b,a)、Hash(Hash(a,b,c))=Hash(a,Hash(b,c))$

项目实现基于GmSSL中SHA256、EC等源码，将实现功能封装与ecmh类中

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_10-56-35.png" alt="Snipaste_2022-07-30_10-56-35" style="zoom:67%;" />

测试运行结果如下：

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_10-58-43.png)
