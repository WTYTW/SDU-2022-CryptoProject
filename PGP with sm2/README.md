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
  
