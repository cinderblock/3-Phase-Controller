
#include "SubSetTest.h"
#include <iostream>

list<SubSetTest*>* SubSetTest::testingSet;

void SubSetTest::testSelf(double tick, double e2){
	if (tick > tstart && tick < tend){
		count++;
		errorSum += e2;
	}
}

string SubSetTest::report(){
	if (count != 0)
		return name+" "+to_string(errorSum/count);
	else
		return name+" Never run";
}

void SubSetTest::init(){
	// testingSet;
	testingSet = new list<SubSetTest*>();
}

void SubSetTest::runTest(double tick, double error){

	for (list<SubSetTest*>::iterator itr = testingSet->begin(); itr != testingSet->end(); itr++){
		(*itr)->testSelf(tick, error);
	}
}

string SubSetTest::reportTests(){
	string s = "";

	for (list<SubSetTest*>::iterator itr = testingSet->begin(); itr != testingSet->end(); itr++){
		s += (*itr)->report() + '\n';
	}

	return s;
}

string SubSetTest::reportVals(){
	string s = "";

	for (list<SubSetTest*>::iterator itr = testingSet->begin(); itr != testingSet->end(); itr++){
		s += to_string((*itr)->errorSum/(*itr)->count) + ',';
	}

	return s;
}

SubSetTest::SubSetTest(string n, ull start, ull end){
	name = n;

	tstart = start;
	tend = end;

	count = 0;
	errorSum = 0;

	testingSet->push_back(this);
}

string SubSetTest::getCSVNames(){

	string s = "";

	for (list<SubSetTest*>::iterator itr = testingSet->begin(); itr != testingSet->end(); itr++){
		s += (*itr)->name + ',';
	}

	return s;
}

void SubSetTest::reset(){
	for (list<SubSetTest*>::iterator itr = testingSet->begin(); itr != testingSet->end(); itr++){
		(*itr)->resetSelf();
	}
}
