#include "Raster_Bot.h"

bool Raster_Bot::begin() {
    // Initialize TFT display, backlight, and touchscreen
    Serial.println("[Bot] Initializing display...");
    if (!display.begin()) {
        Serial.println("[Bot] Display init FAILED");
        return false;
    }
    Serial.println("[Bot] Display init OK");

    // Initialize battery voltage and charger status monitoring
    Serial.println("[Bot] Initializing battery...");
    battery.begin();
    Serial.println("[Bot] Battery init OK");

    // Initialize motors, encoders, and drive controller
    Serial.println("[Bot] Initializing drive...");
    if (!drive.begin()) {
        Serial.println("[Bot] Drive init FAILED");
        return false;
    }
    Serial.println("[Bot] Drive init OK");

    return true;
}
