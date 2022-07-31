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

