/*
 * MotorTest - Motor and encoder test for Raster_Bot library
 *
 * Use this sketch to test motor control and encoder
 * readings through the drive subsystem.
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 */

 #include <Raster_Bot.h>

 Raster_Bot bot;

 void setup() {
     Serial.begin(115200);
     Serial.println("Raster_Bot Motor Test");
 
     if (!bot.begin()) {
         Serial.println("Failed to initialize Raster_Bot!");
         while (1);
     }
 
     Serial.println("Raster_Bot initialized successfully");

    bot.drive.straight(3.0);
 }
 
void loop() {

  static bool stopped = false;
  if (!stopped && millis() >= 10000) {
    bot.drive.stop();
    stopped = true;
    Serial.println("--- Stop commanded ---");
  }

  static uint32_t lastRpmPrintMs = 0;
  if (millis() - lastRpmPrintMs >= 100) {
    lastRpmPrintMs = millis();
    Serial.print("T: ");
    Serial.print(millis() / 1000.0, 1);
    float avgRPM = (bot.drive._leftController.currentRPM + bot.drive._rightController.currentRPM) / 2.0f;
    float speed_cm_s = (avgRPM * WHEEL_CIRCUMFERENCE_CM) / 60.0f;
    Serial.print("s  Speed: ");
    Serial.print(speed_cm_s, 1);
    Serial.print(" cm/s  L RPM: ");
    Serial.print(bot.drive._leftController.currentRPM);
    Serial.print("  L PWM: ");
    Serial.print(bot.drive._leftController.currentPWM);
    Serial.print("  R RPM: ");
    Serial.print(bot.drive._rightController.currentRPM);
    Serial.print("  R PWM: ");
    Serial.print(bot.drive._rightController.currentPWM);
    Serial.print("  L Enc: ");
    Serial.print((long)bot.drive._leftController.encoder.getCount());
    Serial.print("  R Enc: ");
    Serial.print((long)bot.drive._rightController.encoder.getCount());
    int64_t avgTicks = (abs(bot.drive._leftController.encoder.getCount()) + abs(bot.drive._rightController.encoder.getCount())) / 2;
    Serial.print("  Diff: ");
    Serial.print((long)(bot.drive._leftController.encoder.getCount() - bot.drive._rightController.encoder.getCount()));
    Serial.print("  Dist: ");
    Serial.print((float)avgTicks / TICKS_PER_CM, 2);
    Serial.println(" cm");
  }

  bot.drive.update();
}
