#include "Raster_Encoder.h"

// =============================================================================
// Raster_Encoder Implementation
// =============================================================================

bool Raster_Encoder::begin(uint8_t pinA, uint8_t pinB, bool flip) {
    // If the encoder is already attached, return false
    if (_attached) return false;

    // Swap A/B pins to reverse count direction so +PWM = +count
    if (flip) {
        uint8_t tmp = pinA;
        pinA = pinB;
        pinB = tmp;
    }

    // Create PCNT unit with defined hardware limits
    pcnt_unit_config_t unit_config = {
        .low_limit = LOW_LIMIT,
        .high_limit = HIGH_LIMIT,
    };
    if (pcnt_new_unit(&unit_config, &_unit) != ESP_OK) {
        return false;
    }

    // Enable glitch filter to ignore noise spikes shorter than 1us
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    if (pcnt_unit_set_glitch_filter(_unit, &filter_config) != ESP_OK) {
        return false;
    }

    // Trigger an internal event when the counter hits its limits
    if (pcnt_unit_add_watch_point(_unit, HIGH_LIMIT) != ESP_OK) {
        return false;
    }
    if (pcnt_unit_add_watch_point(_unit, LOW_LIMIT) != ESP_OK) {
        return false;
    }

    // Register ISR callback for when a watch point is reached
    pcnt_event_callbacks_t callbacks = { .on_reach = _onReach };
    if (pcnt_unit_register_event_callbacks(_unit, &callbacks, this) != ESP_OK) {
        return false;
    }

    // Channel A: watches pinA edges, pinB level for direction
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = pinA,
        .level_gpio_num = pinB,
    };
    pcnt_channel_handle_t chan_a = NULL;
    if (pcnt_new_channel(_unit, &chan_a_config, &chan_a) != ESP_OK) {
        return false;
    }

    // Set the edge and level actions for Channel A
    if (pcnt_channel_set_edge_action(chan_a, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE) != ESP_OK) {
        return false;
    }
    if (pcnt_channel_set_level_action(chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE) != ESP_OK) {
        return false;
    }

    // If X4 quadrature mode, set up Channel B
    if (ENCODER_MODE == 4) {
        // Channel B: watches pinB edges, pinA level for direction
        pcnt_chan_config_t chan_b_config = {
            .edge_gpio_num = pinB,
            .level_gpio_num = pinA,
        };
        pcnt_channel_handle_t chan_b = NULL;
        if (pcnt_new_channel(_unit, &chan_b_config, &chan_b) != ESP_OK) {
            return false;
        }

        // Set the edge and level actions for Channel B - reversed for 90-degree offset
        if (pcnt_channel_set_edge_action(chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE) != ESP_OK) {
            return false;
        }
        if (pcnt_channel_set_level_action(chan_b, PCNT_CHANNEL_LEVEL_ACTION_INVERSE, PCNT_CHANNEL_LEVEL_ACTION_KEEP) != ESP_OK) {
            return false;
        }
    }

    // Enable, clear, and start the PCNT unit
    if (pcnt_unit_enable(_unit) != ESP_OK) return false;
    if (pcnt_unit_clear_count(_unit) != ESP_OK) return false;
    if (pcnt_unit_start(_unit) != ESP_OK) return false;

    // Set the attached flag to true
    _attached = true;

    return true;
}

void Raster_Encoder::clearCount() {
    // If the encoder is not attached, return
    if (!_attached) return;

    // Acquire the spinlock to prevent race conditions
    portENTER_CRITICAL(&_spinlock);
    
    // Clear the software accumulator and hardware counter
    _accumulator = 0;
    pcnt_unit_clear_count(_unit);

    // Release the spinlock
    portEXIT_CRITICAL(&_spinlock);
}

int64_t Raster_Encoder::getCount() const {
    // If the encoder is not attached, return 0
    if (!_attached) return 0;

    // Initialize the hardware counter and software accumulator
    int hw_count = 0;
    int64_t acc = 0;

    // Acquire the spinlock to prevent race conditions 
    portENTER_CRITICAL(&_spinlock);

    // Get the hardware counter and software accumulator
    pcnt_unit_get_count(_unit, &hw_count);
    acc = _accumulator;

    // Release the spinlock
    portEXIT_CRITICAL(&_spinlock);

    // Return the total count
    return acc + hw_count;
}

// ISR called when the hardware counter hits a watch point.
// The hardware resets the counter to 0 automatically.
bool IRAM_ATTR Raster_Encoder::_onReach(pcnt_unit_handle_t unit,
                                        const pcnt_watch_event_data_t *event,
                                        void *ctx) {

    // Cast the context to a Raster_Encoder pointer
    Raster_Encoder *self = static_cast<Raster_Encoder *>(ctx);

    // Acquire the spinlock to prevent race conditions
    portENTER_CRITICAL_ISR(&self->_spinlock);

    // Add the watch point value to the software accumulator
    self->_accumulator += event->watch_point_value;

    // Release the spinlock
    portEXIT_CRITICAL_ISR(&self->_spinlock);

    return false;
}
