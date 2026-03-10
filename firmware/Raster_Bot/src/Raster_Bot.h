#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>
#include "config.h"

// Custom display class wrapping TFT, Touch, and Backlight
class Raster_Display : public Adafruit_ILI9341 {
public:
    Raster_Display();

    bool begin();
    void setBrightness(uint8_t brightness);
    bool touched();
    TS_Point getTouchPoint();

private:
    Adafruit_FT6206 _ts;
};

// Battery monitor (voltage, charging status, charger connected)
class Raster_Battery {
public:
    void begin();
    float voltage();           // Battery voltage in volts
    bool charging();           // True when actively charging
    bool chargerConnected();   // True when charging source is plugged in

private:
};

class Raster_Bot {
public:
    Raster_Bot();

    bool begin();
    void update();

    Raster_Display display;
    Raster_Battery battery;

private:
};
