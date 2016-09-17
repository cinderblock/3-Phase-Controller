# Three phase motor controller

The following is the documentation for the three phase motor controller, an open source project.
Our current use case is on the "Turingy Driver board" with the Turnigy 480 

## General System

The motor controller has:
- [Output](#output)
  - [Servo](#servo)
  - [Controller](#controller)
  - [Driver](#driver)
- [Sensing](#sense)
  - [Magnetometer](#mlx)
  - [Predictor](#predictor)
  - [Back EMF](#emf)
- I2C communication (independently supported)

------

### Output<a name="output"></a>

The servo simulates the 3 sin waves needed to drive a 3-phase motor with a ~32KHz PWM of 3 half-H bridges.


#### Driver<a name="driver"></a>

code [here](../ThreePhaseDriver.h)

The driver handles the sin wave generation based on a `[0-767]` integer. 
It also uses an amplitude `[0-255]`.

Based on the math of [3 Phase sine generation](https://docs.google.com/spreadsheets/d/1I45kGhncSQvR4_B_AG72Bqk7MJlNRIvBI-JD9qAgE8U/edit?usp=sharing), one output can be held at ground and the other 2 are the difference between themselves and the low phase. Each of the two non-ground phases are symmetric and therefore can use the same table (aka limitedSinTable).

Amplitude adjusts the output so the motor doesn't go full force. It uses an approximate division by 255.

Currently limit amplitude to 30 or 40 (of 255) because back-EMF may blow up the FETs and/or FET drivers. More research is necessary with the 

#### Controller<a name="controller"></a>

code [here](../ThreePhaseController.h)



#### Servo<a name="servo"></a>



------ 

### <a name="sense">Sensing</a>

In order to know How to push the motor we must know where it is in relation to the phase. We use a [Magnetometer](#mlx) to determine the location at low speeds and high resosultion and the [Back EMF](#emf) at high speeds.

Since reading occur with some lag and the reading is much slower then uses we must estimate the current location when using the data. This prediction happens in the [Predictor](#predictor).

#### <a name="mlx">Magnetometer</a>

using the [MLX90363](https://www.melexis.com/-/media/files/documents/datasheets/mlx90363-datasheet-melexis.pdf) - SOIC8 package - and communicating with it over SPI.



#### <a name="emf">Back EMF</a>

NOT YET IMPLEMENTED

#### <a name="predictor">Predictor</a>


