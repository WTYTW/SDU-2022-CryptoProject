#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <stdio.h>

using namespace std;

BIGNUM* pitfall1(EC_KEY* key, BIGNUM* kinv, ECDSA_SIG* signature, const unsigned char* dgst, int dgst_len) {
    BIGNUM* r = NULL, * s = NULL;
    ECDSA_SIG_get0(signature, (const BIGNUM**)&r, (const BIGNUM**)&s);

    BN_CTX* ctx = BN_CTX_new();
    const EC_GROUP* group = EC_KEY_get0_group(key);
    BIGNUM* order = BN_new();
    EC_GROUP_get_order(group, order, ctx);
    BIGNUM* k = BN_new();
    BN_mod_inverse(k, kinv, order, ctx);
    BIGNUM* e = BN_new();
    BN_bin2bn(dgst, dgst_len, e);

    BIGNUM* d = BN_new();
    BN_mod_mul(d, s, k, order, ctx);
    BN_mod_sub(d, d, e, order, ctx);
    BIGNUM* rinv = BN_new();
    BN_mod_inverse(rinv, r, order, ctx);
    BN_mod_mul(d, d, rinv, order, ctx);

    BN_CTX_free(ctx);
    ctx = NULL;
    BN_clear_free(order);
    BN_clear_free(k);
    BN_clear_free(e);
    BN_clear_free(r);
    BN_clear_free(s);
    BN_clear_free(rinv);
    return d;
}

BIGNUM* pitfall2(EC_KEY* key, ECDSA_SIG* signature1, ECDSA_SIG* signature2, const unsigned char* dgst1, int dgst_len1, const unsigned char* dgst2, int dgst_len2) {
    BIGNUM* r1 = BN_new(), * s1 = BN_new();
    ECDSA_SIG_get0(signature1, (const BIGNUM**)&r1, (const BIGNUM**)&s1);

    BIGNUM* r2 = BN_new(), * s2 = BN_new();
    ECDSA_SIG_get0(signature2, (const BIGNUM**)&r2, (const BIGNUM**)&s2);

    if (!BN_cmp(r1, r2))
        cout << "same!" << endl;

    BIGNUM* e1 = BN_new();
    BN_bin2bn(dgst1, dgst_len1, e1);
    BIGNUM* e2 = BN_new();
    BN_bin2bn(dgst2, dgst_len2, e2);

    BN_CTX* ctx = BN_CTX_new();
    const EC_GROUP* group = EC_KEY_get0_group(key);
    BIGNUM* order = BN_new();
    EC_GROUP_get_order(group, order, ctx);

    BN_mod_mul(e2, e2, s1, order, ctx);
    BN_mod_mul(e1, e1, s2, order, ctx);
    BN_mod_sub(e2, e2, e1, order, ctx);
    BN_mod_mul(r1, r1, s2, order, ctx);
    BN_mod_mul(r2, r2, s1, order, ctx);
    BN_mod_sub(r1, r1, r2, order, ctx);
    BN_mod_inverse(r1, r1, order, ctx);
    BIGNUM* d = BN_new();
    BN_mod_mul(d, e2, r1, order, ctx);

    return d;
}



int main(int argc, char** argv)
{
    const char msg1[] = "abcd";
    const char msg2[] = "efgh";

    unsigned char dgst1[EVP_MAX_MD_SIZE];
    unsigned char dgst2[EVP_MAX_MD_SIZE];
    unsigned int dgst_len1 = 0;
    unsigned int dgst_len2 = 0;

    EVP_MD_CTX* md_ctx1 = EVP_MD_CTX_new();
    EVP_MD_CTX* md_ctx2 = EVP_MD_CTX_new();
    EVP_DigestInit(md_ctx1, EVP_sha256());
    EVP_DigestInit(md_ctx2, EVP_sha256());

    EVP_DigestUpdate(md_ctx1, (const void*)msg1, strlen(msg1));
    EVP_DigestFinal(md_ctx1, dgst1, &dgst_len1);
    EVP_DigestUpdate(md_ctx2, (const void*)msg2, strlen(msg2));
    EVP_DigestFinal(md_ctx2, dgst2, &dgst_len2);

    EC_KEY* key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    EC_KEY_generate_key(key);

    BIGNUM* kinv = BN_new(), * rp = BN_new();
    ECDSA_sign_setup(key, NULL, &kinv, &rp);

    ECDSA_SIG* signature1 = ECDSA_do_sign_ex(dgst1, dgst_len1, kinv, rp, key);
    ECDSA_SIG* signature2 = ECDSA_do_sign_ex(dgst2, dgst_len2, kinv, rp, key);

    if (ECDSA_do_verify(dgst1, dgst_len1, signature1, key) != 1) {
        cout << "signature1 verify faild" << endl;
        return false;
    }
    cout << "signature1 verify succeed!" << endl << endl;

    if (ECDSA_do_verify(dgst2, dgst_len2, signature2, key) != 1) {
        cout << "signature2 verify faild" << endl;
        return false;
    }
    cout << "signature2 verify succeed!" << endl << endl;

    /*****************************************************************************************/
    cout << "1.Leaking k leads to leaking of d" << endl;

    BIGNUM* d = pitfall1(key, kinv, signature1, dgst1, dgst_len1);
    const BIGNUM* private_key = EC_KEY_get0_private_key(key);
    if (!BN_cmp(d, private_key))
        cout << "ECDSA PITFALL1 succeed!" << endl;

    /*****************************************************************************************/
    cout << "2.Reusing k leads to leaking of d" << endl;
    d = pitfall2(key, signature1, signature2, dgst1, dgst_len1, dgst2, dgst_len2);
    if (!BN_cmp(d, private_key))
        cout << "ECDSA PITFALL2 succeed!" << endl;

	return 0;
}