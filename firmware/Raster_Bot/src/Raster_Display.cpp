#include "Raster_Display.h"

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

void Raster_Display::showSplash(uint32_t duration_ms) {
    fillScreen(ILI9341_BLACK);
    setTextSize(3);
    setTextColor(ILI9341_WHITE);

    // "Raster" — 6 chars × 18px = 108px wide, 24px tall
    // "Bot"    — 3 chars × 18px =  54px wide, 24px tall
    // 8px gap between lines, total block height = 24 + 8 + 24 = 56px
    int16_t line1X = (width() - 108) / 2;
    int16_t line2X = (width() - 54) / 2;
    int16_t topY   = (height() - 56) / 2;

    setCursor(line1X, topY);
    println("Raster");
    setCursor(line2X, topY + 32);
    println("Bot");

    delay(duration_ms);
    fillScreen(TFT_DEFAULT_BG_COLOR);
}

void Raster_Display::setBrightness(uint8_t brightness) {
    // Troubleshooting-friendly backlight control:
    // use deterministic HIGH/LOW behavior to rule out PWM setup issues.
    digitalWrite(TFT_LITE, (brightness > 0) ? HIGH : LOW);
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
