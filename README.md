# SDU-2022-CryptoProject
创新创业实践项目

## 小组成员

| 姓名   | 班级          | 学号         | git账户名                         |
| ------ | ------------- | ------------ | --------------------------------- |
| 王泰宇 | 2019级网安1班 | 201900460008 | [WTYTW](https://github.com/WTYTW) |



## 项目简介

课程共包含19个项目，完成11个项目。

涉及编程语言包含：C、C++、Python



## 运行指导

- 对于c、c++项目

  编译器使用Visual Studio 2019，通过sln文件导入整个项目，部分密码算法基于开源代码库[GmSSL](https://github.com/guanzhi/GmSSL)，环境配置依照[方法](https://blog.csdn.net/vincy_yeha/article/details/120572903)

  安装完成后，vs项目配置如下：

  <img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_22-34-54.png" alt="Snipaste_2022-07-30_22-34-54" style="zoom:80%;" />

  <img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_22-35-24.png" alt="Snipaste_2022-07-30_22-35-24" style="zoom:80%;" />

- 对于python项目

  pip install gmssl安装包后，可直接运行



## 项目列表

### SM3

- [x] Implement the naive birthday attack of reduced `SM3`
- [x] Implement the Rho method of reduced `SM3`
- [x] Implement length extension attack for `SM3`, `SHA256`, etc.
- [x] Do your best to optimize `SM3` implementation (software)
- [x] Implement Merkle Tree following [RFC6962](https://www.rfc-editor.org/info/rfc6962)

### SM4

- [x] Do your best to optimize `SM4` implementation (software)

### SM2

- [ ] Report on the application of this deduce technique in Ethereum with `ECDSA`
- [x] Implement `SM2` with [RFC6979](https://www.rfc-editor.org/info/rfc6979)
- [x] Verify the some pitfalls with proof-of-concept code
- [x] Implement the above `ECMH` scheme
- [x] Implement a `PGP` scheme with `SM2`

- [x] Implement `SM2` 2P sign with real network communication
- [ ] Implement `SM2` 2P decrypt with real network communication
- [ ] PoC impl of the scheme, or do implement analysis by Google

### Bitcoin-public

- [ ] Send a tx on Bitcoin testnet,and parse the tx data down to every bit,better write script yourself
- [ ] Forge a signature to pretend that you are Satoshi

### Eth-public

- [ ] Research report on MPT

### Real world Cryptanalyses

- [ ] Find a key with hash value "sdu_cst_20220610" under a message composed of your name followed by your student ID.
- [ ] Find a 64-byte message under some k fulfilling that their hash value is symmetrical

## 各项目说明

### Birthday Attack

基于GmSSL-sm3，进行生日攻击找到固定比特数的碰撞

- 设置碰撞的比特数
![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/image-20220729222354390.png)


- 对字符串“naive birthday attack for SM3”的哈希值寻找碰撞

  >碰撞24比特：
  >
  >![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-29_23-26-23.png)
  >
  >碰撞32比特：
  >
  >![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-29_23-24-45.png)

### Rho method

基于GmSSL-sm3，通过Rho方法找到固定比特数的碰撞

- 设置碰撞比特数及Rho大小

  ![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-29_23-49-58.png)

- 对字符串“Rho attack for SM3”的哈希值寻找碰撞

  >碰撞24比特，Rho大小为5
  >
  >![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-01-31.png)

### length extension attack

基于GmSSL sm3和sha256，设置key=“password”，message="length extension attack"

可求得哈希值：

$h_1=sm3(key||message)$、$h_2=sha256(key||message)$

对其进行长度扩展攻击，添加消息extension=“sha256&sm3”

参照sm3和sha256的compress函数在key||message后添加特定比特得到M，在M后级联extension得到新消息M'，从而可在key未知的前提下构造新哈希值

$h_1'=sm3(M||extension)$、$h_2'=sha256(M||extension)$可通过验证

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_15-06-55.png)

### Merkle Tree

基于GmSSL SHA256依照RFC6979实现Merkle Hash Tree

定义节点结构体：

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_13-30-25.png" alt="Snipaste_2022-07-30_13-30-25" style="zoom:67%;" />

实现功能包括：

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_13-50-35.png)

- 树的构建

- 新节点的添加

- 存在性证明（存在则输出其到root沿途hash值，不存在则输出“Not found”）

  运行结果如下：

  ![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_13-47-48.png)

### optimize SM3

在Gmssl SM3源码的基础上进行加速，采用循环展开以及SIMD进行加速，部分代码展示如下：

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-08-48.png" alt="Snipaste_2022-07-30_00-08-48" style="zoom:67%;" />

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-09-04.png" alt="Snipaste_2022-07-30_00-09-04" style="zoom:67%;" />

使用原代码以及加速后代码运算一百万次后的时间对比如下：

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-12-32.png)

- 原    时    间：3431ms
- 加速后时间：1878ms
- 哈希值相同

### optimize SM4

在Gmssl SM4源码的基础上进行加速，采用循环展开以及SIMD进行加速，部分代码展示如下：

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-27-36.png" alt="Snipaste_2022-07-30_00-27-36" style="zoom:80%;" />
使用原代码以及加速后代码进行加解密十万次后的时间对比如下：

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-27-57.png)

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_00-25-44.png" alt="Snipaste_2022-07-30_00-25-44" style="zoom:80%;" />

- 原    时    间：4703ms
- 加速后时间：3277ms
- 加解密结果相同

### Implement SM2 with RFC6979

GmSSL中的SM2中K原本为随机生成，存在安全问题。因此依照RFC6979规定的流程替换随机生成部分。

函数BN_rand即为K生成函数

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_09-49-33.png" alt="Snipaste_2022-07-30_09-49-33" style="zoom:80%;" />

替换到图中调用位置

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_09-51-26.png)

SM2功能验证如下：

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_09-53-29.png" alt="Snipaste_2022-07-30_09-53-29" style="zoom:80%;" />



### Verify Some Pitfalls

对于ECDSA，验证一些陷门如下：

- 泄露k会导致泄露d

  已知$$s=k^{-1}(e+dr) mod\ n$$其中s、e、r皆已知在k泄露的情况下可求出私钥d

  ![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_10-30-24.png)

  

- 重用k会导致泄露d

  若同一用户使用相同的k进行了两次签名，可列出式子：

  $$s_1=k^{-1}(e_1+dr_1) mod\ n$$

  $$s_2=k^{-1}(e_2+dr_2) mod\ n$$

  从而$$s_1(e_2+dr_2)=s_2(e_1+dr_1)$$其中$r_1、s_1、r_2、s_2、e_1、e_2$皆已知，从而可求出$$d$$

  ![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_10-36-51.png)

  

- 两个用户若使用相同的$$k$$，则可以相互推出彼此的私钥$$d$$

  $$s_1=k^{-1}(e_1+d_1r_1) mod\ n$$

  $$s_2=k^{-1}(e_2+d_2r_2) mod\ n$$

  对于用户1，$$s_1(e_2+d_2r_2)=s_2(e_1+d_1r_1)$$其中$r_1、s_1、r_2、s_2、e_1、e_2、d_1$已知从而可求出用户2的私钥$d_2$。用户2同理

  ![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_10-42-48.png)

代码运行结果：

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_10-43-31.png" alt="Snipaste_2022-07-30_10-43-31" style="zoom:80%;" />

### Implement ECMH scheme

把集合中的元素哈希映射成椭圆曲线上的点，再利用ECC的加法将元素求和输出，使得

$Hash(a,b)=Hash(b,a)、Hash(Hash(a,b,c))=Hash(a,Hash(b,c))$

项目实现基于GmSSL中SHA256、EC等源码，将实现功能封装与ecmh类中

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_10-56-35.png" alt="Snipaste_2022-07-30_10-56-35" style="zoom:67%;" />

测试运行结果如下：

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_10-58-43.png)



### PGP scheme with SM2

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_11-02-45.png" alt="Snipaste_2022-07-30_11-02-45" style="zoom:80%;" />

基于python gmssl sm2实现上述PGP方案，分为发送方PGP_sender和接收方PGP_receiver，channel文件夹模拟传输信道

- PGP_sender

  运行：python PGP_sender.py -s str -p path

  -s：加密传输的消息

  -p：加密后消息存储路径，即channel文件夹

  分两步：加密、编码后存为json文件

  ![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_13-22-32.png)

  

- PGP_receiver

  运行：python PGP_receiver.py

  读取channel文件夹中的所有json文件进行解码、解密，输出传输内容![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_13-25-04.png)
  
  

### SM2 2P sign 

<img src="https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-31_11-24-59.png" alt="Snipaste_2022-07-31_11-24-59" style="zoom:80%;" />

依照上述流程基于python实现，设计python 库包含gmssl、crypto等通过pip 安装

- 先运行server.py文件待建立连接
- 再运行user.py文件进行交互生成签名

运行结果如下：![Snipaste_2022-07-31_11-32-09](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-31_11-32-09.png)
