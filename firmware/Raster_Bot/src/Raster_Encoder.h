#pragma once

#include <Arduino.h>
#include <driver/pulse_cnt.h>
#include "config.h"

// Hardware quadrature encoder using the ESP32 PCNT peripheral.
// Supports X2 or X4 decoding based on ENCODER_MODE in config.h.
class Raster_Encoder {
public:
    Raster_Encoder() = default;

    // Prevent copies and assignments (each instance owns a unique PCNT unit and spinlock)
    Raster_Encoder(const Raster_Encoder&) = delete;
    Raster_Encoder& operator=(const Raster_Encoder&) = delete;

    bool begin(uint8_t pinA, uint8_t pinB, bool flip = false);
    int64_t getCount() const;
    void clearCount();

private:
    // Limits for the encoder count (to prevent overflow)
    static const int16_t LOW_LIMIT  = -30000;
    static const int16_t HIGH_LIMIT =  30000;

    // PCNT unit handle
    pcnt_unit_handle_t _unit = NULL;

    // Flag to indicate if the encoder is attached to the hardware
    bool _attached = false;

    // Software accumulator extends the 16-bit hardware counter to 64 bits
    volatile int64_t _accumulator = 0;

    // Spinlock to prevent race conditions when accessing the encoder count
    mutable portMUX_TYPE _spinlock = portMUX_INITIALIZER_UNLOCKED;

    // Callback function for when the encoder reaches a limit
    static bool IRAM_ATTR _onReach(pcnt_unit_handle_t unit,
                                   const pcnt_watch_event_data_t *event,
                                   void *ctx);
};
