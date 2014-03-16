#include "stdafx.h"

//FAKE glm so that the unit tests compile
struct vec2 {
	float x,y;
};
struct vec3 {
	float x,y,z;
};
struct vec4 {
	float x,y,z,w;
};

//add cppjson to compile in this file
#include "../source/jsoncpp.cpp"

//Add the entire save system here
#include "../source/ReflectionData.h"
#include "../source/ReflectionStore.h"
#include "../source/Savable.h"
#include "../source/LoadData.h"
//Add the reflection code to this file
#include "../source/ReflectionData.cpp"
#include "../source/ReflectionStore.cpp"
#include "../source/Savable.cpp"
#include "../source/LoadData.cpp"

#include <fstream>





template <class T>
void TESTSERIALIZE() {
	T original;
	//Fill the object with data
	original.FillWithData();

	//Serialize
	vector<unsigned char> serialized = Savable::Serialize(&original);

	ofstream savefile("savefile.json",ios::binary);
	for (unsigned int i = 0; i < serialized.size();i++)
		savefile << serialized[i];

	//Create a new instance to load data into
	T newObject;

	//Deserialize
	Savable::Deserialize(serialized,&newObject);

	//now check if they're equal
	if (newObject == original)
		return;

	//Otherwise fail
	FAIL;
}


//all reflection test classes here:
//TEST ONE (a basic class-in-class)
class basicSavable : public Savable {
public:
	int a;
	float b;
	double c;
	string d;
	bool e;

	void FillWithData(int starter = 1) {
		a = 52348 * starter;
		b = 9486.981392f;
		c = 94539068.983423478;
		d = "heiyjse geosjo 398059 lsjafiop 8646+894 opfjeowaf83yu295y djsfjaaa";
		e = false;
	}

	bool operator==(const basicSavable & otherSavable) const {
		bool q =  (a == otherSavable.a);
		bool w =  (b == otherSavable.b);
		bool t =  (c == otherSavable.c);
		bool s =   (d == otherSavable.d);
		bool v =   (e == otherSavable.e);

		return q && w && t && s && v;
	}


	CLASS_DECLARATION(basicSavable)
		CLASS_MEMBER(a,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(b,ReflectionData::SAVE_FLOAT)
		CLASS_MEMBER(c,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(d,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(e,ReflectionData::SAVE_BOOL)
	END_DECLARATION
};
//TEST TWO (a class which owns a class)
//Allow the previous class to be constructed
CLASS_SAVE_CONSTRUCTOR(basicSavable);

//define a new class which contains the previous class
class savableOwner : public Savable {
public:
	int a;
	string b;
	basicSavable c;
	basicSavable * d;

	void FillWithData() {
		a = -19203193;
		b = " aw9t slgjksj 923908234902 popo #@*%(FIOSDJ IOGSJEOIGJEOS 90I*()^%*()6 346934ugsd";
		c.FillWithData();
		d = new basicSavable();
		d->FillWithData();
	}

	bool operator==(const savableOwner & otherSavable) const {
		return ((a == otherSavable.a) && (b == otherSavable.b) && (c == otherSavable.c) && (*d == *otherSavable.d));
	}

	CLASS_DECLARATION(savableOwner)
		CLASS_MEMBER(a,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(b,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(c,ReflectionData::SAVE_INSTANCE)
		CLASS_MEMBER(d,ReflectionData::SAVE_OWNEDHANDLE)
	END_DECLARATION
};


//TEST THREE (a class which owns a list of owned objects)

//Determine if two containers contain equivalent data
template<class T>
bool ContainerEquivalent(T a, T b) {
	auto iteratorA = a.begin();
	auto iteratorB = b.begin();
	for (; (iteratorA != a.end()) && (iteratorB != b.end()); iteratorA++, iteratorB++) {
		if (*iteratorA == *iteratorB)
			continue;

		return false;
	}
	return (iteratorA == a.end()) && (iteratorB == b.end());
}

template<class T>
bool PointerContainerEquivalent(T a, T b) {
	auto iteratorA = a.begin();
	auto iteratorB = b.begin();
	for (; (iteratorA != a.end()) && (iteratorB != b.end()); iteratorA++, iteratorB++) {
		if (**iteratorA == **iteratorB)
			continue;

		return false;
	}
	return (iteratorA == a.end()) && (iteratorB == b.end());
}

class containerOwner : public Savable {
public:
	vector<double> a;
	list<int> b;
	list<string> c;
	vector<basicSavable*> d;


	void FillWithData() {
		for (int i = 0; i < 100; i++) {
			a.push_back((i << 2) * i - .05*i);
			b.push_back(i*100 - 59340853 % (i+1));
			{basicSavable * sav = new basicSavable(); sav->FillWithData(); d.push_back(sav);}
		}
		for (int i = 0; i < 50; i++) {
			b.push_back(i*100 - 59340853 % (i+1));
			c.push_back(string("lsiefjeisji 923589 aqwieiwe mcxv") + (char)(i+5) + "x");
		}
		for (int i = 0; i < 75; i++) {
			a.push_back((i << 2) * i - .05*i + 100.5);
			c.push_back(string("ls iefj903 4860347dsjkfaisdjf eiwe mcxv") + (char)(i+5) + "x");
			{basicSavable * sav = new basicSavable(); sav->FillWithData(); d.push_back(sav);}
		}
	}

	bool operator==(const containerOwner & otherSavable) const {
		return ContainerEquivalent<vector<double>>(a,otherSavable.a) && ContainerEquivalent(b,otherSavable.b) && ContainerEquivalent(c,otherSavable.c) && PointerContainerEquivalent(d,otherSavable.d);
	}


	CLASS_DECLARATION(containerOwner)
		CLASS_CONTAINER_MEMBER(a,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_DOUBLE)
		CLASS_CONTAINER_MEMBER(b,ReflectionData::SAVE_LIST,ReflectionData::SAVE_INT32)
		CLASS_CONTAINER_MEMBER(c,ReflectionData::SAVE_LIST,ReflectionData::SAVE_STRING)
		CLASS_CONTAINER_MEMBER(d,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_OWNEDHANDLE)
	END_DECLARATION
};


//TEST FOUR - INHERITANCE 
class crazyInheritanceBase : public Savable {
public:
	int x;
	double y;
	vector<basicSavable*> z;

	void FillWithData() {
		x = 593459;
		y = 4842341.09805662;
		for (int i = 0; i < 214; i++) {
			basicSavable * m = new basicSavable();
			m->FillWithData(i);
			z.push_back(m);
		}
	}
	bool operator==(const crazyInheritanceBase & otherSavable) const {
		return PointerContainerEquivalent(z,otherSavable.z) && (x == otherSavable.x) && (y == otherSavable.y);
	}

	CLASS_DECLARATION(crazyInheritanceBase)
		CLASS_MEMBER(x,ReflectionData::SAVE_INT32)
		CLASS_MEMBER(y,ReflectionData::SAVE_DOUBLE)
		CLASS_CONTAINER_MEMBER(z,ReflectionData::SAVE_VECTOR,ReflectionData::SAVE_OWNEDHANDLE)
	END_DECLARATION
};

class crazyOtherBase : public crazyInheritanceBase {
public:
	double named;
	string other;

	void FillWithData() {
		crazyInheritanceBase::FillWithData();
		other = "9gioj g834ugiodjs gu82a385u23";
		named = 3925.98476;
	}

	bool operator==(const crazyOtherBase & otherSavable) const {
		return (named == otherSavable.named) && (other == otherSavable.other) && (*(crazyInheritanceBase*)this == (crazyInheritanceBase)otherSavable);
	}

	CLASS_DECLARATION(crazyOtherBase)
		INHERITS_FROM(crazyInheritanceBase)
		CLASS_MEMBER(named,ReflectionData::SAVE_DOUBLE)
		CLASS_MEMBER(other,ReflectionData::SAVE_STRING)
	END_DECLARATION
};

class crazyInheritanceDerived : public crazyOtherBase {
public:
	string abc;
	basicSavable * firstInOther;


	void FillWithData() {
		crazyOtherBase::FillWithData();
		//savableOwner::FillWithData();
		abc = "kisjio iej 9238059 11 ``` ";
		firstInOther = z[10];
	}
	bool operator==(const crazyInheritanceDerived & otherSavable) const {
		return (abc == otherSavable.abc) && (*firstInOther == *otherSavable.firstInOther) && (*(crazyOtherBase*)this == (crazyOtherBase)otherSavable);
	}

	CLASS_DECLARATION(crazyInheritanceDerived)
		INHERITS_FROM(crazyOtherBase)
		//INHERITS_FROM(savableOwner)
		CLASS_MEMBER(abc,ReflectionData::SAVE_STRING)
		CLASS_MEMBER(firstInOther,ReflectionData::SAVE_HANDLE)
	END_DECLARATION
};







#define REFLECTIONTEST(withType) TEST("Reflection::"## #withType,[](){TESTSERIALIZE<withType>();})

void reflectiontestsRegister() {
	REFLECTIONTEST(basicSavable);
	REFLECTIONTEST(savableOwner);
	REFLECTIONTEST(containerOwner);
	REFLECTIONTEST(crazyInheritanceDerived);
}