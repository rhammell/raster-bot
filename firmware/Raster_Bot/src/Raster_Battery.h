#pragma once

#include <Arduino.h>
#include "config.h"

// Battery monitor (voltage, charging status, charger connected)
class Raster_Battery {
public:
    Raster_Battery() = default;

    void begin();
    float voltage();           // Battery voltage in volts
    bool charging();           // True when actively charging
    bool chargerConnected();   // True when charging source is plugged in
};
