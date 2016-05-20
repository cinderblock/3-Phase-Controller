

#include "../Predictor.h"
#include "../DriverConstants.h"
#include <csv_parser/csv_parser.hpp>

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

csv_parser file_parser;

void setUp(){


	/* Declare the variables to be used */
	const char * filename = "MagData.csv";
	const char field_terminator = ',';
	const char line_terminator  = '\n';
	const char enclosure_char   = '"';


	/* Define how many records we're gonna skip. This could be used to skip the column definitions. */
	file_parser.set_skip_lines(1);

	/* Specify the file to parse */
	file_parser.init(filename);

	/* Here we tell the parser how to parse the file */
	file_parser.set_enclosed_char(enclosure_char, ENCLOSURE_OPTIONAL);

	file_parser.set_field_term_char(field_terminator);

	file_parser.set_line_term_char(line_terminator);

}

double max = 0;

double differnecewithwrap(float a, float b){
	if (abs(int(a - b)) > (DriverConstants::StepsPerCycle / 2)){
		if ((b - a) > 0)
			return b-a-DriverConstants::StepsPerCycle;
		else
			return b-a+DriverConstants::StepsPerCycle;

	}

	return b-a;
}

int main(){

	setUp();
	ofstream output;

	output.open("out.csv");
	output <<"tick,predicted,actual (approx),error"<<endl;

	csv_row previousRow = file_parser.get_row();
	csv_row row = file_parser.get_row();

	Predictor::init(stoi(previousRow[1]));
	output <<previousRow[0]<< ","<<stoi(previousRow[1])<<"," << stoi(previousRow[1])<<','<<0<<endl;

	// cout<< lastRow[1] << '\t';


	while(file_parser.has_more_rows()){


		// cout << stoi(row[1]) << endl;

		for (int i = 0; i < DriverConstants::PredictsPerValue; i++){
			u2 pred = Predictor::predict();

			double actual = stoi(previousRow[1]) - differnecewithwrap(stoi(row[1]), stoi(previousRow[1]))
								       * ((double)i / DriverConstants::PredictsPerValue);

			if (actual < 0)
				actual += DriverConstants::StepsPerCycle;
			else if (actual >= DriverConstants::StepsPerCycle)
				actual -= DriverConstants::StepsPerCycle;

			output << stoi(previousRow[0]) + ((double)i / DriverConstants::PredictsPerValue) << ','
				<<pred<<','
				<<actual<<','
				<<differnecewithwrap(pred, actual)
				<<endl;
		}
	
		// cout<<endl;
		
	    /* Get the record */
	    previousRow = row;
	    row = file_parser.get_row();
		Predictor::freshPhase(stoi(previousRow[1]));

		// cout << endl<< stoi(lastRow[1]) << '\t';
		// output << previousRow[0]<< ","<<stoi(previousRow[1])<<"," << stoi(previousRow[1])<<endl;

	}





	for (int j = 0; j < 100; j+=11){
	}

	cout << endl;
}

