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
    // Both wheels drive the same direction (translate)
    startMove(speed_cm_s, false);
}

void Raster_Drive::spin(float speed_cm_s) {
    // Wheels drive equal-and-opposite so the robot rotates in place about its
    // center. speed_cm_s is the tangential (rim) speed of each wheel; positive
    // spins clockwise (left wheel forward, right wheel backward).
    startMove(speed_cm_s, true);
}

void Raster_Drive::startMove(float speed_cm_s, bool spinning) {
    // Constrain the speed to the maximum speed
    speed_cm_s = constrain(speed_cm_s, -MAX_SPEED_CM_S, MAX_SPEED_CM_S);

    // Convert the speed to target RPM (signed). For a straight this is the
    // forward wheel speed; for a spin it is each wheel's tangential speed.
    _targetRPM = (speed_cm_s * 60.0f) / WHEEL_CIRCUMFERENCE_CM;

    // Selects the wheel pattern applied in update()
    _spinning = spinning;

    // If starting from a stop, reset controllers (PID state and encoder
    // counts) so the balance correction starts from zero
    if (!_moving) {
        _leftController.reset();
        _rightController.reset();
        _commandedRPM = 0;
        // Encoder counts were just cleared, so realign the odometry delta
        // trackers. The cumulative distance is intentionally left untouched.
        _lastLeftCount = 0;
        _lastRightCount = 0;
        _lastUpdateTime = micros();
        _moving = true;
    }
}

void Raster_Drive::stop() {
    // Immediately stop both motors and go idle
    _targetRPM = 0;
    _commandedRPM = 0;
    _moving = false;
    _spinning = false;
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

    // Accumulate odometry from the centerline (average) wheel displacement.
    // Skip spins: their residual doesn't cancel under abs and would drift the
    // total. Always refresh baselines so a spin->straight delta stays correct.
    int64_t leftCount = _leftController.getEncoderCount();
    int64_t rightCount = _rightController.getEncoderCount();
    if (!_spinning) {
        float centerTicks = ((leftCount - _lastLeftCount)
                           + (rightCount - _lastRightCount)) / 2.0f;
        _distanceCm += fabsf(centerTicks) / TICKS_PER_CM;
    }
    _lastLeftCount = leftCount;
    _lastRightCount = rightCount;

    // Ramp the commanded RPM toward the target at the acceleration limit.
    // MAX_ACCEL_RPM_S <= 0 disables ramping (snap straight to target).
    float maxDelta = MAX_ACCEL_RPM_S * dt;
    if (maxDelta <= 0.0f) {
        _commandedRPM = _targetRPM;
    } else {
        _commandedRPM += constrain(_targetRPM - _commandedRPM, -maxDelta, maxDelta);
    }

    // Proportional wheel-sync correction, applied on top of the ramped command
    // (not slewed by the ramp). WHEEL_SYNC_KP = 0 disables it. Reuses the counts
    // read above for odometry.
    if (_spinning) {
        // Spin: wheels run opposite, so counts should sum to 0 (+ = clockwise).
        float tickError = (float)(leftCount + rightCount);
        float correction = WHEEL_SYNC_KP * tickError;
        _leftController.setRPM(_commandedRPM - correction);
        _rightController.setRPM(-_commandedRPM - correction);
    } else {
        // Straight: wheels run together, so counts should stay matched (diff 0).
        float tickError = (float)(leftCount - rightCount);
        float correction = WHEEL_SYNC_KP * tickError;
        _leftController.setRPM(_commandedRPM - correction);
        _rightController.setRPM(_commandedRPM + correction);
    }

    // Run the PID controllers to compute and apply PWM outputs
    _leftController.update(dt);
    _rightController.update(dt);
}

bool Raster_Drive::isMoving() const {
    // Return true if the drive is moving, false otherwise
    return _moving;
}

DriveTelemetry Raster_Drive::getTelemetry() const {
    // Return the current motor telemetry for both sides of the drive
    return {
        _leftController.getRPM(),
        _leftController.getPWM(),
        _rightController.getRPM(),
        _rightController.getPWM(),
        _leftController.getEncoderCount(),
        _rightController.getEncoderCount()
    };
}

float Raster_Drive::getDistance() const {
    // Return the cumulative straight-line distance, in cm, driven since power-on
    return _distanceCm;
}

void Raster_Drive::resetDistance() {
    // Zero the odometer; does not affect motor/encoder state
    _distanceCm = 0.0f;
}

