/*
 * pin_definitions.h - Hardware pin assignments and wiring orientation
 *
 * Constants defined in this file describe the physical wiring
 * of a specific board revision: which MCU pin connects to which
 * peripheral, and how the motors and encoders are oriented.
 * These are hardware facts, not tunable behavior.
 */

#pragma once

// Board version: 1 for v0.3, 0 for v0.4
#define BOARD_V03  0

#if BOARD_V03

// TFT Pins (v0.3)
#define TFT_CS    14
#define TFT_DC    12
#define TFT_RST   13
#define TFT_LITE   9
#define TFT_CLK   10
#define TFT_MOSI  11
#define TFT_MISO -1

// Touchscreen pins (v0.3)
#define TOUCH_SDA   8
#define TOUCH_SCL  18

// Battery charging pins (v0.3)
#define CHG      17
#define PGOOD    16

// Battery voltage monitor (v0.3)
#define VBAT_DIV   7

// Motor driver (v0.3)
#define DRV_SLEEP 42

// Left motor (v0.3)
#define MOT_LEFT_IN1    21  // PWM A
#define MOT_LEFT_IN2    47  // PWM B
#define MOT_LEFT_ENC_A  40  // Encoder channel A
#define MOT_LEFT_ENC_B  41  // Encoder channel B

// Right motor (v0.3)
#define MOT_RIGHT_IN1     39  // PWM A
#define MOT_RIGHT_IN2     38  // PWM B
#define MOT_RIGHT_ENC_A    2  // Encoder channel A
#define MOT_RIGHT_ENC_B    1  // Encoder channel B

#else

// TFT Pins (v0.2)
#define TFT_CS    17
#define TFT_DC    15
#define TFT_RST   16
#define TFT_LITE  12
#define TFT_CLK   13
#define TFT_MOSI  14
#define TFT_MISO -1

// Touchscreen pins (v0.2)
#define TOUCH_SDA  21
#define TOUCH_SCL  18

// Battery charging pins (v0.2)
#define CHG      36
#define PGOOD    35

// Battery voltage monitor (v0.2)
#define VBAT_DIV  10

// Motor driver (v0.2)
#define DRV_SLEEP 40

// Left motor (v0.2)
#define MOT_LEFT_IN1    47  // PWM A
#define MOT_LEFT_IN2    33  // PWM B
#define MOT_LEFT_ENC_A  38  // Encoder channel A
#define MOT_LEFT_ENC_B  39  // Encoder channel B

// Right motor (v0.2)
#define MOT_RIGHT_IN1     48  // PWM A 
#define MOT_RIGHT_IN2     34  // PWM B
#define MOT_RIGHT_ENC_A   41  // Encoder channel A
#define MOT_RIGHT_ENC_B   42  // Encoder channel B

#endif

// Motor / encoder wiring orientation (both versions)
#define MOT_LEFT_FLIP       false  // Swap PWM pins so +PWM = forward
#define MOT_LEFT_ENC_FLIP   false  // Swap encoder pins so +PWM = +count
#define MOT_RIGHT_FLIP       true   // Swap PWM pins so +PWM = forward
#define MOT_RIGHT_ENC_FLIP   true  // Swap encoder pins so +PWM = +count
