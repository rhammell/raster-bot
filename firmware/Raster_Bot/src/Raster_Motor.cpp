#include "Raster_Motor.h"

// =============================================================================
// Raster_Motor Implementation
// =============================================================================

void Raster_Motor::begin(uint8_t in1Pin, uint8_t in2Pin, bool flip) {
    if (flip) {
        _in1Pin = in2Pin;
        _in2Pin = in1Pin;
    } else {
        _in1Pin = in1Pin;
        _in2Pin = in2Pin;
    }

    pinMode(_in1Pin, OUTPUT);
    pinMode(_in2Pin, OUTPUT);

    stop();
}

void Raster_Motor::setPWM(int pwm) {
    pwm = constrain(pwm, -255, 255);

    if (pwm > 0) {
        analogWrite(_in1Pin, pwm);
        analogWrite(_in2Pin, 0);
    } else if (pwm < 0) {
        analogWrite(_in1Pin, 0);
        analogWrite(_in2Pin, -pwm);
    } else {
        stop();
    }
}

void Raster_Motor::stop() {
    analogWrite(_in1Pin, 0);
    analogWrite(_in2Pin, 0);
}
