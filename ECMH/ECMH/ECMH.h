#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <assert.h>
#include <vector>
#include <string>

using namespace std;

class ecmh {
	EC_POINT* point;
	EC_GROUP* group;
	BIGNUM* x, * y;
	BN_CTX* ctx;
	size_t num;

public:
	ecmh();
	~ecmh();

	void init(vector<int> vt);
	EC_POINT* hash2point(int data);
	void add_num(int data);
	void show();
};
