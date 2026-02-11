#include "Raster_Bot.h"

// =============================================================================
// Raster_Display Implementation
// =============================================================================

// Constructor for Raster_Display
// Initializes the SPI bus, I2C bus, and passes SPI to the parent class
// Uses the default SPI bus (configured in begin() via SPI.begin())
Raster_Display::Raster_Display()
    : Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST) {
}

bool Raster_Display::begin() {
    // 1. Start hardware buses with explicit pins
    SPI.begin(TFT_CLK, TFT_MISO, TFT_MOSI, TFT_CS);
    Wire.begin(I2C_SDA, I2C_SCL);

    // 2. Initialize TFT
    Adafruit_ILI9341::begin();
    setRotation(TFT_DEFAULT_ROTATION);
    fillScreen(TFT_DEFAULT_BG_COLOR);

    // 3. Initialize Backlight
    pinMode(TFT_LITE, OUTPUT);
    setBrightness(TFT_DEFAULT_BRIGHTNESS);

    // 4. Initialize Touch (uses default Wire bus, configured above)
    if (!_ts.begin(TOUCH_THRESHOLD)) {
        return false;
    }

    return true;
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
    int16_t x = p.x;
    int16_t y = p.y;
    int16_t tmp;

    switch (getRotation()) {
        case 0: // Portrait
            x = map(p.x, 0, TOUCH_WIDTH, width(), 0);
            y = map(p.y, 0, TOUCH_HEIGHT, height(), 0);
            break;

        case 1: // Landscape
            x = map(p.y, 0, TOUCH_HEIGHT, width(), 0);
            y = map(p.x, 0, TOUCH_WIDTH, 0, height());
            break;

        case 2: // Inverted Portrait
            x = map(p.x, 0, TOUCH_WIDTH, 0, width());
            y = map(p.y, 0, TOUCH_HEIGHT, 0, height());
            break;

        case 3: // Inverted Landscape
            x = map(p.y, 0, TOUCH_HEIGHT, 0, width());
            y = map(p.x, 0, TOUCH_WIDTH, height(), 0);
            break;
    }

    return TS_Point(x, y, p.z);
}


// =============================================================================
// Raster_Bot Implementation
// =============================================================================

Raster_Bot::Raster_Bot() {
}

bool Raster_Bot::begin() {
    if (!display.begin()) {
        return false;
    }

    return true;
}

void Raster_Bot::update() {
    // TODO: Handle any periodic tasks
}
