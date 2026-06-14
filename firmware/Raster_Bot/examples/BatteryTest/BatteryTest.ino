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

// Print a centered string at a given Y position (size 2, 12px per char)
void printCentered(const char* text, int y, uint16_t color) {
  int w = strlen(text) * 12;
  bot.display.setTextSize(2);
  bot.display.setTextColor(color);
  bot.display.setCursor((240 - w) / 2, y);
  bot.display.print(text);
}

void drawStaticLayout() {
  bot.display.fillScreen(ILI9341_BLACK);

  // Header
  bot.display.setTextColor(ILI9341_CYAN);
  bot.display.setTextSize(2);
  bot.display.setCursor(10, 10);
  bot.display.print("Battery Test");
  bot.display.setTextColor(ILI9341_WHITE);
  bot.display.setTextSize(1);
  bot.display.setCursor(10, 30);
  bot.display.print("Raster Bot");

  // Static labels
  printCentered("USB Connected", 87, ILI9341_DARKGREY);
  printCentered("Charging", 148, ILI9341_DARKGREY);
  printCentered("Voltage", 209, ILI9341_DARKGREY);
}

void drawBatteryValues() {
  // USB Connected value
  bot.display.fillRect(0, 107, 240, 16, ILI9341_BLACK);
  bool usb = bot.battery.chargerConnected();
  printCentered(usb ? "Yes" : "No", 107, usb ? ILI9341_GREEN : ILI9341_WHITE);

  // Charging value
  bot.display.fillRect(0, 168, 240, 16, ILI9341_BLACK);
  bool chg = bot.battery.charging();
  printCentered(chg ? "Yes" : "No", 168, chg ? ILI9341_GREEN : ILI9341_WHITE);

  // Voltage value
  bot.display.fillRect(0, 229, 240, 16, ILI9341_BLACK);
  char buf[16];
  dtostrf(bot.battery.voltage(), 0, 2, buf);
  strcat(buf, " V");
  printCentered(buf, 229, ILI9341_YELLOW);
}

void setup() {
  // Bring up serial and core board peripherals.
  Serial.begin(115200);
  Serial.println("Raster Bot Battery Test");

  // Initialize the bot
  if (!bot.begin()) {
    Serial.println("Failed to initialize Raster Bot!");
    while (1);
  }
  Serial.println("Raster Bot initialized successfully");

  // Show the splash screen
  bot.display.showSplash();

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
