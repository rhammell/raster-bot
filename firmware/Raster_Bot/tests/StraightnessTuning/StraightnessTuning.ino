/*
 * StraightnessTuning - Straightness correction tuning for Raster_Bot library
 *
 * Prerequisite: PID tuning must be completed first (see PIDTuning).
 * Disable ramping so the bot runs at a constant RPM.
 *
 * Use this sketch to tune the straightness correction gain
 * (STRAIGHT_KP) defined in robot_definitions.h. Each change
 * requires recompiling and uploading.
 *
 * The sketch drives the bot forward and monitors the encoder tick
 * difference (diff) between the left and right wheels. The diff is
 * displayed on the TFT for untethered floor testing, turning green
 * when near zero and red when drifting.
 *
 * If the diff drifts steadily in one direction, STRAIGHT_KP is too
 * low and the correction is not strong enough to keep the wheels
 * matched. If the diff oscillates rapidly around zero, STRAIGHT_KP
 * is too high and the correction is overcorrecting. A good result
 * is a diff that stays near zero with no sustained drift or
 * oscillation.
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>

// Raster Bot instance
Raster_Bot bot;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("Raster Bot Straightness Tuning");

    // Initialize the bot
    if (!bot.begin()) {
        Serial.println("Failed to initialize Raster Bot!");
        while (1);
    }
    Serial.println("Raster Bot initialized successfully");

    // Draw static TFT layout
    bot.display.fillScreen(ILI9341_BLACK);
    bot.display.setTextColor(ILI9341_CYAN);
    bot.display.setTextSize(2);
    bot.display.setCursor(4, 6);
    bot.display.print("Straightness Tuning");
    bot.display.setTextColor(ILI9341_DARKGREY);
    bot.display.setCursor(4, 36);
    bot.display.print("Diff:");

    // Start driving forward
    bot.drive.straight(6.0);
}

void loop() {
    // Stop after 10 seconds
    static bool stopped = false;
    if (!stopped && millis() >= 10000) {
        bot.drive.stop();
        stopped = true;
        Serial.println("--- Stop commanded ---");
    }

    // Update the TFT and serial output at 10 Hz
    static uint32_t lastPrintMs = 0;
    if (millis() - lastPrintMs >= 100) {
        lastPrintMs = millis();

        // Read current motor and encoder state
        MotorStatus m = bot.drive.getMotorStatus();
        EncoderStatus e = bot.drive.getEncoderStatus();
        int64_t diff = e.leftCount - e.rightCount;

        // Display diff in large text, green when near zero, red when drifting
        bot.display.fillRect(4, 70, 232, 60, ILI9341_BLACK);
        bot.display.setTextSize(6);
        bot.display.setTextColor(abs(diff) < 50 ? ILI9341_GREEN : ILI9341_RED);
        bot.display.setCursor(4, 70);
        bot.display.print((long)diff);

        // Display per-wheel encoder counts
        bot.display.fillRect(4, 150, 232, 80, ILI9341_BLACK);
        bot.display.setTextSize(2);
        bot.display.setTextColor(ILI9341_WHITE);
        bot.display.setCursor(4, 150);
        bot.display.print("L: ");
        bot.display.print((long)e.leftCount);
        bot.display.setCursor(4, 172);
        bot.display.print("R: ");
        bot.display.print((long)e.rightCount);

        // Display the average speed
        float avgRPM = (m.leftRPM + m.rightRPM) / 2.0f;
        float speed_cm_s = (avgRPM * WHEEL_CIRCUMFERENCE_CM) / 60.0f;
        bot.display.setCursor(4, 194);
        bot.display.print("Speed: ");
        bot.display.print(speed_cm_s, 1);
        bot.display.print(" cm/s");

        // Serial telemetry
        Serial.print("T: ");
        Serial.print(millis() / 1000.0, 1);
        Serial.print("s  Diff: ");
        Serial.print((long)diff);
        Serial.print("  L Enc: ");
        Serial.print((long)e.leftCount);
        Serial.print("  R Enc: ");
        Serial.print((long)e.rightCount);
        Serial.print("  L RPM: ");
        Serial.print(m.leftRPM, 1);
        Serial.print("  R RPM: ");
        Serial.println(m.rightRPM, 1);
    }

    // Run the drive control loop
    bot.drive.update();
}
