#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include "../definitions/pin_definitions.h"
#include "../definitions/robot_definitions.h"

// Custom display class wrapping TFT, Touch, and Backlight
// Inherits from Adafruit_ILI9341
class Raster_Display : public Adafruit_ILI9341 {
public:
    // Constructor
    Raster_Display();

    // Public methods
    bool begin();
    void showSplash();
    void setBrightness(uint8_t brightness);
    bool touched();
    TS_Point getTouchPoint();

private:
    // Private members
    SPIClass _spiBus = SPIClass(FSPI);
    TwoWire  _i2cBus = TwoWire(0);
    Adafruit_FT6206 _ts;
};
