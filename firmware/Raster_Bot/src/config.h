#pragma once

// =============================================================================
// Raster_Bot Configuration
// =============================================================================

// Pin Definitions
// -----------------------------------------------------------------------------
#define TFT_SPI_HOST FSPI // Hardware SPI peripheral to use
#define TFT_CS    14
#define TFT_DC    12
#define TFT_RST   13
#define TFT_LITE  9
#define TFT_CLK   10   // SPI SCK for TFT
#define TFT_MOSI  11   // SPI MOSI for TFT
#define TFT_MISO -1   // SPI MISO (unused by display)

#define TOUCH_I2C_HOST 0  // Hardware I2C peripheral to use
#define TOUCH_SDA  8   // I2C SDA for touchscreen
#define TOUCH_SCL  18   // I2C SCL for touchscreen

// Battery monitor (BQ24072 charger + voltage divider)
#define VBAT_DIV  7   // Analog pin for battery voltage divider (ADC1_6)
#define CHG      17   // BQ24072 CHG pin (active low: LOW = charging)
#define PGOOD    16   // BQ24072 PGOOD pin (active low: LOW = power good / charger connected)

// Voltage divider: V_battery = V_pin * VBAT_DIV_RATIO
// Set to (R1 + R2) / R2 where R1 is battery-to-pin, R2 is pin-to-GND
#define VBAT_DIV_RATIO  2.0f

// Hardware Settings
// -----------------------------------------------------------------------------
#define TFT_DEFAULT_ROTATION   0
#define TFT_DEFAULT_BG_COLOR   ILI9341_BLACK
#define TFT_DEFAULT_BRIGHTNESS 255

#define TOUCH_THRESHOLD 40

// Feature Flags
// -----------------------------------------------------------------------------
// TODO: Define feature toggles here
