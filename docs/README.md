# Three phase motor controller

The following is the documentation for the three phase motor controller, an open source project.
Our current use case is on the [3 Phase Driver Board](https://github.com/cinderblock/3-Phase-Driver) with the [Quanum MT5206](https://hobbyking.com/en_us/quanum-mt-series-5206-320kv-brushless-multirotor-motor-built-by-dys.html). 

## General System

[Software Map](Three Phase Control.html)

The motor controller has:
- [Output](#output)
  - [Servo](#servo)
  - [Controller](#controller)
  - [Driver](#driver)
- [Sensing](#sense)
  - [Magnetometer](#mlx)
  - [Predictor](#predictor)
  - [Back EMF Sensing](#emf)

------

### Output<a name="output"></a>

The servo simulates the 3 sin waves needed to drive a 3-phase motor with a ~32KHz PWM of 3 half-H bridges.


#### Driver<a name="driver"></a>

code [here](../ThreePhaseDriver.h)

The driver handles the sin wave generation based on a `[0-767]` integer. 
It also uses an amplitude `[0-255]`.

Based on the math of [3 Phase sine generation](https://docs.google.com/spreadsheets/d/1I45kGhncSQvR4_B_AG72Bqk7MJlNRIvBI-JD9qAgE8U/edit?usp=sharing), one output can be held at ground and the other 2 are the difference between themselves and the low phase. Each of the two non-ground phases are symmetric and therefore can use the same table (aka limitedSinTable).

Amplitude adjusts the output so the motor doesn't go full force. It uses an approximate division by 255.

Currently limit amplitude to 30 or 40 (of 255) because inductive voltage spikes caused by the motor **may blow up the FETs and/or FET drivers**. More research is necessary to understand why and how to prevent such exposive events.

#### Controller<a name="controller"></a>

code [here](../ThreePhaseController.h)

The 3-phase controller handles injesting [predictor](#predictor) data to understand current position and push at a 90&deg; ([electrical rotational](units.md#revunit)) angle to current position.

Controller passes along amplitude command to driver.

#### Servo<a name="servo"></a>

code [here](../ServoController.h)

Recieves the input commands for amplitude, velocity, or positional commands.
- Amplitude passes values along to controller
- Velocity command has a deadband and a step change in amplitude if outside of the deadband
- Positional command uses a PID controller. Sum of P, I, and D terms.
  - P value (default: ?) multipled by difference between desired and current position in [phase units](units.md#phase).
  - I (not implemented)
  - D value (default: ?) multiplied by velocity in [velocity units](units.md#velocity).
- Distance command resets the rotation counter and sets a position command of current position + distance

------ 

### Sensing<a name="sense"></a>

For driving the motor we need to know the current angular position. We use a [Magnetometer](#mlx) to determine the location at low speeds and with high resosultion. It is possible to use the [Back EMF sensing](#emf) at high speeds.

Since reading occur with some lag and the reading is much slower then uses we must estimate the current location when using the data. This prediction happens in the [Predictor](#predictor).

#### Magnetometer<a name="mlx"></a>

Using [MLX90363](https://www.melexis.com/-/media/files/documents/datasheets/mlx90363-datasheet-melexis.pdf) - SOIC8 package - communicating over SPI.

This device allows for 14 bit precision and is updated at a rate of ~1 kHz. Reads from the device happen at ~800 Hz.

Maximum revolutions per second of [Turnigy 480 - 1320kv](http://www.hobbyking.com/hobbyking/store/__19038__Turnigy_Park480_Brushless_Outrunner_1320kv.html) is 262. Given 800 Hz read speed this allows for more then 2 readings per mechanical revolution. This is sufficient to determine positions and velocity at all drivable velocities for [Turnigy 480 - 1320kv](http://www.hobbyking.com/hobbyking/store/__19038__Turnigy_Park480_Brushless_Outrunner_1320kv.html). However, 800 Hz update rate is insufficant for use as exact position in [pwm cycle](units.md#pwm), therefore we must use a [predictor](#predictor).

#### Back EMF Sensing<a name="emf"></a>

NOT YET IMPLEMENTED

#### Predictor<a name="predictor"></a>

[Magnetometer](#mlx) has sensor lag and slow reads compared to [pwm cycles](units.md#pwm). Therefore, we must attempt to predict current position and velocity when the [controller](#controller) requests it.

Predictor estimates velocity by having a current estimate (initially 0). Every [Magnetometer](#mlx) reading, if the estimate was incorrect shift the estimate by a step (currently 5 [velocity units](units.md#velocity)) in the correct direction. 
