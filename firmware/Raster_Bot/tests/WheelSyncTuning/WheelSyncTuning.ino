/*
 * WheelSyncTuning - Wheel-sync correction tuning for Raster_Bot library
 *
 * Prerequisite: PID tuning must be completed first (see PIDTuning).
 * Disable ramping so the bot runs at a constant RPM.
 *
 * Use this sketch to tune the single wheel-sync correction gain
 * (WHEEL_SYNC_KP) defined in robot_definitions.h. The same gain
 * balances both a straight (wheels together) and a spin (wheels
 * opposite), so this sketch can exercise either one. Set SPIN_MODE
 * below to pick which, then recompile and upload for each test.
 *
 * Both modes monitor the balance "diff" between the wheels: for a
 * straight it is (L - R) and should stay near zero as the counts
 * track together; for a spin it is (L + R) and should stay near zero
 * as the counts grow equal-and-opposite. The diff is shown large on
 * the TFT for untethered floor testing, green near zero and red when
 * drifting.
 *
 * If the diff drifts steadily in one direction, WHEEL_SYNC_KP is too
 * low and the correction is not strong enough to keep the wheels
 * matched. If the diff oscillates rapidly around zero, WHEEL_SYNC_KP
 * is too high and the correction is overcorrecting. A good result
 * is a diff that stays near zero with no sustained drift or
 * oscillation.
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>

// Tuning options: set SPIN_MODE to false to tune a straight, true to tune a
// spin. DRIVE_SPEED_CM_S is the wheel speed used for either move.
constexpr bool  SPIN_MODE       = false;
constexpr float DRIVE_SPEED_CM_S = 6.0f;

// Raster Bot instance
Raster_Bot bot;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("Raster Bot Wheel Sync Tuning");

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
    bot.display.print(SPIN_MODE ? "Spin Tuning" : "Straight Tuning");
    bot.display.setTextColor(ILI9341_DARKGREY);
    bot.display.setCursor(4, 36);
    bot.display.print(SPIN_MODE ? "Diff (L+R):" : "Diff (L-R):");

    // Start the move being tuned
    if (SPIN_MODE) {
        bot.drive.spin(DRIVE_SPEED_CM_S);
    } else {
        bot.drive.straight(DRIVE_SPEED_CM_S);
    }
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
        // Update the last print time
        lastPrintMs = millis();

        // Get the drive telemetry
        DriveTelemetry s = bot.drive.getTelemetry();

        // Balance error to drive toward zero. Straight wheels track together
        // (L - R); spin wheels run equal-and-opposite (L + R).
        int64_t diff = SPIN_MODE ? (s.leftCount + s.rightCount)
                                 : (s.leftCount - s.rightCount);

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
        bot.display.print((long)s.leftCount);
        bot.display.setCursor(4, 172);
        bot.display.print("R: ");
        bot.display.print((long)s.rightCount);

        // Display the average wheel speed. Use magnitudes so a spin (wheels
        // opposite) reports each wheel's tangential speed instead of ~0.
        float avgRPM = (fabsf(s.leftRPM) + fabsf(s.rightRPM)) / 2.0f;
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
        Serial.print((long)s.leftCount);
        Serial.print("  R Enc: ");
        Serial.print((long)s.rightCount);
        Serial.print("  L RPM: ");
        Serial.print(s.leftRPM, 1);
        Serial.print("  R RPM: ");
        Serial.println(s.rightRPM, 1);
    }

    // Run the drive control loop
    bot.drive.update();
}
