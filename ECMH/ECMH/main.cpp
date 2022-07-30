#include "ECMH.h"

int main() {
	ecmh test1, test2, test3;

	vector<int> vt1 = { 1,2,3 };
	vector<int> vt2 = { 3,2,1 };
	vector<int> vt3 = { 1,3 };
	test1.init(vt1);
	test2.init(vt2);
	test3.init(vt3);
	test3.add_num(2);

	test1.show();
	test2.show();
	test3.show();
}