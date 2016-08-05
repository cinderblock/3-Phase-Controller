# Three Phase Motor controller units

The 3 phase motor controller has serval differnt units in many different places

## Position

#### Mechanical vs Electrical Revolution

Electrical Revolution is defined as a step driven by one full sin
Mechanical Revolution is defined as one revolution of the motor
3 phase motors are wound in such a way that there are multiple elctrical (in this case 7) revolutions per mechanical ones.

### Magnetometer

Has 14 bits of resolution (16384 steps) per Mecanical Revolution.

### Phase

Each phase is one byte (256 steps).
3 phases per electrical revolution.

## Time

### Intruction cycle

16 MHz clock speed

### PWM

32 kHz pwm update rate

### Magnetometer

800 Hz update rate (32 kHz / 40 (ThreePhaseController::cyclesPWMPerMLX))
