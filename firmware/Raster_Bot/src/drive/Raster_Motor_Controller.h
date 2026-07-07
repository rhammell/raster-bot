#pragma once

#include <Arduino.h>
#include "Raster_Motor.h"
#include "Raster_Encoder.h"
#include "Raster_PID.h"

// Per-side wiring for a motor controller. Behavioral tuning (PID gains,
// encoder mode) is read from robot_definitions.h inside begin(), not passed here.
struct MotorControllerConfig {
    uint8_t in1Pin, in2Pin;
    bool    motorFlip;
    uint8_t encAPin, encBPin;
    bool    encFlip;
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

    // Feedforward characterization only (see tuning/1_FeedforwardTuning): refresh
    // the RPM measurement as usual but drive a fixed open-loop PWM, bypassing
    // the PID and feedforward entirely. Lets the steady RPM sustained by each
    // PWM level be logged to build the PID_FF table. Not used in normal driving.
    void updateOpenLoop(float dt, int pwm);

    // Telemetry accessors
    float   getRPM() const { return _currentRPM; }
    float   getPWM() const { return _currentPWM; }
    int64_t getEncoderCount() const { return encoder.getCount(); }

private:
    // Read the encoder delta and update the filtered _currentRPM for this tick.
    // Shared by update() (closed loop) and updateOpenLoop() (characterization).
    void measureRPM(float dt);

    // Open-loop feedforward PWM required to hold the given wheel speed, from the
    // measured PID_FF lookup table (linearly interpolated, clamped at the ends,
    // signed by rpm). Returns 0 when feedforward is disabled or rpm is 0.
    float feedforward(float rpm) const;

    // Private members
    Raster_Motor   motor;
    Raster_Encoder encoder;
    Raster_PID     pid;
    float          _targetRPM = 0.0f;
    float          _currentRPM = 0.0f;
    float          _currentPWM = 0.0f;
    int64_t        _lastEncoderCount = 0;
};
