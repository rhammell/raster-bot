#pragma once

#include <Arduino.h>
#include "../definitions/pin_definitions.h"
#include "../definitions/robot_definitions.h"
#include "Raster_Motor_Controller.h"

struct DriveStatus {
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
    void stop();
    void update();
    bool isMoving() const;
    DriveStatus getStatus() const;

private:
    // Private members
    Raster_Motor_Controller _leftController;
    Raster_Motor_Controller _rightController;
    bool     _moving = false;
    float    _targetRPM = 0;
    float    _commandedRPM = 0;   // ramped value actually sent to the wheels
    uint32_t _lastUpdateTime = 0;
};
