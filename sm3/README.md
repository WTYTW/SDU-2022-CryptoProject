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

