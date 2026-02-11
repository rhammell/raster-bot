#pragma once

// =============================================================================
// Raster_Bot Configuration
// =============================================================================

// Pin Definitions
// -----------------------------------------------------------------------------
#define TFT_CS    18
#define TFT_DC    3
#define TFT_RST   6
#define TFT_LITE  5
#define TFT_CLK   2   // SPI SCK for TFT
#define TFT_MOSI  1   // SPI MOSI for TFT
#define TFT_MISO -1   // SPI MISO (unused by display)

#define I2C_SDA  13   // I2C SDA for touchscreen
#define I2C_SCL  14   // I2C SCL for touchscreen

// Hardware Settings
// -----------------------------------------------------------------------------
#define TFT_DEFAULT_ROTATION   0
#define TFT_DEFAULT_BG_COLOR   ILI9341_BLACK
#define TFT_DEFAULT_BRIGHTNESS 255

#define TOUCH_THRESHOLD 40
#define TOUCH_WIDTH     240  // Fixed physical width of touch digitizer
#define TOUCH_HEIGHT    320  // Fixed physical height of touch digitizer

// Feature Flags
// -----------------------------------------------------------------------------
// TODO: Define feature toggles here
