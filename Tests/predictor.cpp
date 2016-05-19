

#include "../Predictor.h"
#include "../DriverConstants.h"

#include <iostream>

using namespace std;

int main(){
	Predictor::init(0);

	cout << 0 << '\t';

	for (int j = 0; j < 100; j+=11){
		for (int i = 0; i < DriverConstants::PredictsPerValue; i++){
			cout << Predictor::predict() << ' ';
		}
	
		cout<<endl;
		
		Predictor::freshPhase(j);
		cout << j << '\t';
	}

	cout << endl;
}
