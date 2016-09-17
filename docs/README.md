# Three phase motor controller

The following is the documentation for the three phase motor controller, an open source project.
Our current use case is on the "Turingy Driver board" with the Turnigy 480 

## General System

The motor controller has:
- [Output](#output)
  - [Servo](#servo)
  - [Controller](#controller)
  - [Driver](#driver)
- [Sensor](#) / [Predictor](#)
- I2C communication (independently supported)

------

### Output<a name="output"></a>

The servo simulates the 3 sin waves needed to drive a 3-phase motor with a ~32KHz PWM of 3 half-H bridges.


#### Driver<a name="driver"></a>

code [here](../ThreePhaseDriver.h)

The driver handles the sin wave generation based on a number `[0-767]`. 
It also accepts an amplitude `[0-255]`.

Based on the math of [3 Phase sine generation](https://docs.google.com/spreadsheets/d/1I45kGhncSQvR4_B_AG72Bqk7MJlNRIvBI-JD9qAgE8U/edit?usp=sharing), one output can be held at ground and the other 2 are the difference between themselves and the low phase. Each of the two non-ground phases are symmetric and therefore can use the same table (aka limitedSinTable).

Amplitude adjusts the output so the motor doesn't go full force. It uses an approximate division by 255.

#### Controller<a name="controller"></a>

code [here](../ThreePhaseController.h)


#### Servo<a name="servo"></a>



------

### Sensor<a name="estimator"></a>


#### Predictor<a name="estimator"></a>

