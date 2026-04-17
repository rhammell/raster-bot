#include "Raster_Motor_Controller.h"

// =============================================================================
// Raster_Motor_Controller Implementation
// =============================================================================

bool Raster_Motor_Controller::begin(const MotorControllerConfig& config) {
    // Initialize H-bridge PWM outputs (motorFlip swaps pins so +PWM = forward)
    motor.begin(config.in1Pin, config.in2Pin, config.motorFlip);

    // Initialize quadrature encoder via PCNT hardware (encFlip swaps pins so +PWM = +count)
    if (!encoder.begin(config.encAPin, config.encBPin, config.encFlip)) {
        return false;
    }

    // Initialize PID speed controller
    pid.begin(config.kp, config.ki, config.kd, config.pidOutputMin, config.pidOutputMax);

    return true;
}

void Raster_Motor_Controller::setRPM(float rpm) {
    pid.setSetpoint(rpm);
}

void Raster_Motor_Controller::update() {
    // Get the current time
    uint32_t now = micros();
    uint32_t elapsed = now - _lastUpdateTime;

    // Skip if the minimum interval hasn't passed
    if (elapsed < PID_UPDATE_INTERVAL_US) return;

    // Update the last update time
    _lastUpdateTime = now;

    // Compute the time delta in seconds
    float dt = elapsed / 1e6f;
    
    // Get the current encoder count and the delta
    int64_t count = encoder.getCount();
    int64_t delta = count - _lastEncoderCount;

    // Update the last encoder count
    _lastEncoderCount = count;

    // Compute the revolutions
    float revolutions = (float)delta / ENCODER_TICKS_PER_REV;

    // Compute the current RPM
    currentRPM = (revolutions / dt) * 60.0f;

    // Run PID and apply output to motor
    currentPWM = pid.compute(currentRPM, dt);
    motor.setPWM((int)currentPWM);
}

void Raster_Motor_Controller::stop() {
    setRPM(0);
    pid.reset();
}

void Raster_Motor_Controller::reset() {
    pid.reset();
    encoder.clearCount();
    _lastEncoderCount = 0;
}
