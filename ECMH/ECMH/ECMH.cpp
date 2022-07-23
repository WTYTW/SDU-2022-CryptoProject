#include "ECMH.h"

string hash_string(string data) {
	EVP_MD_CTX* SHActx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(SHActx, EVP_sha256(), NULL);
	unsigned char hash_data[SHA256_DIGEST_LENGTH];
	unsigned int len = data.size();
	EVP_DigestUpdate(SHActx, data.data(), len);
	EVP_DigestFinal_ex(SHActx, hash_data, &len);
	EVP_MD_CTX_free(SHActx);
	return string(reinterpret_cast<char*>(hash_data), SHA256_DIGEST_LENGTH);
}

ecmh::ecmh() {
	x = y = NULL;
	x = BN_new();
	ctx = BN_CTX_new();
	num = 0;
}

ecmh::~ecmh() {
	BN_free(x);
	BN_CTX_free(ctx);
}

void ecmh::init(vector<int> vt) {
	group = EC_GROUP_new_by_curve_name(NID_sm2p256v1);
	point = EC_POINT_new(group);
	assert(EC_POINT_set_to_infinity(group, point));//判断是否为无穷远点
	num = vt.size();
	for (auto& i : vt) add_num(i);
}

EC_POINT* ecmh::hash2point(int data) {
	string hash_str = hash_string(to_string(data));
	unsigned char* buf = (unsigned char*)hash_str.c_str();
	EC_POINT* dpoint = EC_POINT_new(group);
	BN_bin2bn(buf, SHA256_DIGEST_LENGTH, x);
	EC_POINT_set_compressed_coordinates_GFp(group, dpoint, x, 0, ctx);
	while (!EC_POINT_is_on_curve(group, dpoint, ctx) || EC_POINT_is_at_infinity(group, dpoint)) {
		BN_add_word(x, 1);
		EC_POINT_set_compressed_coordinates_GFp(group, dpoint, x, 0, ctx);
	}
	return dpoint;
}

void ecmh::add_num(int data) {
	EC_POINT* p = hash2point(data);
	EC_POINT_add(group, point, point, p, ctx);
	num++;
}

void ecmh::show() {
	char* hashdata = EC_POINT_point2hex(group, point, POINT_CONVERSION_COMPRESSED, ctx);
	printf("%s\n", hashdata);
}