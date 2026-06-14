#pragma once

#include <Arduino.h>

// Controls a single motor channel on the DRV8833 H-bridge.
// PWM sign controls direction, magnitude controls duty cycle.
class Raster_Motor {
public:
    Raster_Motor() = default;

    void begin(uint8_t in1Pin, uint8_t in2Pin, bool flip = false);
    void setPWM(int pwm);   // -255 to +255 (sign = direction)
    void stop();

private:
    uint8_t _in1Pin = 0;
    uint8_t _in2Pin = 0;
};
