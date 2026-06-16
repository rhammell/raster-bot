#include "Raster_Drive.h"

// =============================================================================
// Raster_Drive Implementation
// =============================================================================

bool Raster_Drive::begin() {
    // Enable DRV8833 motor driver by pulling nSLEEP high
    Serial.println("[Drive] Enabling DRV8833...");
    pinMode(DRV_SLEEP, OUTPUT);
    digitalWrite(DRV_SLEEP, HIGH);

    // Initialize left motor controller (motor, encoder, PID)
    Serial.println("[Drive] Initializing left motor controller...");
    MotorControllerConfig leftConfig = {
        MOT_LEFT_IN1, MOT_LEFT_IN2, MOT_LEFT_FLIP,
        MOT_LEFT_ENC_A, MOT_LEFT_ENC_B, MOT_LEFT_ENC_FLIP,
        PID_KP, PID_KI, PID_KD, PID_OUTPUT_MIN, PID_OUTPUT_MAX
    };
    if (!_leftController.begin(leftConfig)) {
        Serial.println("[Drive] Left motor controller init FAILED");
        return false;
    }
    Serial.println("[Drive] Left motor controller init OK");

    // Initialize right motor controller (motor, encoder, PID)
    Serial.println("[Drive] Initializing right motor controller...");
    MotorControllerConfig rightConfig = {
        MOT_RIGHT_IN1, MOT_RIGHT_IN2, MOT_RIGHT_FLIP,
        MOT_RIGHT_ENC_A, MOT_RIGHT_ENC_B, MOT_RIGHT_ENC_FLIP,
        PID_KP, PID_KI, PID_KD, PID_OUTPUT_MIN, PID_OUTPUT_MAX
    };
    if (!_rightController.begin(rightConfig)) {
        Serial.println("[Drive] Right motor controller init FAILED");
        return false;
    }
    Serial.println("[Drive] Right motor controller init OK");

    return true;
}

void Raster_Drive::straight(float speed_cm_s) {
    // Constrain the speed to the maximum speed
    speed_cm_s = constrain(speed_cm_s, -MAX_SPEED_CM_S, MAX_SPEED_CM_S);

    // Convert the speed to target RPM
    _targetRPM = (speed_cm_s * 60.0f) / WHEEL_CIRCUMFERENCE_CM;

    // If starting from a stop, reset controllers (PID state and encoder
    // counts) so the straightness correction starts from zero
    if (!_moving) {
        _leftController.reset();
        _rightController.reset();
        _lastUpdateTime = micros(); // see if ths is necessar
        _moving = true;
    }
}

void Raster_Drive::stop() {
    // Immediately stop both motors and go idle
    _targetRPM = 0;
    _moving = false;
    _leftController.stop();
    _rightController.stop();
}

void Raster_Drive::update() {
    // Nothing to do while idle
    if (!_moving) return;

    // Get the current time and the elapsed time
    uint32_t now = micros();
    uint32_t elapsed = now - _lastUpdateTime;

    // Skip if the minimum interval hasn't passed
    if (elapsed < DRIVE_UPDATE_INTERVAL_US) return;

    // Update the last update time
    _lastUpdateTime = now;

    // Convert the elapsed time to seconds
    float dt = elapsed / 1e6f;

    // Compute a proportional correction from the encoder tick difference
    // between left and right wheels to keep the robot driving straight.
    // With STRAIGHT_KP = 0 the correction is zero and both wheels run at
    // the raw target RPM (used to isolate the PID during tuning).
    float tickError = (float)(_leftController.getEncoderCount()
                            - _rightController.getEncoderCount());
    float correction = STRAIGHT_KP * tickError;
    _leftController.setRPM(_targetRPM - correction);
    _rightController.setRPM(_targetRPM + correction);

    // Run the PID controllers to compute and apply PWM outputs
    _leftController.update(dt);
    _rightController.update(dt);
}

bool Raster_Drive::isMoving() const {
    // Return true if the drive is moving, false otherwise
    return _moving;
}

DriveStatus Raster_Drive::getStatus() const {
    // Return the current status of the drive
    return {
        _leftController.getRPM(),
        _leftController.getPWM(),
        _rightController.getRPM(),
        _rightController.getPWM(),
        _leftController.getEncoderCount(),
        _rightController.getEncoderCount()
    };
}

