#pragma once

#include <math.h>

// TFT Pins
#define TFT_CS    17
#define TFT_DC    15
#define TFT_RST   16
#define TFT_LITE  12
#define TFT_CLK   13
#define TFT_MOSI  14
#define TFT_MISO -1

// Touchscreen pins
#define TOUCH_SDA  21  
#define TOUCH_SCL  18  

// Battery charging  pins (BQ24072 charger)
#define CHG      36
#define PGOOD    35

// Battert voltage monitor
#define VBAT_DIV  10
#define VBAT_DIV_RATIO  2.0f

// Motor driver (DRV8833)
#define DRV_SLEEP 40 

// Left motor (DRV8833 channel A + N20 quadrature encoder)
#define MOT_LEFT_IN1    47//  // PWM A
#define MOT_LEFT_IN2    33  // PWM B
#define MOT_LEFT_ENC_A  38  // Encoder channel A
#define MOT_LEFT_ENC_B  39  // Encoder channel B
#define MOT_LEFT_FLIP       false  // Swap PWM pins so +PWM = forward
#define MOT_LEFT_ENC_FLIP   false  // Swap encoder pins so +PWM = +count

// Right motor (DRV8833 channel B + N20 quadrature encoder)
#define MOT_RIGHT_IN1    33  // PWM A - SWAPPED DUE TO SCHEMATIC ERROR
#define MOT_RIGHT_IN2    34  // PWM B - SWAPPED DUE TO SCHEMATIC ERROR
#define MOT_RIGHT_ENC_A   41  // Encoder channel A
#define MOT_RIGHT_ENC_B   42  // Encoder channel B
#define MOT_RIGHT_FLIP       true   // Swap PWM pins so +PWM = forward
#define MOT_RIGHT_ENC_FLIP   true  // Swap encoder pins so +PWM = +count

// Drive settings
#define WHEEL_DIAMETER_CM       3.0
#define WHEEL_CIRCUMFERENCE_CM  (M_PI * WHEEL_DIAMETER_CM)
#define TICKS_PER_CM            (ENCODER_TICKS_PER_REV / WHEEL_CIRCUMFERENCE_CM)
#define MAX_SPEED_CM_S          10.0f
#define DECEL_DISTANCE_CM       1.0f
#define MIN_SPEED_CM_S          1.0f

// Motor gear ratio
#define MOTOR_GEAR_RATIO 100.0    // Gear ratio of the motor

// Motor encoder settings
// ENCODER_MODE: 4 for X4 quadrature (both edges, both channels)
//               2 for X2 quadrature (both edges, channel A only)
#define ENCODER_MODE 4
#define ENCODER_PPR 3             // Pulses per revolution (pole pairs on the magnetic disc)
#define ENCODER_TICKS_PER_REV (ENCODER_PPR * ENCODER_MODE * MOTOR_GEAR_RATIO)

// PID controller settings
#define PID_KP          6.0f
#define PID_KI          2.0f
#define PID_KD          0.0f
#define PID_OUTPUT_MIN  -255.0f
#define PID_OUTPUT_MAX   255.0f
#define DRIVE_UPDATE_INTERVAL_US  20000  // Drive loop interval in microseconds (20ms = 50Hz)

// Display and touchscreen settings
#define TFT_DEFAULT_ROTATION   0
#define TFT_DEFAULT_BG_COLOR   ILI9341_BLACK
#define TFT_DEFAULT_BRIGHTNESS 255
#define TOUCH_THRESHOLD 40
