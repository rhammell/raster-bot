#include "Raster_Bot.h"

// =============================================================================
// Raster_Bot Implementation
// =============================================================================

bool Raster_Bot::begin() {
    // Initialize TFT display, backlight, and touchscreen
    if (!display.begin()) {
        return false;
    }

    // Initialize battery voltage and charger status monitoring
    battery.begin();

    // Initialize motors, encoders, and drive controller
    if (!drive.begin()) {
        return false;
    }

    return true;
}
