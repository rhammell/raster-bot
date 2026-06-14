#include "Raster_Motor.h"

// =============================================================================
// Raster_Motor Implementation
// =============================================================================

void Raster_Motor::begin(uint8_t in1Pin, uint8_t in2Pin, bool flip) {
    // Set the motor pins
    if (flip) {
        _in1Pin = in2Pin;
        _in2Pin = in1Pin;
    } else {
        _in1Pin = in1Pin;
        _in2Pin = in2Pin;
    }

    // Set the motor pins to output
    pinMode(_in1Pin, OUTPUT);
    pinMode(_in2Pin, OUTPUT);

    // Stop the motor
    stop();
}

void Raster_Motor::setPWM(int pwm) {
    // Constrain the PWM value to the valid range
    pwm = constrain(pwm, -255, 255);

    // Set the motor pins to the appropriate state
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
    // Stop the motor
    analogWrite(_in1Pin, 0);
    analogWrite(_in2Pin, 0);
}
