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

