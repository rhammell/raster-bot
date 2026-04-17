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

float Raster_PID::compute(float measurement, float dt) {
    if (dt <= 0.0f) return 0.0f;

    float error = _setpoint - measurement;

    // Proportional term
    float pTerm = _kp * error;

    // Integral term with anti-windup clamping
    _integral += error * dt;
    _integral = constrain(_integral, _outputMin / (_ki != 0.0f ? _ki : 1.0f),
                                     _outputMax / (_ki != 0.0f ? _ki : 1.0f));
    float iTerm = _ki * _integral;

    // Derivative term (on error)
    float dTerm = _kd * (error - _prevError) / dt;
    _prevError = error;

    // Sum and clamp output
    float output = pTerm + iTerm + dTerm;
    output = constrain(output, _outputMin, _outputMax);

    return output;
}

void Raster_PID::reset() {
    _integral  = 0.0f;
    _prevError = 0.0f;
}
