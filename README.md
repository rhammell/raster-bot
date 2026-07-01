# Raster Bot

![Raster Bot](images/readme/hero-placeholder.svg)

Raster Bot is a compact, two-wheeled differential-drive robot built around an
ESP32 and a capacitive touchscreen display. The project is fully open: it
includes the custom PCB, the 3D-printable mechanical design, the firmware
(packaged as an Arduino library), and the source graphics.

The firmware provides closed-loop motor control with quadrature encoders and
PID speed regulation, battery/charge monitoring, and a touchscreen UI — all
wrapped in a simple high-level API so sketches can drive the bot in a few lines.

## Build guide

A full step-by-step build tutorial — bill of materials, wiring, assembly, and
flashing — is published on Hackster.io:

**[Raster Bot build guide on Hackster.io](https://www.hackster.io/)** <!-- TODO: replace with the real tutorial URL -->

This README focuses on the contents of the repository and the firmware API;
see the Hackster guide if you want to build one from scratch.

## Repository structure

| Folder | Description |
| --- | --- |
| [`firmware/`](firmware/) | ESP32 firmware, packaged as the `Raster_Bot` Arduino library, plus example and tuning sketches. |
| [`hardware/`](hardware/) | KiCad project for the custom PCB (schematic, board layout, and footprint/symbol libraries). |
| [`mechanical/`](mechanical/) | FreeCAD model of the chassis and mechanical parts. |
| [`graphics/`](graphics/) | Logo artwork — SVG source and exported PNGs used for the board silkscreen and the boot splash screen. |

### `firmware/`

The firmware lives in `firmware/Raster_Bot/`, structured as an Arduino library:

| Path | Description |
| --- | --- |
| `src/Raster_Bot.h` / `.cpp` | Top-level `Raster_Bot` class that ties the subsystems together. |
| `src/definitions/` | Compile-time configuration: `pin_definitions.h` (wiring), `robot_definitions.h` (tuning/behavior), and `build_config.h` (build-variant flags). |
| `src/display/` | `Raster_Display` — TFT, capacitive touch, and backlight control (plus the splash logo). |
| `src/battery/` | `Raster_Battery` — battery voltage and charger-status monitoring. |
| `src/drive/` | Differential drive stack: `Raster_Motor`, `Raster_Encoder`, `Raster_PID`, `Raster_Motor_Controller`, and `Raster_Drive`. |
| `examples/` | Standalone feature demos: `BatteryTest`, `GraphicsTest`, `TouchTest`. |
| `tests/` | Tuning sketches: `PIDTuning`, `StraightnessTuning`, `DistanceTuning`. |
| `library.properties` | Arduino library metadata and dependencies. |

**Dependencies** (install via the Arduino Library Manager): Adafruit GFX
Library, Adafruit ILI9341, Adafruit FT6206 Library. Target architecture is
`esp32`.

### `hardware/`

KiCad design for the Raster Bot PCB. Open `hardware/Raster_Bot/Raster_Bot.kicad_pro`
in KiCad to view the schematic and board. Project-local footprint, symbol, and
3D-model libraries are under `hardware/Raster_Bot/libraries/`.

### `mechanical/`

FreeCAD source (`mechanical/src/Raster_Bot.FCStd`) for the robot's mechanical
design, parameterized for resizing.

### `graphics/`

- `src/raster_logo.svg` — master logo artwork.
- `export/raster_logo_silkscreen.png` — logo for the PCB silkscreen.
- `export/raster_logo_splash.png` — logo shown on the display at boot.

## Firmware API

Include the library and create a single `Raster_Bot` instance. It exposes three
subsystems as public members: `display`, `battery`, and `drive`.

```cpp
#include <Raster_Bot.h>

Raster_Bot bot;

void setup() {
    Serial.begin(115200);
    if (!bot.begin()) {
        Serial.println("Raster Bot init failed");
        while (1);
    }
    bot.drive.straight(10.0f);   // drive forward at 10 cm/s
}

void loop() {
    bot.drive.update();          // must be called frequently for PID control
}
```

### `Raster_Bot`

| Method | Description |
| --- | --- |
| `bool begin()` | Initializes the display, battery monitor, and drive subsystems. Returns `false` if any fail. |
| `display` | `Raster_Display` instance (see below). |
| `battery` | `Raster_Battery` instance (see below). |
| `drive` | `Raster_Drive` instance (see below). |

### `Raster_Display`

Extends Adafruit's `Adafruit_ILI9341`, so all standard Adafruit GFX drawing
calls are available in addition to:

| Method | Description |
| --- | --- |
| `bool begin()` | Initializes the TFT, touchscreen, and backlight. |
| `void showSplash()` | Plays the boot splash animation. |
| `void setBrightness(uint8_t brightness)` | Sets backlight brightness (0–255). |
| `bool touched()` | Returns `true` while the screen is being touched. |
| `TS_Point getTouchPoint()` | Returns the current touch point, mapped to the active screen rotation. |

### `Raster_Battery`

| Method | Description |
| --- | --- |
| `void begin()` | Initializes battery/charge-status monitoring. |
| `float voltage()` | Battery voltage in volts. |
| `bool charging()` | `true` while actively charging. |
| `bool chargerConnected()` | `true` when a charging source is plugged in. |

### `Raster_Drive`

Differential drive with per-wheel PID speed control. Call `update()` frequently
from `loop()` to run the control loop.

| Method | Description |
| --- | --- |
| `bool begin()` | Initializes the motor driver, both motor controllers, and encoders. |
| `void straight(float speed_cm_s)` | Drives both wheels at the given speed (negative = reverse). |
| `void stop()` | Stops both motors and goes idle. |
| `void update()` | Runs the drive control loop; call as often as possible. |
| `bool isMoving()` | `true` while a drive command is active. |
| `DriveTelemetry getTelemetry()` | Returns current motor telemetry. |
| `float getDistanceCm()` | Returns cumulative straight-line distance driven since power-on. |
| `void resetDistance()` | Resets the cumulative drive distance to zero. |

`DriveTelemetry` contains: `leftRPM`, `leftPWM`, `rightRPM`, `rightPWM`,
`leftCount`, and `rightCount` (wheel-output RPM and raw encoder counts).

## License

<!-- TODO: add a license (e.g. MIT for firmware, CERN-OHL or CC-BY-SA for hardware). -->
