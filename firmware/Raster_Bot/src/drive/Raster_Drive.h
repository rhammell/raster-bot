#pragma once

#include <Arduino.h>
#include "../definitions/pin_definitions.h"
#include "../definitions/robot_definitions.h"
#include "Raster_Motor_Controller.h"

struct DriveTelemetry {
    float leftRPM;
    float leftPWM;
    float rightRPM;
    float rightPWM;
    int64_t leftCount;
    int64_t rightCount;
};

// Differential drive controller coordinating left and right motor controllers.
class Raster_Drive {
public:
    // Constructor
    Raster_Drive() = default;

    // Public methods
    bool begin();
    void straight(float speed_cm_s);
    void spin(float speed_cm_s);
    void stop();
    void update();
    bool isMoving() const;
    DriveTelemetry getTelemetry() const;

    // Cumulative straight-line distance, in cm, driven since power-on. Grows
    // for both forward and backward motion; pure in-place spins do not add to it.
    float getDistance() const;
    void resetDistance();

private:
    // Drive motion state: Idle (stopped), Straight (wheels together, translate),
    // or Spin (wheels equal-and-opposite, rotate in place).
    enum class DriveMode { Idle, Straight, Spin };

    // Shared start-of-move logic for straight() and spin(). mode selects
    // whether the wheels run together (translate) or opposite (rotate).
    void startMove(float speed_cm_s, DriveMode mode);

    // Private members
    Raster_Motor_Controller _leftController;
    Raster_Motor_Controller _rightController;
    DriveMode _mode = DriveMode::Idle;
    float    _targetRPM = 0;
    float    _commandedRPM = 0;   // ramped value actually sent to the wheels
    uint32_t _lastUpdateTime = 0;

    // Odometry: accumulated centerline distance plus the encoder counts from
    // the previous update() used to compute each tick's delta.
    float    _distanceCm = 0.0f;
    int64_t  _lastLeftCount = 0;
    int64_t  _lastRightCount = 0;
};
