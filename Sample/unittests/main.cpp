#define FAIL throw 1
#include "../source/stdafx.h"
#include "../source/ContiguousList.h"


class unittest {
public:
	string name;
	function<void()> test;
	unittest(string name, function<void()> test) {
		this->name = name;
		this->test = test;
	}
};

//Register unit tests
vector<unittest> units;
//helper function
void TEST(string name, function<void()> test) {
	units.push_back(unittest(name,test));
}

int main(int argc, char** argv) {
	//Each unit test is added here
	TEST("contiguous list iterate",[]() {
		int test[] = {5, -3, 12, 976, 0, 1, 25, -5, -18, -5, 20};
		ContiguousList<int> listOfNumberPointers;
		for (int i = 0; i < sizeof(test)/sizeof(int);i++) {
			int * n = new int;
			*n = test[i];
			listOfNumberPointers.insert(n);
		}
		int x = 0;
		for (auto it : listOfNumberPointers) {
			//If they fail to match the result, fail
			if (*it != test[x])
				FAIL;
			x++;
		}
	});
	TEST("contiguous list sort",[]() {
		int test[] = {5, -3, 12, 976, 0, 1, 25, -5, -18, -5, 20};
		ContiguousList<int> listOfNumberPointers;
		vector<int*> correctList;
		for (int i = 0; i < sizeof(test)/sizeof(int);i++) {
			int * n = new int;
			*n = test[i];
			listOfNumberPointers.insert(n);
			correctList.push_back(n);
		}

		listOfNumberPointers.sort([](int * a, int * b) -> bool {
			return *a < *b;
		});
		sort(correctList.begin(),correctList.end(),[](int * a, int * b) -> bool {
			return *a < *b;
		});


		int x = 0;
		for (auto it : listOfNumberPointers) {
			//If they fail to match the result, fail
			if (*it != *correctList[x])
				FAIL;
			x++;
		}
	});

	int testsPassed = 0;
	//Unit tests are executed here
	for (auto test : units) {
		try {
			test.test();
			testsPassed++;
		}
		catch (...) {
			cout << "TEST \"" << test.name << "\" FAILED\n";
		}
	}
	cout << "Tests passed: (" << testsPassed << "/" << units.size() << ")\n";
 	return 0;
}