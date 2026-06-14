/*
 * MotorStress - Direct PWM motor stress test for Raster_Bot library
 *
 * Use this sketch to stress test the motors and motor driver by
 * applying raw PWM signals, bypassing PID and drive control.
 *
 * Two test modes (set TEST_MODE below):
 *   0 = Steady: 5s pause, 5s motors on at TEST_PWM, repeat.
 *       Useful for multimeter current measurements.
 *   1 = Reversal: 5s pause, then instant full-speed direction
 *       reversals every REVERSAL_MS for RUN_TIME_MS total.
 *       Produces maximum current spikes at each transition.
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>

Raster_Bot bot;

const int  TEST_MODE   = 1;     // 0 = steady, 1 = reversal stress
const int  TEST_PWM    = 100;   // PWM magnitude (0-255)
const int  REVERSAL_MS = 2000;  // Time in each direction before reversing
const int  RUN_TIME_MS = 30000; // Total run time per cycle (mode 1)

void setup() {
    Serial.begin(115200);
    Serial.println("Raster Bot Motor Stress Test");

    if (!bot.begin()) {
        Serial.println("Failed to initialize Raster Bot!");
        while (1);
    }

    Serial.println("Raster Bot initialized successfully");
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
        bot.drive._leftController.motor.setPWM(TEST_PWM);
        bot.drive._rightController.motor.setPWM(TEST_PWM);
        delay(5000);

        Serial.println("--- Motors OFF ---");
        bot.drive._leftController.motor.stop();
        bot.drive._rightController.motor.stop();

    } else {
        Serial.println("--- Reversal stress test start ---");
        int pwm = TEST_PWM;
        uint32_t startTime = millis();

        while (millis() - startTime < RUN_TIME_MS) {
            Serial.print("PWM: ");
            Serial.println(pwm);
            bot.drive._leftController.motor.setPWM(pwm);
            bot.drive._rightController.motor.setPWM(pwm);
            delay(REVERSAL_MS);
            pwm = -pwm;
        }

        Serial.println("--- Motors OFF ---");
        bot.drive._leftController.motor.stop();
        bot.drive._rightController.motor.stop();
    }
}
