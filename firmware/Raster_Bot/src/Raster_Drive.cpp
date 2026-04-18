#include "Raster_Drive.h"

// =============================================================================
// Raster_Drive Implementation
// =============================================================================

bool Raster_Drive::begin() {
    // Enable DRV8833 motor driver
    pinMode(DRV_SLEEP, OUTPUT);
    digitalWrite(DRV_SLEEP, HIGH);

    // Initialize left motor controller
    MotorControllerConfig leftConfig = {
        MOT_LEFT_IN1, MOT_LEFT_IN2, MOT_LEFT_FLIP,
        MOT_LEFT_ENC_A, MOT_LEFT_ENC_B, MOT_LEFT_ENC_FLIP,
        PID_KP, PID_KI, PID_KD, PID_OUTPUT_MIN, PID_OUTPUT_MAX
    };
    if (!_leftController.begin(leftConfig)) {
        return false;
    }

    // Initialize right motor controller
    MotorControllerConfig rightConfig = {
        MOT_RIGHT_IN1, MOT_RIGHT_IN2, MOT_RIGHT_FLIP,
        MOT_RIGHT_ENC_A, MOT_RIGHT_ENC_B, MOT_RIGHT_ENC_FLIP,
        PID_KP, PID_KI, PID_KD, PID_OUTPUT_MIN, PID_OUTPUT_MAX
    };
    if (!_rightController.begin(rightConfig)) {
        return false;
    }

    return true;
}

void Raster_Drive::straight(float speed_cm_s) {
    // Set the distance bounded flag to false
    _distanceBounded = false;
    _targetDistance = 0;

   // Start the straight movement
    _startStraight(speed_cm_s);
}

void Raster_Drive::straight(float speed_cm_s, float distance_cm) {
    // Set the distance bounded flag to true
    _distanceBounded = true;
    _targetDistance = distance_cm;

    // Start the straight movement
    _startStraight(speed_cm_s);
}

void Raster_Drive::_startStraight(float speed_cm_s) {
    // Constrain the speed to the maximum speed 
    speed_cm_s = constrain(speed_cm_s, -MAX_SPEED_CM_S, MAX_SPEED_CM_S);
    _cruiseSpeed = speed_cm_s;

    // Convert the speed to RPM
    float rpm = (_cruiseSpeed * 60.0f) / WHEEL_CIRCUMFERENCE_CM;

    // Set the RPM for the left and right motors
    _setRPM(rpm, rpm);

    // Set the moving flag to true
    _moving = true;
}

void Raster_Drive::stop() {
    _setRPM(0, 0);
    _moving = false;
    _distanceBounded = false;
}

void Raster_Drive::update() {
    // Get the current time and the elapsed time
    uint32_t now = micros();
    uint32_t elapsed = now - _lastUpdateTime;

    // Skip if the minimum interval hasn't passed
    if (elapsed < DRIVE_UPDATE_INTERVAL_US) return;
    _lastUpdateTime = now;

    // Convert the elapsed time to seconds
    float dt = elapsed / 1e6f;

    // Update the left and right motor controllers
    _leftController.update(dt);
    _rightController.update(dt);

    // if (_moving && _distanceBounded) {
    //     float traveled = _ticksToCm(
    //         (abs(_leftController.encoder.getCount()) + abs(_rightController.encoder.getCount())) / 2
    //     );
    //     float remaining = _targetDistance - traveled;
    //
    //     if (remaining <= 0) {
    //         stop();
    //         return;
    //     }
    //
    //     if (remaining < DECEL_DISTANCE_CM) {
    //         float scale = remaining / DECEL_DISTANCE_CM;
    //         float speed = _cruiseSpeed > 0
    //             ? max(_cruiseSpeed * scale,  MIN_SPEED_CM_S)
    //             : min(_cruiseSpeed * scale, -MIN_SPEED_CM_S);
    //         float rpm = (speed * 60.0f) / WHEEL_CIRCUMFERENCE_CM;
    //         _leftController.setRPM(rpm);
    //         _rightController.setRPM(rpm);
    //     }
    // }
}

bool Raster_Drive::isMoving() const {
    return _moving;
}

float Raster_Drive::_ticksToCm(int64_t ticks) const {
    return (float)ticks / TICKS_PER_CM;
}

void Raster_Drive::_setRPM(float leftRPM, float rightRPM) {
    // Reset the left and right motor controllers
    _leftController.reset();
    _rightController.reset();

    // Set the RPM for the left and right motors
    _leftController.setRPM(leftRPM);
    _rightController.setRPM(rightRPM);
}
