#pragma once

#include <Arduino.h>
#include "Raster_Display.h"
#include "Raster_Battery.h"
#include "Raster_Drive.h"

class Raster_Bot {
public:
    Raster_Bot() = default;

    bool begin();

    Raster_Display display;
    Raster_Battery battery;
    Raster_Drive   drive;

private:
};
