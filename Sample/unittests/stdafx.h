
#include "../source/stdafx.h"

#define FAIL throw 1


class unittest {
public:
	string name;
	function<void()> test;
	unittest(string name, function<void()> test) {
		this->name = name;
		this->test = test;
	}
};

extern vector<unittest> units;
void TEST(string name, function<void()> test);
void reflectiontestsRegister();