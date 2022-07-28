import json
import os
import base64
from gmssl import sm2
from gmssl.sm4 import CryptSM4, SM4_DECRYPT

private_key = '00B9AB0B828FF68872F21A837FC303668428DEA11DCD1B24429D0C99E24EED83D5'
public_key = 'B9C9A6E04E9C91F7BA880429273747D7EF5DDEB0BB2FF6317EB00BEF331A83081A6994B8993F3F5D6EADDDB81872266C87C018FB4162F5AF347B483E24620207'

def receive_str(path):
    with open(path) as read_file:
        data=json.load(read_file)
    Enc_str=data["Enc_str"]
    PK_Enc=data["PK_Enc"]
        
    
    #解码
    Enc_str = Enc_str.encode('utf-8')
    Enc_str = base64.b64decode(Enc_str)
    PK_Enc = PK_Enc.encode('utf-8')
    PK_Enc = base64.b64decode(PK_Enc)


    #解密
    SM2=sm2.CryptSM2(public_key=public_key, private_key=private_key)
    session_key=SM2.decrypt(PK_Enc)
    SM4=CryptSM4()
    SM4.set_key(session_key,SM4_DECRYPT)
    message=SM4.crypt_ecb(Enc_str)
    print(message.decode())

if __name__ == '__main__':
    filelists=os.listdir("./channel")
    for filename in filelists:
        filepath="./channel/"+filename
        receive_str(filepath)
