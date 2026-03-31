#include "Raster_Battery.h"

// =============================================================================
// Raster_Battery Implementation
// =============================================================================

void Raster_Battery::begin() {
    pinMode(CHG, INPUT);
    pinMode(PGOOD, INPUT);
    // VBAT_DIV is analog - no pinMode needed on ESP32 (input by default)
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
