/*
 * robot_definitions.h - Robot tuning and behavioral constants
 *
 * Constants defined in this file describe how the robot behaves
 * rather than how it is wired: physical geometry, control gains,
 * calibration factors, and default settings. These values are
 * intrinsic to the robot and its algorithms, and are shared by
 * the subsystems that need them.
 */

#pragma once

#include <math.h>

// Battery voltage divider ratio (Vbat = Vadc * ratio)
#define VBAT_DIV_RATIO      2.0f

// Wheel geometry
#define WHEEL_DIAMETER_CM       4.05
#define WHEEL_CIRCUMFERENCE_CM  (M_PI * WHEEL_DIAMETER_CM)
#define TICKS_PER_CM            (ENCODER_TICKS_PER_REV / WHEEL_CIRCUMFERENCE_CM)

// Drive speed limits
#define MAX_SPEED_CM_S          10.0f

// Straightness correction (tuned with tests/StraightnessTuning)
#define STRAIGHT_KP             0.0f

// Acceleration and deceleration (tuned with tests/DistanceTuning)
#define MAX_ACCEL_RPM_S         30.0f
#define DECEL_DISTANCE_CM       3.0f
#define MIN_SPEED_CM_S          0.5f

// Motor gear ratio
#define MOTOR_GEAR_RATIO 100.37   

// Motor encoder settings
// ENCODER_MODE: 4 for X4 quadrature (both edges, both channels)
//               2 for X2 quadrature (both edges, channel A only)
#define ENCODER_MODE 4
#define ENCODER_PPR 3             // Pulses per revolution (pole pairs on the magnetic disc)
#define ENCODER_TICKS_PER_REV (ENCODER_PPR * ENCODER_MODE * MOTOR_GEAR_RATIO)

// PID speed controller gains (tuned with tests/PIDTuning)
#define PID_KP          6.0f
#define PID_KI          2.0f
#define PID_KD          0.0f    // Not used; derivative amplifies encoder quantization noise

// PID output limits and loop rate
#define PID_OUTPUT_MIN  -255.0f
#define PID_OUTPUT_MAX   255.0f
#define DRIVE_UPDATE_INTERVAL_US  20000  // 20ms = 50Hz

// Display and touchscreen settings
#define TFT_DEFAULT_ROTATION   0
#define TFT_DEFAULT_BG_COLOR   ILI9341_BLACK
#define TFT_DEFAULT_BRIGHTNESS 255
#define TOUCH_THRESHOLD 40
