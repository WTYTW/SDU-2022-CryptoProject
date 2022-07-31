from Crypto.Util.number import inverse
import socket
from random import randint

# 参数配置
P = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF
N = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123
A = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC
B = 0x28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93
G_X = 0x32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7
G_Y = 0xBC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0
G = (G_X, G_Y)


def EC_add(p, q):
    if p == 0 and q == 0: return 0
    elif p == 0: return q
    elif q == 0: return p
    else:
        if p[0] > q[0]:
            p, q = q, p
        slope = (q[1] - p[1])*inverse(q[0] - p[0], P) % P

        r = [(slope**2 - p[0] - q[0]) % P]
        r.append((slope*(p[0] - r[0]) - p[1]) % P)

        return (r[0], r[1])

def EC_double(p):
    slope = (3*p[0]**2 + A)*inverse(2*p[1], P) % P

    r = [(slope**2 - 2*p[0]) % P]
    r.append((slope*(p[0] - r[0]) - p[1])%P)

    return (r[0], r[1])

def EC_mul(s, p):
    n = p
    r = 0 
    s_binary = bin(s)[2:] 
    s_length = len(s_binary)
    for i in reversed(range(s_length)):
        if s_binary[i] == '1':
            r = EC_add(r, n)
        n = EC_double(n)

    return r



client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client.bind(('', 12321))

print("等待建立连接...")

#step 2------------------------------------------

d2 = randint(1,N-1)
x,address = client.recvfrom(1024)
x = int(x.decode(),16)
y,address = client.recvfrom(1024)
y = int(y.decode(),16)
P1 = (x,y)
P1 = EC_mul(inverse(d2,P),P1)
P1 = EC_add(P1,(G_X,-G_Y))

#step 4------------------------------------------
x,address = client.recvfrom(1024)
x = int(x.decode(),16)
y,address = client.recvfrom(1024)
y = int(y.decode(),16)
Q1 = (x,y)
e,address = client.recvfrom(1024)
e = int(e.decode(),16)
k2 = randint(1,N-1)
k3 = randint(1,N-1)
Q2 = EC_mul(k2,G)
x1,y1 = EC_mul(k3,Q1)
x1,y1 = EC_add((x1,y1),Q2)
r =(x1 + e)%N
s2 = (d2 * k3)%N
s3 = (d2 * (r+k2))%N
client.sendto(hex(r).encode(),address)
client.sendto(hex(s2).encode(),address)
client.sendto(hex(s3).encode(),address)
print("连接已关闭")