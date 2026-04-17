#pragma once

#include <Arduino.h>
#include "config.h"
#include "Raster_Motor_Controller.h"

// Differential drive controller coordinating left and right motor controllers.
class Raster_Drive {
public:
    Raster_Drive() = default;

    bool begin();
    void straight(float speed_cm_s);
    void straight(float speed_cm_s, float distance_cm);
    void stop();
    void update();
    bool isMoving() const;

    Raster_Motor_Controller _leftController;
    Raster_Motor_Controller _rightController;

private:
    void _startStraight(float speed_cm_s);
    void _setRPM(float leftRPM, float rightRPM);
    float _ticksToCm(int64_t ticks) const;

    bool  _moving = false;
    bool  _distanceBounded = false;
    float _cruiseSpeed = 0;
    float _targetDistance = 0;
};
