# 3 Phase Sine Wave Generation

Generating 3 sine waves 120deg out of phase with each other from DC voltages is a non-trivial task.

![3 Phase Sine Waves](3%20Phase%20Sine%20Waves.svg)

The first problem we run into is that it's not possible for BLDC ESC hardware to generate 3 independent sine waves relative to some constant neutral.
Were we to be naive and simply "try it", because of the topology of 3 phase switching MOSFETs, the distinctly digital operation, and to simplify significantly, we would end up generating non-sense currents in the motor.

However, we can very easily generate varying positive average voltage on one channel relative to ground.
Herein lies the main "trick" to generate 3 Phase sine currents to feed into the motor.
If at any point in time one "leg" of the motor is held at ground, the other two drivers can easily create arbitrary average voltages via PWM outputs.
This PWMed voltage, when connected to the large inductance and inertia of the motor, is smoothed into a constant current that correlates to the average voltage.

The first step is therefore to shift the waveforms at every point in time by the current minimum waveform.

![3 Phase Sine Wave Minimums](3%20Phase%20Sine%20Waves%20Min.svg)

This results in one of the 3 legs always being "0" and the other two doing some weird other shape.

![3 Phase Sine Wave Minimums](3%20Phase%20Sine%20Waves%20Shifted.svg)

Now, that looks like a sine wave still!
If we remember our trigonometric identities:

```math
{\displaystyle a\sin x+b\sin(x+\theta )=c\sin(x+\varphi )}
where

{\displaystyle c={\sqrt {a^{2}+b^{2}+2ab\cos \theta }},}
and

\varphi =\operatorname {atan2} \left(b\,\sin \theta ,a+b\cos \theta \right).
```

And

```math
a = b = 1
theta = 2pi / 3
```

The waveform is therefore, after some simplification:

```math
cos(pi/6 - x)
```

Now we take just a 1/6th of a revolution of that function and use that as our main function specifying what average voltage we should be at for any particular rotation angle x.

## Code

All of the functionality described above is captured in the `ThreePhaseDriver.cpp` source file.

The `ThreePhaseDriver` accepts an angle input and configures the hardware to generate the 3 voltages as specified by the above calculations.




