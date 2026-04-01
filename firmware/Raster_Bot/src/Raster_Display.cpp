#include "Raster_Display.h"

// =============================================================================
// Raster_Display Implementation
// =============================================================================

Raster_Display::Raster_Display()
    : Adafruit_ILI9341(&_spiBus, TFT_DC, TFT_CS, TFT_RST) {
}

bool Raster_Display::begin() {
    // Start hardware buses with explicit pins
    _spiBus.begin(TFT_CLK, TFT_MISO, TFT_MOSI, TFT_CS);
    _i2cBus.begin(TOUCH_SDA, TOUCH_SCL);

    // Initialize TFT
    Adafruit_ILI9341::begin();
    setRotation(TFT_DEFAULT_ROTATION);
    fillScreen(TFT_DEFAULT_BG_COLOR);

    // Initialize backlight
    pinMode(TFT_LITE, OUTPUT);
    setBrightness(TFT_DEFAULT_BRIGHTNESS);

    // Initialize touch
    if (!_ts.begin(TOUCH_THRESHOLD, &_i2cBus)) {
        return false;
    }

    return true;
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
