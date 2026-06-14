#include "Raster_Battery.h"

// =============================================================================
// Raster_Battery Implementation
// =============================================================================

void Raster_Battery::begin() {
    pinMode(CHG, INPUT);
    pinMode(PGOOD, INPUT);
}

float Raster_Battery::voltage() {
    int mv = analogReadMilliVolts(VBAT_DIV);
    return (mv / 1000.0f) * VBAT_DIV_RATIO;
}

bool Raster_Battery::charging() {
    // BQ24072 CHG is active low: LOW = charging in progress
    return digitalRead(CHG) == LOW;
}

bool Raster_Battery::chargerConnected() {
    // BQ24072 PGOOD is active low: LOW = power good (charger connected)
    return digitalRead(PGOOD) == LOW;
}
