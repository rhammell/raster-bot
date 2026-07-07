/*
 * 2_PIDTuning - PID gain tuning for Raster_Bot library
 *
 * Run this AFTER 1_FeedforwardTuning. The recommended order is: characterize the
 * velocity feedforward first (tuning/1_FeedforwardTuning), enable it, then tune
 * the PID on top of it here. With feedforward supplying the bulk of the holding
 * PWM, the PID only has to trim the small residual, so the gains (especially
 * KI) stay low and the loop is well behaved.
 *
 * Prerequisites before tuning:
 *   - PID_FEEDFORWARD_ENABLE = 1 with a PID_FF table from 1_FeedforwardTuning
 *     (or set it to 0 to tune pure PID with no feedforward).
 *   - WHEEL_SYNC_KP = 0 and MAX_ACCEL_RPM_S = 0 (ramping off), so the PID
 *     responds to a clean step input with no other control loops active.
 *   - Leave RPM_FILTER_ALPHA at its normal value (do NOT zero it) - the RPM
 *     filter is a passive part of the system the PID must be tuned against,
 *     not a competing control loop.
 *
 * Use this sketch to tune the PID speed controller gains (PID_KP,
 * PID_KI, PID_KD) defined in robot_definitions.h. Each change
 * requires recompiling and uploading.
 *
 * The sketch commands a constant speed and prints the measured RPM
 * so you can observe how the controller responds.
 *
 * Output modes (set PLOT_MODE below):
 *   0 = Human-readable Serial Monitor output
 *   1 = Arduino Serial Plotter output (Target, L_RPM, R_RPM)
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 *
 * --- Quick PID Tuning Guide ---
 *
 * 1. Set PID_KI = 0 and start with a small PID_KP (e.g. 2.0) in
 *    robot_definitions.h. Compile, upload, and open Serial Monitor.
 *
 * 2. Watch how the reported RPM approaches the target.
 *    - Too slow to reach target: increase KP.
 *    - Oscillates around target or overshoots: decrease KP.
 *    Repeat until RPM rises quickly without oscillating. A small
 *    steady-state error (offset from target) is expected with KP alone.
 *    (With feedforward enabled the wheel starts near target, so the P
 *    response is small - that is expected.)
 *
 * 3. Add a small PID_KI (e.g. 1.0) to eliminate the steady-state error.
 *    - RPM still settles below target: increase KI.
 *    - RPM overshoots then slowly settles, or oscillates: decrease KI.
 *    With feedforward doing most of the work, KI can stay low - it only
 *    trims the residual the feedforward leaves behind.
 *
 * 4. Good result: RPM reaches and holds the target with no sustained
 *    oscillation and no visible overshoot. Convergence within a few
 *    seconds is acceptable — ramping will eliminate the large step
 *    response in normal operation.
 *
 * 5. Sanity-check the tune at several speeds across your range (set
 *    DRIVE_SPEED_CM_S below), since the motor is nonlinear. When done,
 *    re-enable MAX_ACCEL_RPM_S (see tuning/4_DistanceTuning) for smooth launches.
 */

#include <Raster_Bot.h>

// Raster Bot instance
Raster_Bot bot;

// Drive speed and target RPM
// Clamp to MAX_SPEED_CM_S so the displayed target matches the setpoint
// straight() actually commands (it constrains speed to this limit)
const float DRIVE_SPEED_CM_S = 8.0f;
const float CLAMPED_SPEED_CM_S = constrain(DRIVE_SPEED_CM_S, -MAX_SPEED_CM_S, MAX_SPEED_CM_S);
const float TARGET_RPM = (CLAMPED_SPEED_CM_S * 60.0f) / WHEEL_CIRCUMFERENCE_CM;

// Plot mode
// 0 = Serial Monitor, 1 = Serial Plotter
const int   PLOT_MODE = 0;  

// Center a float value on the 240px-wide display
void printCentered(float value, int decimals, int y, int textSize) {
    char buf[16];
    dtostrf(value, 0, decimals, buf);
    bot.display.setTextSize(textSize);
    int w = strlen(buf) * 6 * textSize;
    bot.display.setCursor((240 - w) / 2, y);
    bot.display.print(buf);
}

// Center a "PWM <value>" readout (size 2) on the 240px-wide display
void printPWM(float pwm, int y) {
    char buf[16];
    snprintf(buf, sizeof(buf), "PWM %d", (int)pwm);
    bot.display.setTextSize(2);
    int w = strlen(buf) * 6 * 2;
    bot.display.setCursor((240 - w) / 2, y);
    bot.display.print(buf);
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("Raster Bot PID Tuning");

    // Initialize the bot
    if (!bot.begin()) {
        Serial.println("Failed to initialize Raster Bot!");
        while (1);
    }
    Serial.println("Raster Bot initialized successfully");

    // Print the target RPM
    Serial.print("Target RPM: ");
    Serial.println(TARGET_RPM, 1);

    // Draw static TFT layout (240px wide)
    bot.display.fillScreen(ILI9341_BLACK);

    // Draw the title text
    bot.display.setTextColor(ILI9341_CYAN);
    bot.display.setTextSize(2);
    bot.display.setCursor(10, 6);
    bot.display.print("PID Tuning");
    bot.display.setTextColor(ILI9341_WHITE);
    bot.display.setTextSize(1);
    bot.display.setCursor(10, 26);
    bot.display.print("Raster Bot");

    // Draw the static labels
    bot.display.setTextColor(ILI9341_DARKGREY);
    bot.display.setTextSize(2);
    bot.display.setCursor(54, 58);
    bot.display.print("Target RPM:");
    bot.display.setCursor(84, 128);
    bot.display.print("L RPM:");
    bot.display.setCursor(84, 214);
    bot.display.print("R RPM:");

    // Show target RPM (static)
    bot.display.setTextColor(ILI9341_WHITE);
    printCentered(TARGET_RPM, 1, 78, 4);

    // Placeholders for RPM and PWM values
    bot.display.setTextColor(ILI9341_DARKGREY);
    bot.display.setTextSize(4);
    bot.display.setCursor(108, 148);
    bot.display.print("-");
    bot.display.setCursor(108, 234);
    bot.display.print("-");

    // Countdown before driving
    for (int i = 5; i > 0; i--) {
        bot.display.fillRect(0, 304, 240, 16, ILI9341_BLACK);
        bot.display.setTextSize(1);
        bot.display.setTextColor(ILI9341_DARKGREY);
        bot.display.setCursor(4, 308);
        bot.display.print("Driving in ");
        bot.display.print(i);
        bot.display.print("...");
        delay(1000);
    }
    bot.display.fillRect(0, 304, 240, 16, ILI9341_BLACK);

    // Drive forward at the target speed
    bot.drive.straight(DRIVE_SPEED_CM_S);
}

void loop() {
    // Print the motor status every 100 milliseconds
    static uint32_t lastPrintMs = 0;
    if (millis() - lastPrintMs >= 100) {
        // Update the last print time
        lastPrintMs = millis();

        // Get the drive telemetry
        DriveTelemetry s = bot.drive.getTelemetry();

        // Left RPM — green when within 10% of target, red otherwise
        bot.display.fillRect(0, 148, 240, 32, ILI9341_BLACK);
        bot.display.setTextColor(abs(s.leftRPM - TARGET_RPM) < TARGET_RPM * 0.05f ? ILI9341_GREEN : ILI9341_RED);
        printCentered(s.leftRPM, 1, 148, 4);
        bot.display.fillRect(0, 184, 240, 16, ILI9341_BLACK);
        bot.display.setTextColor(ILI9341_YELLOW);
        printPWM(s.leftPWM, 184);

        // Right RPM
        bot.display.fillRect(0, 234, 240, 32, ILI9341_BLACK);
        bot.display.setTextColor(abs(s.rightRPM - TARGET_RPM) < TARGET_RPM * 0.05f ? ILI9341_GREEN : ILI9341_RED);
        printCentered(s.rightRPM, 1, 234, 4);
        bot.display.fillRect(0, 270, 240, 16, ILI9341_BLACK);
        bot.display.setTextColor(ILI9341_YELLOW);
        printPWM(s.rightPWM, 270);

        // Print the motor status in the plot mode
        if (PLOT_MODE) {
            Serial.print("Target:");
            Serial.print(TARGET_RPM, 1);
            Serial.print(" L_RPM:");
            Serial.print(s.leftRPM, 1);
            Serial.print(" R_RPM:");
            Serial.println(s.rightRPM, 1);

        // Print the motor status in the serial monitor mode
        } else {
            Serial.print("T: ");
            Serial.print(millis() / 1000.0, 1);
            Serial.print("s  Target: ");
            Serial.print(TARGET_RPM, 1);
            Serial.print("  L RPM: ");
            Serial.print(s.leftRPM, 1);
            Serial.print("  L PWM: ");
            Serial.print(s.leftPWM, 0);
            Serial.print("  R RPM: ");
            Serial.print(s.rightRPM, 1);
            Serial.print("  R PWM: ");
            Serial.println(s.rightPWM, 0);
        }
    }

    // Update the drive subsystem
    bot.drive.update();
}
