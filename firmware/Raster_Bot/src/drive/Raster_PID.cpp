#include "Raster_PID.h"

// =============================================================================
// Raster_PID Implementation
// =============================================================================

void Raster_PID::begin(float kp, float ki, float kd, float outputMin, float outputMax) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
    _outputMin = outputMin;
    _outputMax = outputMax;
    reset();
}

void Raster_PID::setGains(float kp, float ki, float kd) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

void Raster_PID::setOutputLimits(float min, float max) {
    _outputMin = min;
    _outputMax = max;
}

void Raster_PID::setSetpoint(float setpoint) {
    _setpoint = setpoint;
}

float Raster_PID::compute(float measurement, float dt, float feedforward) {
    // Check if the time delta is valid (i.e. not zero)
    if (dt <= 0.0f) return 0.0f;

    // Calculate the error between the setpoint and the measurement
    float error = _setpoint - measurement;

    // Proportional term
    float pTerm = _kp * error;

    // Integral term with anti-windup clamping. The clamp is taken against the
    // output headroom the feedforward leaves behind, so the integral only trims
    // the residual and cannot wind up into PWM the feedforward already provides.
    _integral += error * dt;
    if (_ki != 0.0f) {
        _integral = constrain(_integral, (_outputMin - feedforward) / _ki,
                                         (_outputMax - feedforward) / _ki);
    }
    float iTerm = _ki * _integral;

    // Derivative term (on error)
    float dTerm = _kd * (error - _prevError) / dt;
    _prevError = error;

    // Sum feedforward with the PID terms and clamp output
    float output = feedforward + pTerm + iTerm + dTerm;
    output = constrain(output, _outputMin, _outputMax);

    return output;
}

void Raster_PID::reset() {
    _integral  = 0.0f;
    _prevError = 0.0f;
}
