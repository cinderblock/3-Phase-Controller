
#ifndef SUBSETTEST_H
#define SUBSETTEST_H

#include <string>
#include <climits>
#include <list>

using namespace std;

typedef unsigned long long ull;

//class used to test against smaller portions of data set
//used for testing effectivenss in differnt conditions
class SubSetTest{
	static list<SubSetTest*>* testingSet;

	string name;

	ull tstart;
	ull tend;

	ull count;
	double errorSum;

	void testSelf(double tick, double e2);


public:
	static void init();

	static void runTest(double tick, double error);

	static string reportTests();

	SubSetTest(string n, ull start, ull end);

	inline SubSetTest(string name, ull start){
		SubSetTest(name, start, ULLONG_MAX);
	}


	string report();
};

#endif
