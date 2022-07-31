### length extension attack

基于GmSSL sm3和sha256，设置key=“password”，message="length extension attack"

可求得哈希值：

$h_1=sm3(key||message)$、$h_2=sha256(key||message)$

对其进行长度扩展攻击，添加消息extension=“sha256&sm3”

参照sm3和sha256的compress函数在key||message后添加特定比特得到M，在M后级联extension得到新消息M'，从而可在key未知的前提下构造新哈希值

$h_1'=sm3(M||extension)$、$h_2'=sha256(M||extension)$可通过验证

![image](https://github.com/WTYTW/SDU-2022-CryptoProject/blob/main/picture/Snipaste_2022-07-30_15-06-55.png)

