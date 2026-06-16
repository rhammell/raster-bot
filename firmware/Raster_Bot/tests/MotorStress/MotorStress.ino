/*
 * MotorStress - Direct PWM motor stress test (standalone)
 *
 * Use this sketch to stress test the motors and motor driver by
 * applying raw PWM directly to the DRV8833 pins, bypassing the
 * Raster_Bot library, PID, and drive control entirely.
 *
 * Two test modes (set TEST_MODE below):
 *   0 = Steady: 5s pause, 5s motors on at TEST_PWM, repeat.
 *       Useful for multimeter current measurements.
 *   1 = Reversal: 5s pause, then instant full-speed direction
 *       reversals every REVERSAL_MS for RUN_TIME_MS total.
 *       Produces maximum current spikes at each transition.
 *
 * Pin assignments below mirror pin_definitions.h for the v0.2 board.
 * Update them if testing a different board revision.
 */

// DRV8833 pins (v0.2 board)
const uint8_t DRV_SLEEP    = 40;
const uint8_t LEFT_IN1     = 47;
const uint8_t LEFT_IN2     = 33;
const uint8_t RIGHT_IN1    = 48;
const uint8_t RIGHT_IN2    = 34;

const int  TEST_MODE   = 1;     // 0 = steady, 1 = reversal stress
const int  TEST_PWM    = 100;   // PWM magnitude (0-255)
const int  REVERSAL_MS = 2000;  // Time in each direction before reversing
const int  RUN_TIME_MS = 30000; // Total run time per cycle (mode 1)

// Apply a signed PWM to one motor channel.
// Sign sets direction; magnitude sets duty cycle.
void setMotorPWM(uint8_t in1, uint8_t in2, int pwm) {
    pwm = constrain(pwm, -255, 255);
    if (pwm > 0) {
        analogWrite(in1, pwm);
        analogWrite(in2, 0);
    } else if (pwm < 0) {
        analogWrite(in1, 0);
        analogWrite(in2, -pwm);
    } else {
        analogWrite(in1, 0);
        analogWrite(in2, 0);
    }
}

// Drive both motors at the same signed PWM.
void setBothMotors(int pwm) {
    setMotorPWM(LEFT_IN1, LEFT_IN2, pwm);
    setMotorPWM(RIGHT_IN1, RIGHT_IN2, pwm);
}

void stopBothMotors() {
    setBothMotors(0);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Motor Stress Test (standalone)");

    // Configure DRV8833 control pins
    pinMode(DRV_SLEEP, OUTPUT);
    pinMode(LEFT_IN1, OUTPUT);
    pinMode(LEFT_IN2, OUTPUT);
    pinMode(RIGHT_IN1, OUTPUT);
    pinMode(RIGHT_IN2, OUTPUT);

    // Enable the driver and ensure motors start stopped
    digitalWrite(DRV_SLEEP, HIGH);
    stopBothMotors();

    Serial.print("Test mode: ");
    Serial.println(TEST_MODE == 0 ? "Steady PWM" : "Reversal stress");
    Serial.print("PWM magnitude: ");
    Serial.println(TEST_PWM);
}

void loop() {
    Serial.println("--- Paused (5s) ---");
    delay(5000);

    if (TEST_MODE == 0) {
        Serial.println("--- Motors ON ---");
        setBothMotors(TEST_PWM);
        delay(5000);

        Serial.println("--- Motors OFF ---");
        stopBothMotors();

    } else {
        Serial.println("--- Reversal stress test start ---");
        int pwm = TEST_PWM;
        uint32_t startTime = millis();

        while (millis() - startTime < RUN_TIME_MS) {
            Serial.print("PWM: ");
            Serial.println(pwm);
            setBothMotors(pwm);
            delay(REVERSAL_MS);
            pwm = -pwm;
        }

        Serial.println("--- Motors OFF ---");
        stopBothMotors();
    }
}
