
#include "ThreePhaseCalibrator.h"
#include "MLX90363.h"
#include "ThreePhaseDriver.h"

u2 ThreePhaseCalibrator::position;
u1 ThreePhaseCalibrator::magRoll;

void ThreePhaseCalibrator::init(){
	MLX90363::init();
	ThreePhaseDriver::init();
	position = 0;

	ThreePhaseDriver::setAmplitude(0);

	MLX90363::prepareGET1Message(MLX90363::MessageType::Alpha);


	magRoll = MLX90363::getRoll();

	// Get two new readings to get started
	while (!MLX90363::hasNewData(magRoll));
	while (!MLX90363::hasNewData(magRoll));


}

void ThreePhaseCalibrator::goTo(u2 pos){
	if(pos <= MAX)
		position = pos;
	else
		position = MAX;
}

void ThreePhaseCalibrator::advance(){
	
	if (++position > MAX){
		position = 0;
	}

}
