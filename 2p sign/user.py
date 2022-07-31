
from gmssl import sm3, func
from Crypto.Util.number import inverse
import sys
import socket
import binascii
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

address = ('127.0.0.1', 12321)
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    client.connect(('127.0.0.1', 12321))
    print("连接建立")
except Exception:
    print('连接失败')
    sys.exit()
else:
#step 1------------------------------------------
    d1 = randint(1,N-1)
    P1 = EC_mul(inverse(d1,P),G)
    x,y = hex(P1[0]),hex(P1[1])
    
    client.sendto(x.encode('utf-8'), address)
    client.sendto(y.encode('utf-8'), address)

#step 3------------------------------------------
    msg = "SM2 2P sign with real network communication"
    msg = hex(int(binascii.b2a_hex(msg.encode()).decode(), 16)).upper()[2:]
    ID_user = "WTYTW"
    ID_user = hex(int(binascii.b2a_hex(ID_user.encode()).decode(), 16)).upper()[2:]
    ENTL_A = '{:04X}'.format(len(ID_user) * 4)
    ide = ENTL_A + ID_user + '{:064X}'.format(A) + '{:064X}'.format(B) + '{:064X}'.format(G_X) + '{:064X}'.format(G_Y)
    
    Z = sm3.sm3_hash(func.bytes_to_list(ide.encode()))
    M=Z + msg
    e = sm3.sm3_hash(func.bytes_to_list(M.encode()))
    k1 = randint(1,N-1)
    Q1 = EC_mul(k1,G)
    x,y = hex(Q1[0]),hex(Q1[1])

    client.sendto(x.encode('utf-8'),address)
    client.sendto(y.encode('utf-8'),address)
    client.sendto(e.encode('utf-8'),address)

#step 5------------------------------------------
    r,_ = client.recvfrom(1024)
    r = int(r.decode(),16)
    s2,_ = client.recvfrom(1024)
    s2 = int(s2.decode(),16)
    s3,_ = client.recvfrom(1024)
    s3 = int(s3.decode(),16)
    s=((d1 * k1) * s2 + d1 * s3 - r)%N
    print(f"Signature : {hex(r)} {hex(s)}")
    client.close()