#pragma once

#include <Arduino.h>
#include "display/Raster_Display.h"
#include "battery/Raster_Battery.h"
#include "drive/Raster_Drive.h"

// Raster Bot class
class Raster_Bot {
    
public:
    // Constructor
    Raster_Bot() = default;

    // Public methods
    bool begin();

    // Public members
    Raster_Display display;
    Raster_Battery battery;
    Raster_Drive   drive;
};
