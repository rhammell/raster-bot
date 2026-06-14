#pragma once

#include <Arduino.h>
#include "Raster_Motor.h"
#include "Raster_Encoder.h"
#include "Raster_PID.h"

// Bundles all configuration needed to initialize a motor controller.
struct MotorControllerConfig {
    uint8_t in1Pin, in2Pin;
    bool    motorFlip;
    uint8_t encAPin, encBPin;
    bool    encFlip;
    float   kp, ki, kd;
    float   pidOutputMin, pidOutputMax;
};

// Coordinates a single motor/encoder pair with PID speed control.
class Raster_Motor_Controller {
public:
    // Constructor
    Raster_Motor_Controller() = default;

    // Prevent copies (encoder owns a unique PCNT unit)
    Raster_Motor_Controller(const Raster_Motor_Controller&) = delete;
    Raster_Motor_Controller& operator=(const Raster_Motor_Controller&) = delete;

    // Public methods
    bool begin(const MotorControllerConfig& config);
    void setRPM(float rpm);
    void update(float dt);
    void stop();   // Stops the motor; does NOT clear PID/encoder state - call reset() before reusing
    void reset();
    int64_t getEncoderCount() const { return encoder.getCount(); }

    // Public members
    Raster_Motor   motor;
    Raster_Encoder encoder;
    Raster_PID     pid;
    float          currentRPM = 0.0f;
    float          currentPWM = 0.0f;

private:
    // Private members
    int64_t  _lastEncoderCount = 0;
};
