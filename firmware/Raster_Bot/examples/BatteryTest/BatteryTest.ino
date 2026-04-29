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

// Draw a row of text at a fixed x position.
// Uses setTextColor(fg, bg) to overwrite old text without needing fillRect.
void drawRow(int16_t x, int16_t y, const String &text, uint16_t color) {
  bot.display.setTextSize(1);
  bot.display.setCursor(x, y);
  bot.display.setTextColor(color, ILI9341_BLACK);
  bot.display.print(text);
}

void drawStaticLayout() {
  // Clear screen and draw fixed header elements.
  bot.display.fillScreen(ILI9341_BLACK);

  bot.display.setCursor(10, 10);
  bot.display.setTextColor(ILI9341_WHITE);
  bot.display.setTextSize(2);
  bot.display.println("Raster Bot");

  bot.display.setTextSize(1);
  bot.display.setCursor(10, 35);
  bot.display.setTextColor(ILI9341_CYAN);
  bot.display.println("Battery Test");
}

void drawBatteryValues() {
  // Text size 1 = 6px per char. Longest line is 25 chars = 150px.
  // Center block horizontally, center vertically below header.
  int16_t blockW = 25 * 6;
  int16_t blockH = 48;
  int16_t x = (bot.display.width() - blockW) / 2;
  int16_t startY = (bot.display.height() - blockH) / 2;

  // Labels padded so colons align (all 19 chars including trailing space)
  // "Battery Charging: " is the longest label
  int16_t valX = x + 19 * 6;

  drawRow(x, startY,      "   USB Connected: ", ILI9341_WHITE);
  drawRow(valX, startY,   bot.battery.chargerConnected() ? "Yes    " : "No     ", bot.battery.chargerConnected() ? ILI9341_GREEN : ILI9341_WHITE);

  drawRow(x, startY + 20, "Battery Charging: ", ILI9341_WHITE);
  drawRow(valX, startY + 20, bot.battery.charging()      ? "Yes    " : "No     ", bot.battery.charging() ? ILI9341_GREEN : ILI9341_WHITE);

  drawRow(x, startY + 40, " Battery Voltage: ", ILI9341_WHITE);
  drawRow(valX, startY + 40, String(bot.battery.voltage(), 2) + " V ", ILI9341_YELLOW);
}

void setup() {
  // Bring up serial and core board peripherals.
  Serial.begin(115200);
  Serial.println("Raster_Bot Battery Test");

  if (!bot.begin()) {
    Serial.println("Failed to initialize Raster_Bot!");
    while (1);
  }

  bot.display.showSplash(3000);

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
