/*
 * TouchTest - Touch drawing test for Raster_Bot library
 * 
 * This sketch demonstrates how to use the touchscreen
 * to draw on the display. Touch anywhere to draw.
 * 
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>

Raster_Bot bot;

void setup() {
  Serial.begin(115200);
  Serial.println("Raster_Bot Touch Test");

  // Initialize the bot (display, touchscreen, etc.)
  if (!bot.begin()) {
    Serial.println("Failed to initialize Raster_Bot!");
    while (1);
  }

  // Set screen rotation to portrait
  bot.display.setRotation(0);

  Serial.println("Raster_Bot initialized successfully");

  // Draw title text
  bot.display.setCursor(10, 10);
  bot.display.setTextColor(ILI9341_WHITE);
  bot.display.setTextSize(2);
  bot.display.println("Raster Bot");

  bot.display.setTextSize(1);
  bot.display.setCursor(10, 35);
  bot.display.setTextColor(ILI9341_CYAN);
  bot.display.println("Touch Test");

  // Draw instruction centered on screen
  // "Touch to draw!" = 14 chars * 12px (size 2) = 168px wide, 16px tall
  int16_t textW = 14 * 12;
  int16_t textH = 16;
  int16_t textX = (bot.display.width() - textW) / 2;
  int16_t textY = (bot.display.height() - textH) / 2;
  bot.display.setCursor(textX, textY);
  bot.display.setTextColor(ILI9341_WHITE);
  bot.display.setTextSize(2);
  bot.display.println("Touch to draw!");
}

void loop() {
  if (bot.display.touched()) {
    TS_Point p = bot.display.getTouchPoint();

    // Draw a small circle where the user touches
    bot.display.fillCircle(p.x, p.y, 3, ILI9341_YELLOW);

    // Print coordinates to Serial for debugging
    Serial.print("X: "); Serial.print(p.x);
    Serial.print("\tY: "); Serial.println(p.y);
  }

  delay(10);
}
