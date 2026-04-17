/*
 * TuningTest - PID tuning test for Raster_Bot library
 *
 * Use this sketch to tune PID gains and test RPM control
 * through the drive subsystem.
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>

Raster_Bot bot;

void setup() {
    Serial.begin(115200);
    Serial.println("Raster_Bot Tuning Test");

    if (!bot.begin()) {
        Serial.println("Failed to initialize Raster_Bot!");
        while (1);
    }

    Serial.println("Raster_Bot initialized successfully");

    bot.drive.straight(30.0);
}

void loop() {
    static uint32_t lastRpmPrintMs = 0;
    if (millis() - lastRpmPrintMs >= 100) {
        lastRpmPrintMs = millis();
        Serial.print("T: ");
        Serial.print(millis() / 1000.0, 1);
        Serial.print("s  L RPM: ");
        Serial.print(bot.drive._leftController.currentRPM);
        Serial.print("  L PWM: ");
        Serial.print(bot.drive._leftController.currentPWM);
        Serial.print("  R RPM: ");
        Serial.print(bot.drive._rightController.currentRPM);
        Serial.print("  R PWM: ");
        Serial.println(bot.drive._rightController.currentPWM);
    }

    bot.drive.update();
}
