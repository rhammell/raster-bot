#pragma once

#include <Arduino.h>
#include "Raster_Display.h"
#include "Raster_Battery.h"

class Raster_Bot {
public:
    Raster_Bot();

    bool begin();
    void update();

    Raster_Display display;
    Raster_Battery battery;

private:
};
