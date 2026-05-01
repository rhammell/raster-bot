#include "Raster_Display.h"
#include "splash_logo.h"

// =============================================================================
// Raster_Display Implementation
// =============================================================================

Raster_Display::Raster_Display()
    : Adafruit_ILI9341(&_spiBus, TFT_DC, TFT_CS, TFT_RST) {
}

bool Raster_Display::begin() {
    // Start SPI bus with explicit pins
    Serial.println("[Display] Starting SPI bus...");
    _spiBus.begin(TFT_CLK, TFT_MISO, TFT_MOSI, TFT_CS);

    // Start I2C bus for touchscreen
    Serial.println("[Display] Starting I2C bus...");
    _i2cBus.begin(TOUCH_SDA, TOUCH_SCL);

    // Initialize TFT display
    Serial.println("[Display] Initializing TFT...");
    Adafruit_ILI9341::begin();
    setRotation(TFT_DEFAULT_ROTATION);
    fillScreen(TFT_DEFAULT_BG_COLOR);

    // Initialize backlight
    Serial.println("[Display] Initializing backlight...");
    pinMode(TFT_LITE, OUTPUT);
    setBrightness(TFT_DEFAULT_BRIGHTNESS);

    // Initialize capacitive touchscreen
    Serial.println("[Display] Initializing touchscreen...");
    if (!_ts.begin(TOUCH_THRESHOLD, &_i2cBus)) {
        Serial.println("[Display] Touchscreen init FAILED");
        return false;
    }
    Serial.println("[Display] Touchscreen init OK");

    return true;
}

void Raster_Display::showSplash() {
    // Turn off backlight
    setBrightness(0);
    fillScreen(ILI9341_BLACK);

    // Draw logo
    int16_t x = (width() - logoWidth) / 2;
    int16_t y = (height() - logoHeight) / 2;
    drawBitmap(x, y, logo, logoWidth, logoHeight, ILI9341_WHITE);

    // Hold on black for 500ms
    delay(500);

    // Fade in
    for (int i = 0; i <= 255; i += 5) {
        setBrightness(i);
        delay(10);
    }

    // Hold on logo for 2000ms
    delay(2000);

    // Fade out
    for (int i = 255; i >= 0; i -= 5) {
        setBrightness(i);
        delay(10);
    }

    // Hold on black before returning
    fillScreen(ILI9341_BLACK);
    delay(500);

    // Reset backlight to default
    fillScreen(TFT_DEFAULT_BG_COLOR);
    setBrightness(TFT_DEFAULT_BRIGHTNESS);
}

void Raster_Display::setBrightness(uint8_t brightness) {
    analogWrite(TFT_LITE, brightness);
}

// Check if screen is currently being touched
bool Raster_Display::touched() {
    return _ts.touched();
}

// Get the mapped touch point based on screen rotation
TS_Point Raster_Display::getTouchPoint() {
    TS_Point p = _ts.getPoint();
    int16_t x, y;

    switch (getRotation()) {
        case 0:
            x = width() - p.x;
            y = height() - p.y;
            break;
        case 1:
            x = width() - p.y;
            y = p.x;
            break;
        case 2:
            x = p.x;
            y = p.y;
            break;
        case 3:
            x = p.y;
            y = height() - p.x;
            break;
        default:
            x = p.x;
            y = p.y;
            break;
    }

    return TS_Point(x, y, p.z);
}
