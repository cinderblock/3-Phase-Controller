/*
This c program is testing the Predictor class used to predict 
the motor controllers current position between position updates
*/

#include "../Predictor.h"
#include "../DriverConstants.h"
#include <csv_parser/csv_parser.hpp>

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <climits>
#include <list>

#include "SubSetTest.h"


using namespace std;

const u1 speedMultiply = 1;
const bool WriteToFile = true;
const bool GetMaxes = false;
const bool fullTest = false;

const u1 tickCol = 0;
const u1 phaseCol = 2;
const u1 mechCol = 1;

ofstream output;
ofstream testOutput;

csv_parser file_parser;

void csvSetUp(){

	/* Declare the variables to be used */
	const char * filename = "MagData2.csv";
	const char field_terminator = ',';
	const char line_terminator	= '\n';
	const char enclosure_char	 = '"';



	/* Define how many records we're gonna skip. This could be used to skip the column definitions. */
	file_parser.set_skip_lines(1);

	/* Specify the file to parse */
	file_parser.init(filename);

	/* Here we tell the parser how to parse the file */
	file_parser.set_enclosed_char(enclosure_char, ENCLOSURE_OPTIONAL);

	file_parser.set_field_term_char(field_terminator);

	file_parser.set_line_term_char(line_terminator);


}

void setUp(){
	if(!fullTest && WriteToFile){
		output.open("out.csv");
		output <<"tick,predicted,actual (approx),mech,error,error^2"<<endl;
	}

	SubSetTest::init();
	// new SubSetTest("stopped", 0, 500);
	// new SubSetTest("Reverse", 5036, 5040);
	// new SubSetTest("HighSpeed", 3650, 3900);
	// new SubSetTest("BackForth", 6500, 8400);

	if(fullTest){
		testOutput.open("test.csv");
		testOutput <<"value,total error,"<<SubSetTest::getCSVNames()<<endl;

	}

}

double max = 0;

double differnecewithwrap(double a, double b, int wrap){
	if (abs(int(a - b)) > (wrap / 2)){
		if ((b - a) > 0)
			return b-a-wrap;
		else
			return b-a+wrap;

	}

	return b-a;
}

s2 maxDelta = 0;
ull maxDeltaPos = 0;

s2 maxDeltaDelta = 0;
ull maxDeltaDeltaPos = 0;

const long long probMaxDeltaDelta = 100;

u2 getInput(u2 mechPhase){

	u1 mech = (mechPhase/DriverConstants::StepsPerCycle);
	mech = DriverConstants::ElectricalPerMechanical-1-mech;
	u2 input = ((int)mech<<12)+mechPhase%DriverConstants::StepsPerCycle;
	// cout<<((int)mech<<12)<<' '<<mechPhase%DriverConstants::StepsPerCycle<<' ';

	return input;
}

u2 getMechPhase(u2 phase){
	return (phase & DriverConstants::MaskForPhase) + (phase >> 12) * DriverConstants::StepsPerCycle;
}

void runTest(u2 adj = 60){

	double errorSum = 0;
	ull num = 0;

	csv_row previousRow;// = file_parser.get_row();
	csv_row row = file_parser.get_row();

	{
		auto input = getInput(stoi(row[mechCol]));
		// cout<<input<<endl;
		Predictor::init(input);
		// Predictor::adjustVal = adj;
		// Predictor::ratio = adj;
	}

	s2 prevDelta = 0;

	while(file_parser.has_more_rows()){

		/* Get the record */
		previousRow = row;

		//dumb way of atifically speeding up motor
		//this way does not keep the refrence data in between computed cycles
		//in other words not as acurate to check again
		// for (int a = 0; a < speedMultiply; a++)
			row = file_parser.get_row();

		//double check for skipped ticks
		// if (stoi(row[tickCol]) - stoi(previousRow[tickCol]) != 1){
		// 	cout<<"skipped tick(s) ";
		// 	for (int i = stoi(previousRow[tickCol])+1; i < stoi(row[tickCol]); i++){
		// 		cout << i<< ' ';
		// 	}
		// 	cout << endl;
		// }

		// u1 mechPhase = (stoi(previousRow[mechCol])/DriverConstants::StepsPerCycle);
		// u2 input = mechPhase<<12+stoi(previousRow[phaseCol]);
		u2 input = getInput(stoi(previousRow[mechCol]));
		// cout<<input<<' ';
		Predictor::freshPhase(input);

		ull tick = stoi(previousRow[tickCol]) / speedMultiply;

		// cout << stoi(row[phaseCol]) << endl;

		for (int i = 1; i < DriverConstants::PredictsPerValue; i++){
			u2 pred = Predictor::predict();

			s2 delta = differnecewithwrap(stoi(row[phaseCol]), stoi(previousRow[phaseCol]), DriverConstants::StepsPerRotation);

			if(delta - prevDelta > probMaxDeltaDelta){
				delta -= DriverConstants::StepsPerCycle;

			}
			else if (delta - prevDelta < -probMaxDeltaDelta){
				delta += DriverConstants::StepsPerCycle;

				// cout << -probMaxDeltaDelta ;
			}


			if (GetMaxes){
				if(abs(delta) > abs(maxDelta)){
					maxDelta = delta;
					maxDeltaPos = stoi(previousRow[tickCol]);
				}
			
				if ((delta > 0 && prevDelta < 0) || (delta < 0 && prevDelta > 0)){
					if(abs(delta-prevDelta) > abs(maxDeltaDelta)){
						maxDeltaDelta = delta-prevDelta;
						maxDeltaDeltaPos = stoi(previousRow[tickCol]);
					}
				}
			}
			
			prevDelta = delta;
			
			double partialStep = ((double)i / DriverConstants::PredictsPerValue);

			double actual = stoi(previousRow[phaseCol]) - delta * partialStep;

			if (actual < 0)
				actual += DriverConstants::StepsPerCycle;
			else if (actual >= DriverConstants::StepsPerCycle)
				actual -= DriverConstants::StepsPerCycle;


			double error = differnecewithwrap(pred, actual, DriverConstants::StepsPerCycle);
			double e2 = error * error;

			errorSum += e2;
			num++;

			SubSetTest::runTest(stoi(previousRow[tickCol])+partialStep, e2);

			if(WriteToFile){
				output << tick
					//adds the decimal portion as a string or the full number is not reccorded in large numbers
					<< to_string(partialStep).substr(1,4) << ','
					<<pred<<','
					<<actual<<','
					<<(getMechPhase(getInput(stoi(previousRow[mechCol]))) - delta * partialStep)<<','
					<<error<<','
					<<e2
					<<endl;
			}
		}
	

		// cout<<endl;
		
		// cout << endl<< stoi(lastrow[phaseCol]) << '\t';
		// output << previousRow[tickCol]<< ","<<stoi(previousRow[phaseCol])<<"," << stoi(previousRow[phaseCol])<<endl;
	}

	if(!fullTest){
		cout << errorSum / num << endl;
		cout << SubSetTest::reportTests() << endl;
	}
	else{
		// testOutput << (int)(Predictor::adjustVal) << ','<<(errorSum / num)<<','<<SubSetTest::reportVals()<<endl;
	}
	
	if (GetMaxes){
		cout<<"maxDelta @ " << maxDeltaPos<<endl;
		cout<<"maxDeltaDelta @ " << maxDeltaDeltaPos<<endl;
	}

}


int main(int argc, char* argv[]){

	if(argc > 1){

	}

	setUp();


	if(!fullTest){
		csvSetUp();
		runTest();
	}
	else{
		for(int i = 0; i <= ((u1)-1); i++){
			SubSetTest::reset();
			csvSetUp();

			// cout<<i<<endl;
			runTest(i);
		}
	}
}

