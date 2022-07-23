#include "ECMH.h"

int main() {
	ecmh test1, test2;

	vector<int> vt1 = { 1,2,3 };
	vector<int> vt2 = { 1,3 };
	test1.init(vt1);
	test2.init(vt2);
	test2.add_num(2);

	test1.show();
	test2.show();
}