import argparse
import secrets
import json
from gmssl import sm2
from gmssl.sm4 import CryptSM4, SM4_ENCRYPT
import base64

public_key = 'B9C9A6E04E9C91F7BA880429273747D7EF5DDEB0BB2FF6317EB00BEF331A83081A6994B8993F3F5D6EADDDB81872266C87C018FB4162F5AF347B483E24620207'

def send_str(args):
    session_key=secrets.token_bytes(16)
    SM4=CryptSM4()
    SM4.set_key(session_key,SM4_ENCRYPT)
    Enc_str=SM4.crypt_ecb(bytes(args.string,encoding='utf-8'))
    SM2=sm2.CryptSM2(public_key=public_key, private_key=None)
    PK_Enc=SM2.encrypt(session_key)

    Enc_str=base64.b64encode(Enc_str)
    Enc_str = Enc_str.decode('utf-8')
    PK_Enc=base64.b64encode(PK_Enc)
    PK_Enc = PK_Enc.decode('utf-8')

    with open("./channel/test.json",'a+') as new_file:
        data={"PK_Enc":PK_Enc,"Enc_str":Enc_str}
        json.dump(data,new_file)


def send_doc(args):
    with open(args.path, "r") as f:
        data = f.readline()
        print(data)




if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='the sender of PGP with SM2')

    parser.add_argument('--method', '-m', type=str, default='none')
    parser.add_argument('--string', '-s', type=str, default='none')
    parser.add_argument('--path', '-p', type=str, default='none')

    arguments = parser.parse_args()
    if arguments.method=='string':
        send_str(arguments)