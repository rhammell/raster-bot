/*
 * GraphicsTest - Basic graphics test for Raster_Bot library
 * 
 * This sketch demonstrates how to initialize the Raster_Bot,
 * draw basic shapes and text to the display, and run a
 * bouncing ball animation.
 * 
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>

Raster_Bot bot;

// Bouncing ball arena (inside the border)
#define ARENA_BORDER_X  5
#define ARENA_BORDER_Y  190
#define ARENA_BORDER_W  230
#define ARENA_BORDER_H  125

// Inner area the ball can move in (1px inside the border)
#define ARENA_X  (ARENA_BORDER_X + 1)
#define ARENA_Y  (ARENA_BORDER_Y + 1)
#define ARENA_W  (ARENA_BORDER_W - 2)
#define ARENA_H  (ARENA_BORDER_H - 2)

#define BALL_RADIUS  6
#define BALL_COLOR   ILI9341_YELLOW

int ballX, ballY;
int ballDX = 2;
int ballDY = 3;

void setup() {
  Serial.begin(115200);
  Serial.println("Raster_Bot Graphics Test");

  // Initialize the bot (display, touchscreen, etc.)
  if (!bot.begin()) {
    Serial.println("Failed to initialize Raster_Bot!");
    while (1);
  }

  Serial.println("Raster_Bot initialized successfully");

  // Draw static graphics
  drawTestScreen();

  // Initialize ball position to center of arena
  ballX = ARENA_X + ARENA_W / 2;
  ballY = ARENA_Y + ARENA_H / 2;
}

void loop() {
  bounceBall();
  delay(15);
}

void drawTestScreen() {
  // Clear screen
  bot.display.fillScreen(ILI9341_BLACK);

  // Draw title text
  bot.display.setCursor(10, 10);
  bot.display.setTextColor(ILI9341_WHITE);
  bot.display.setTextSize(2);
  bot.display.println("Raster Bot");

  bot.display.setTextSize(1);
  bot.display.setCursor(10, 35);
  bot.display.setTextColor(ILI9341_CYAN);
  bot.display.println("Graphics Test");

  // Draw rectangle
  bot.display.drawRect(10, 60, 65, 60, ILI9341_RED);
  bot.display.fillRect(15, 65, 55, 50, ILI9341_DARKGREY);

  // Draw circle
  bot.display.drawCircle(120, 90, 30, ILI9341_GREEN);
  bot.display.fillCircle(120, 90, 25, ILI9341_DARKGREEN);

  // Draw triangle
  bot.display.drawTriangle(197, 60, 165, 120, 230, 120, ILI9341_BLUE);
  bot.display.fillTriangle(197, 70, 175, 113, 220, 113, ILI9341_BLUE);

  // Draw horizontal divider
  bot.display.drawLine(10, 135, 230, 135, ILI9341_YELLOW);

  // Draw pixel grid between divider and bouncing ball label
  for (int x = 10; x <= 230; x += 10) {
    for (int y = 145; y <= 177; y += 8) {
      bot.display.drawPixel(x, y, ILI9341_WHITE);
    }
  }

  // Draw arena border
  bot.display.drawRect(ARENA_BORDER_X, ARENA_BORDER_Y, ARENA_BORDER_W, ARENA_BORDER_H, ILI9341_WHITE);

  Serial.println("Test screen drawn");
}

void bounceBall() {
  // Erase the old ball
  bot.display.fillCircle(ballX, ballY, BALL_RADIUS, ILI9341_BLACK);

  // Update position
  ballX += ballDX;
  ballY += ballDY;

  // Clamp to inner arena bounds (ball stays fully inside border)
  int minX = ARENA_X + BALL_RADIUS;
  int maxX = ARENA_X + ARENA_W - 1 - BALL_RADIUS;
  int minY = ARENA_Y + BALL_RADIUS;
  int maxY = ARENA_Y + ARENA_H - 1 - BALL_RADIUS;

  if (ballX <= minX) {
    ballX = minX;
    ballDX = abs(ballDX);
  }
  if (ballX >= maxX) {
    ballX = maxX;
    ballDX = -abs(ballDX);
  }
  if (ballY <= minY) {
    ballY = minY;
    ballDY = abs(ballDY);
  }
  if (ballY >= maxY) {
    ballY = maxY;
    ballDY = -abs(ballDY);
  }

  // Draw the new ball
  bot.display.fillCircle(ballX, ballY, BALL_RADIUS, BALL_COLOR);
}
