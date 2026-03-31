#include "Raster_Bot.h"

// =============================================================================
// Raster_Bot Implementation
// =============================================================================

Raster_Bot::Raster_Bot() {
}

bool Raster_Bot::begin() {
    if (!display.begin()) {
        return false;
    }

    battery.begin();

    return true;
}

void Raster_Bot::update() {
    // TODO: Handle any periodic tasks
}
