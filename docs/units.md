# Three Phase Motor controller units

The 3 phase motor controller has several different units in many different places

## Position<a name="pos"></a>

#### Mechanical vs Electrical Revolution<a name="revunit"></a>

Electrical Revolution is defined as a step driven by one full sin. 
Mechanical Revolution is defined as one revolution of the motor. 
3 phase motors are wound in such a way that there are multiple elctrical (in the case of [Turnigy 480](http://www.hobbyking.com/hobbyking/store/__19038__Turnigy_Park480_Brushless_Outrunner_1320kv.html) 7) revolutions per mechanical ones.

### Magnetometer<a name="mlx-steps"></a>

Has 14 bits of resolution (16384 steps) per Mecanical Revolution.

### Phase<a name="phase"></a>

- Each phase is one byte (256 steps).
- 3 phases per [electrical revolution](#revunit).
- 5376 phase steps per [mechanical revolution](#revunit).

### Velocity<a name="velocity"></a>

Velocity is measured in units of [phase position](#phase) change during one [pwm cycle](#pwm). This is too minimize calculations per pwm cycle.

## Time<a name="time"></a>

### Intruction cycle<a name="clock"></a>

16 MHz clock speed

### PWM<a name="pwm"></a>

31.25 kHz pwm update rate. 512 clock cycles per pwm cycle.

### Magnetometer<a name="mlx-time"></a>

800 Hz update rate (31.25 kHz / 40 (ThreePhaseController::cyclesPWMPerMLX))

