# Three phase motor controller

The following is the documentation for the three phase motor controller, an open source project.
Our current use case is on the [3 Phase Driver Board](https://github.com/cinderblock/3-Phase-Driver) with the [Quanum MT5206](https://hobbyking.com/en_us/quanum-mt-series-5206-320kv-brushless-multirotor-motor-built-by-dys.html). 

## General System

[Software Map](https://drive.draw.io/?lightbox=1&highlight=0000ff&edit=_blank&layers=1&nav=1&title=Three%20Phase%20Control#R7V1bc6M6Ev41rjr7MC7uhsckc%2FGpmjmVmmRn9m1LAcWwgxElcJzMr18JhAFJXJwgO5OQmpoYuS1w99efupsWWZhX28cvGKThNxTAeGFowePC%2FLgwDF3XbfKLjjyVI7ZnlAMbHAVMqB64iX5DNqix0V0UwKwlmCMU51HaHvRRkkA%2Fb40BjNG%2BLXaP4vZZU7CBwsCND2Jx9GcU5GE56tpaPb6G0Saszqxr7J074P%2FaYLRL2PkWhnlf%2FJRvb0E1F5PPQhCgfWPI%2FLQwrzBCeflq%2B3gFY6rbSm3l5z53vHu4bgyTfMwHLM3X71YutFfQ0MGd98EsZ3gA8Y7p4uJ2CzfANHYWu%2BT8qVJTto%2B2MUjI0WUQYWKICCXkHQgycvLLEOHoN0pyEJMxjQywqSHO4WPn9eoHLRB0QbSFOX4iIo%2FV9ZafYLjSTXa8r620cthY2LDQqhIEDBmbw9S1dsgLpqCRynJsQSUwIEBihwjnIdqgBMSf6tHLAh0wYDoJ8y1Vj05eZjnA%2BQVFLxlIUKHWYuxzFFcqhElQSfgxyLLILweZiF4eMYdyyNH%2FYJ4%2FsWOwyxEZqi%2FrK0Lp4eQY%2FTqAXT9Yi36fTlt1oSdDO%2BzDDpnKnwHewK55vA4UYBiDPHpoX5HMpMVHiabAU0MgRVGSZ42Zr%2BlADS5ba6PL1Djn4eQtp1eevCivoAbX4as8E2%2BO4J2m%2Fmv9W0AhcbC8DS4Ms%2Bg3uCsEKJCYLoi0fbmwP5IREEcb6r0%2BMS%2FEZIA6akQ48YK9sY2CoEBwDO5gfHlguisUI1yct%2BK6HldnjM2upCbCXliVftZJCtpStwyvZYoP1Qp0HGIEEx%2FmYfMaPI%2Bg%2B%2FuMwJhnkRcaWhdZ%2BMd3MuBc%2F%2FxGfq0BDvZEYYLda26hNt%2BHUQ5vUlC44p6s1G1IjKJjuYt38%2FOq7RKOKxK0LiNoJfxsiHr8m4Ib79KSB56vvHuysH0G2yimX3QN4wdIfYW9wfhWN9hxw0O04kfmbiUDy0TvCbkf5WQvMKCpjTCgdioDVpM2DHhz%2FfdsObnlhg1nnMxwYhx5G2IIr0OQwY%2BYEDJe%2BmkqWLIRUb4GOylmS5sLZ01dYjNbZjNLSTgr0uXCcGIaSWQpSMjrDX29MC%2FIPxA8gMSHt%2Biv5XL5r0qOnLMlOklgQsLWjNm7YdYY3ueSKCWnIS29Xj9KNrdFfPvBer79BgzYDg6ccVRpKDHeSlD2nIsM5SI2E%2BpNRlYrOQbUJiMmF0nZen8yMiA%2FfTJiixS%2FBnH8E%2BR%2BOLO7LJSSsbuMH2wl%2FCCaq5vciTPc5CCHf71ZYrePInZbMNzqVLy%2B0mdeP57XnTG8bp2D13UuTredfl4fkFfA62KR6dvX%2F3ia6ZgzqfNZli3NsmyRGzwl3CCmxxVfB9FDxdVrkNC6naHdYpBkoKzS17TekPxB9B8Q2ieyV9%2Bv%2FnyOd47geMmdhNOly447k%2FzRJF%2B5Xi%2FJd2HghXVhvSpwMPRYLgeL8srZp2pkHH9Lok03A3ck%2BsUV3JDwBAJiNyQ%2B%2FzMNfbzimxJuL7toS9OsQPJSsLWt%2BsFdtWdQcw%2FCXgm2rUt414TU6TryKcujLcjRnO8JoYGsmGfZktBATb7nzvUgOZ5715SVNWJNcc%2BSOBxYqep9sAYKQlqv%2FPSLQaW7BmGERej5D9xXfPEdgoCEdrSu8Ma5wu6oGlbWaUefhyp%2FgywMWW2IjzOmySPElE9WGyIu%2BWtHWF67iNMQUC9B9CDZ0Oyio0xUDt9hfqQp9e%2BUJR3fd0lC0EFesXUFZkdM%2FIdnK13liJ785GQ30l1nXkyesZisXu1iYnLBSlVl6lxM%2BuUVLCZi9El0eJEE1xj5MMvImkLvNhyq1O97NRH6Is%2B7nLiC7b7Q%2FESjFiO%2FCqNJClVXKKF66VpYGpKSD49cRFqT%2FOkrRgcojlwxlPQSVMBqYIB1DMzeyuXzlsRX9VP5qitpNSztRAbp0kwNBWnQFyX0a7M4vuFdfb5YFwzIQdn280Z90dXkJu%2F2ReNk1WVXXXW5js2aAZz%2BRgI4d1Q1oMPyiqsB3Iq%2F6o%2Ff%2BsWnD99csRbQWI276V9NdMSVxR3JwuhJnNFR4oy2oJnrA6kKTPoDxsiP8qe3ypodyY98oZS0YZwsA7bFmLZe3EjgSkgpjue2p4VQBj%2FcRG1azZKYTVfibo4Yh0rcTFaVut5lISWrQ9xSRkBlTFRvdyMj9L08LAIm8gsfXQt7Y05td9wY6%2FZiQ4YHJd1Unhju%2BqXzfgX7%2FsxkWmVUU3BdCJK83ZQ6i6aE5MTbuTcQP6CZ37r5TZK9OScLJhzjuexW2JV8ku7dXlBXc8CWkkD5P03dypj3vXOZdyyXHfr1lHOZ6wnqfd9F%2BTFNQ9UC0JvSde36ULz9mNtObKiusVc0NdcFFGDoPGUBj4OQ8k0gkvWHp3wcgaKSP7gzs%2FkMCWIAascpnhlhczGWK%2BnZPR1pCwr7JmgsACTzqDxMCIiaLheClH5m%2B7ihz2FZplGwhMkmSmC2hDHRJY78%2F25RTkOkZgRm2Wo0643d4aImfNVF5a7F3RNtAptij%2FzAI0tWnJIkCfHJ4KeLEf5adNmzq0iWBp1KRYZYMVibr09F5ySxyu3fa%2BRZkUrvY27cEUFCl9Ff2jDM87KhLTW78dOesKNXXZxWZLK%2Bactv%2FuIWeIvre%2FRUh8XGO292GoVu74zo5nonCSAmQbd71LQTodvkInZP9fY%2B4533hY9Bd8U4Z0G3xcHQmgbd3lHTToVurqThearRLcmv2ObVl4VwrYcZqkxW%2BcZwTZIuuJJAT0kXl6GuxPinVIhG8cWYWM%2BcpCAkVny4J9Q5%2BiR84XGxns1n7BMxBL%2B9XddUb1OxDIEiBJCfv2bFEedZa1aW7NEhr05j3AJ3Xo3NCfJwCjGCM201IRZ7ZKhrr8r%2F23u5hWcFj%2BVMi88gFHEmHyO4A48QGZBXQLFzBv2K4W9zGYbL7z8ei3dhIv4hJ1PhnT%2FPEN775RXgfc6px%2BF9eixzT71z%2BTRoNJb5ifjdFlNhmT%2FPEJb75RVg%2BZ0%2FiuaMWHanwjI%2FkSos8%2BcZwnK%2FvAIsv%2FMOqaniEGuSfigJt3WkbMeTd8ct9okB72jHBSID8gracdR1dM2An4LhBwLm8Qwv3KtSA3j9SMD3yytoYRSLeT6IY4Jm8iXzwmKbXQwwnYP2TxE5SYP0F0iVoMV0dxr9UGOXK%2BR2uQ7uQ%2F9Iu6pBmmKU4qjcBZuyDbCg2lg7cqortN2ChH4V%2Ftn5b6Pbur%2BgZ1QdFNUWWpPtKWw2l8nuhDyj054c1n9zq8Rl%2FYfNzE%2F%2FBw%3D%3D)

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
