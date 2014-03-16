#include "stdafx.h"

#include "../source/ContiguousList.h"

#include <stdio.h>

//Compile lodepng
//Use defines to override SDL read/write with normal read write
#define SDL_RWops FILE
#define SDL_RWFromFile fopen
#define SDL_RWseek fseek
#define SDL_RWtell ftell
#define SDL_RWread(context,ptr,size,maxnum) fread(ptr,size,maxnum,context)
#define SDL_RWwrite(context,ptr,size,maxnum) fwrite(ptr,size,maxnum,context)
#define SDL_RWclose fclose
#define RW_SEEK_END SEEK_END
#define RW_SEEK_SET SEEK_SET
#include "../source/lodepng.cpp"


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
		ContiguousList<int*> listOfNumberPointers;
		for (int i = 0; i < sizeof(test)/sizeof(int);i++) {
			int * n = new int;
			*n = test[i];
			listOfNumberPointers.push_back(n);
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
		ContiguousList<int*> listOfNumberPointers;
		vector<int*> correctList;
		for (int i = 0; i < sizeof(test)/sizeof(int);i++) {
			int * n = new int;
			*n = test[i];
			listOfNumberPointers.push_back(n);
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

	//Add refleciton unit tests
	reflectiontestsRegister();

	int testsPassed = 0;
	//Unit tests are executed here
	for (auto test : units) {
		try {
			test.test();
			testsPassed++;
		}
		catch (int & err) {
			cout << "TEST \"" << test.name << "\" FAILED\n";
		}
	}

	cout << "Tests passed: (" << testsPassed << "/" << units.size() << ")\n";
 	return 0;
}