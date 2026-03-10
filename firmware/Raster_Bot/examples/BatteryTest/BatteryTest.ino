/*
 * BatteryTest - Battery status test for Raster_Bot library
 *
 * Shows charger connection, charging state, and battery voltage
 * using the Raster_Bot battery API.
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>

Raster_Bot bot;

// Simple timer for periodic display refresh.
static uint32_t lastUpdateMs = 0;

// Draw one line of text centered on the display.
void drawCenteredText(const String &text, int16_t y, uint8_t textSize, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;

  bot.display.setTextSize(textSize);
  bot.display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);

  int16_t x = (bot.display.width() - (int16_t)w) / 2;
  bot.display.setCursor(x, y);
  bot.display.setTextColor(color);
  bot.display.print(text);
}

void drawStaticLayout() {
  // Clear screen and draw fixed header elements.
  bot.display.fillScreen(ILI9341_BLACK);

  drawCenteredText("Raster Bot", 12, 2, ILI9341_WHITE);
  drawCenteredText("Battery Test", 38, 1, ILI9341_CYAN);

  bot.display.drawLine(20, 58, bot.display.width() - 20, 58, ILI9341_DARKGREY);
}

void drawBatteryValues() {
  // Clear the value area, then redraw all rows.
  bot.display.fillRect(0, 70, bot.display.width(), 80, ILI9341_BLACK);

  // Build item/value strings from the battery API.
  String usbConnected = String("USB Connected: ") +
                        (bot.battery.chargerConnected() ? "Yes" : "No");
  String batteryCharging = String("Battery Charging: ") +
                           (bot.battery.charging() ? "Yes" : "No");
  String batteryVoltage = String("Battery Voltage: ") +
                          String(bot.battery.voltage(), 2) + " V";

  drawCenteredText(usbConnected, 78, 1, ILI9341_WHITE);
  drawCenteredText(batteryCharging, 98, 1, ILI9341_WHITE);
  drawCenteredText(batteryVoltage, 118, 1, ILI9341_YELLOW);
}

void setup() {
  // Bring up serial and core board peripherals.
  Serial.begin(115200);
  Serial.println("Raster_Bot Battery Test");

  if (!bot.begin()) {
    Serial.println("Failed to initialize Raster_Bot!");
    while (1);
  }

  // Use portrait mode to match other examples.
  bot.display.setRotation(0);

  // Draw static UI once and values immediately.
  drawStaticLayout();
  drawBatteryValues();
  lastUpdateMs = millis();
}

void loop() {
  // Update UI and serial output at a steady interval.
  const uint32_t now = millis();
  if (now - lastUpdateMs >= 500) {
    drawBatteryValues();
    lastUpdateMs = now;

    Serial.print("USB: ");
    Serial.print(bot.battery.chargerConnected() ? "Connected" : "Not connected");
    Serial.print(" | Charging: ");
    Serial.print(bot.battery.charging() ? "Yes" : "No");
    Serial.print(" | Voltage: ");
    Serial.print(bot.battery.voltage(), 2);
    Serial.println(" V");
  }
}
