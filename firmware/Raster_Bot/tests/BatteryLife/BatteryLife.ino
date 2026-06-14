/*
 * BatteryLife - Battery life logging test for Raster_Bot library
 *
 * Drives the bot back and forth at half speed while periodically
 * sampling the battery voltage and POSTing each reading to a server
 * on the local network. The companion server.py logs every reading
 * with a timestamp so battery runtime can be measured over hours.
 *
 * Motion cycle (repeats forever):
 *   1. Forward at half speed for 20 s
 *   2. Pause (brake) briefly
 *   3. Reverse at half speed for 20 s
 *   4. Pause (brake) briefly
 *
 * Every 60 s a battery voltage sample is POSTed to SERVER_URL as JSON.
 *
 * The TFT shows the elapsed run time, the latest battery voltage, and a
 * scrolling log of POST attempts (yellow), successes (green), and
 * failures (red).
 *
 * Setup: edit WIFI_SSID / WIFI_PASS and SERVER_URL below, then run
 * server.py (in this folder) on a machine on the same network.
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>
#include <WiFi.h>
#include <HTTPClient.h>

Raster_Bot bot;

// --- Network configuration ---
const char* WIFI_SSID = "AlvinsHouse";
const char* WIFI_PASS = "Hiyadumdum";

// Address of the logging server. Replace the IP with the address of the
// machine running server.py (shown when the server starts up).
const char* SERVER_URL = "http://192.168.50.105:8080/log";

// --- Motion configuration ---
const float DRIVE_SPEED_CM_S = MAX_SPEED_CM_S / 2.0f;  // Half of full speed
const uint32_t DRIVE_MS      = 20000;  // Time spent driving in each direction
const uint32_t PAUSE_MS      = 2000;   // Pause between direction changes

// --- Logging configuration ---
const uint32_t LOG_INTERVAL_MS = 60000;  // Sample/POST the battery once a minute

// Motion state machine
enum MotionPhase { FORWARD, PAUSE_AFTER_FORWARD, REVERSE, PAUSE_AFTER_REVERSE };
MotionPhase motionPhase = FORWARD;
uint32_t phaseStartMs = 0;

uint32_t lastLogMs = 0;

// --- Display layout (240x320 portrait) ---
const int16_t HEADER_VALUE_X = 78;   // X where the time/voltage values start
const int16_t TIME_Y         = 34;   // Y of the elapsed-time line
const int16_t VBAT_Y         = 58;   // Y of the voltage line
const int16_t MSG_AREA_Y     = 92;   // Y where the message log begins
const int16_t MSG_LINE_H     = 22;   // Pixel height of one message line
const uint8_t MSG_COUNT      = 10;   // Number of messages kept on screen

// Circular buffer of recent log messages shown at the bottom of the screen.
struct LogLine {
    char     text[28];
    uint16_t color;
};
LogLine  msgLog[MSG_COUNT];
uint8_t  msgHead  = 0;   // Index of the oldest message in the buffer
uint8_t  msgUsed  = 0;   // Number of messages currently stored
uint32_t bootMs   = 0;   // millis() at the moment logging started

// Redraw the message log area from the circular buffer (newest at the bottom).
void redrawMessages() {
    bot.display.fillRect(0, MSG_AREA_Y, bot.display.width(),
                         bot.display.height() - MSG_AREA_Y, ILI9341_BLACK);
    bot.display.setTextSize(2);
    for (uint8_t i = 0; i < msgUsed; i++) {
        uint8_t idx = (msgHead + i) % MSG_COUNT;
        bot.display.setCursor(4, MSG_AREA_Y + i * MSG_LINE_H);
        bot.display.setTextColor(msgLog[idx].color);
        bot.display.print(msgLog[idx].text);
    }
}

// Append a message to the on-screen log (and mirror it to Serial).
void addMessage(const char* text, uint16_t color) {
    uint8_t idx;
    if (msgUsed < MSG_COUNT) {
        idx = (msgHead + msgUsed) % MSG_COUNT;
        msgUsed++;
    } else {
        idx = msgHead;
        msgHead = (msgHead + 1) % MSG_COUNT;
    }
    strncpy(msgLog[idx].text, text, sizeof(msgLog[idx].text) - 1);
    msgLog[idx].text[sizeof(msgLog[idx].text) - 1] = '\0';
    msgLog[idx].color = color;

    Serial.print("[Msg] ");
    Serial.println(text);
    redrawMessages();
}

// Draw the static labels and dividers that never change.
void displayInit() {
    bot.display.fillScreen(ILI9341_BLACK);

    bot.display.setTextSize(2);
    bot.display.setTextColor(ILI9341_CYAN);
    bot.display.setCursor(4, 6);
    bot.display.print("Battery Time");

    bot.display.setTextColor(ILI9341_WHITE);
    bot.display.setCursor(4, TIME_Y);
    bot.display.print("Time:");
    bot.display.setCursor(4, VBAT_Y);
    bot.display.print("Vbat:");

    bot.display.drawFastHLine(0, MSG_AREA_Y - 6, bot.display.width(), ILI9341_DARKGREY);
}

// Update the elapsed-time value (HH:MM:SS) in the header.
void updateElapsedDisplay() {
    uint32_t elapsedS = (millis() - bootMs) / 1000;
    uint32_t h = elapsedS / 3600;
    uint32_t m = (elapsedS % 3600) / 60;
    uint32_t s = elapsedS % 60;

    char buf[16];
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu",
             (unsigned long)h, (unsigned long)m, (unsigned long)s);

    bot.display.fillRect(HEADER_VALUE_X, TIME_Y, bot.display.width() - HEADER_VALUE_X,
                         16, ILI9341_BLACK);
    bot.display.setTextSize(2);
    bot.display.setTextColor(ILI9341_WHITE);
    bot.display.setCursor(HEADER_VALUE_X, TIME_Y);
    bot.display.print(buf);
}

// Update the latest voltage value in the header.
void updateVoltageDisplay(float voltage) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%.3f V", voltage);

    bot.display.fillRect(HEADER_VALUE_X, VBAT_Y, bot.display.width() - HEADER_VALUE_X,
                         16, ILI9341_BLACK);
    bot.display.setTextSize(2);
    bot.display.setTextColor(ILI9341_GREEN);
    bot.display.setCursor(HEADER_VALUE_X, VBAT_Y);
    bot.display.print(buf);
}

void connectWiFi() {
    Serial.print("[WiFi] Connecting to ");
    Serial.println(WIFI_SSID);
    addMessage("WiFi connecting...", ILI9341_YELLOW);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint32_t startMs = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startMs < 20000) {
        delay(250);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("[WiFi] Connected, IP: ");
        Serial.println(WiFi.localIP());
        char buf[28];
        snprintf(buf, sizeof(buf), "WiFi %s", WiFi.localIP().toString().c_str());
        addMessage(buf, ILI9341_GREEN);
    } else {
        Serial.println("[WiFi] Connection FAILED (will keep retrying)");
        addMessage("WiFi FAILED", ILI9341_RED);
    }
}

// POST a single battery voltage reading to the server as JSON.
void postVoltage(float voltage) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[Log] WiFi down, attempting reconnect...");
        connectWiFi();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[Log] Skipping POST (no WiFi)");
            addMessage("POST skipped: no WiFi", ILI9341_RED);
            return;
        }
    }

    char msg[28];
    snprintf(msg, sizeof(msg), "POSTing %.3f V...", voltage);
    addMessage(msg, ILI9341_YELLOW);

    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    char payload[64];
    snprintf(payload, sizeof(payload), "{\"voltage\": %.3f}", voltage);

    int code = http.POST((uint8_t*)payload, strlen(payload));
    Serial.print("[Log] POST ");
    Serial.print(payload);
    Serial.print(" -> HTTP ");
    Serial.println(code);

    if (code > 0 && code < 400) {
        snprintf(msg, sizeof(msg), "POST OK (HTTP %d)", code);
        addMessage(msg, ILI9341_GREEN);
    } else {
        snprintf(msg, sizeof(msg), "POST FAILED (%d)", code);
        addMessage(msg, ILI9341_RED);
    }

    http.end();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Raster Bot Battery Time Test");

    if (!bot.begin()) {
        Serial.println("Failed to initialize Raster Bot!");
        while (1);
    }
    Serial.println("Raster Bot initialized successfully");

    bootMs = millis();
    displayInit();
    updateElapsedDisplay();

    connectWiFi();

    // Take an initial reading right away, then once per minute after.
    float v = bot.battery.voltage();
    Serial.print("[Log] Initial battery voltage: ");
    Serial.println(v, 3);
    updateVoltageDisplay(v);
    postVoltage(v);
    lastLogMs = millis();

    // Start the motion cycle driving forward.
    motionPhase = FORWARD;
    phaseStartMs = millis();
    bot.drive.straight(DRIVE_SPEED_CM_S);
    Serial.println("[Motion] FORWARD");
}

void updateMotion() {
    uint32_t now = millis();

    switch (motionPhase) {
        case FORWARD:
            if (now - phaseStartMs >= DRIVE_MS) {
                bot.drive.stop();
                motionPhase = PAUSE_AFTER_FORWARD;
                phaseStartMs = now;
                Serial.println("[Motion] PAUSE");
            }
            break;

        case PAUSE_AFTER_FORWARD:
            if (now - phaseStartMs >= PAUSE_MS) {
                bot.drive.straight(-DRIVE_SPEED_CM_S);
                motionPhase = REVERSE;
                phaseStartMs = now;
                Serial.println("[Motion] REVERSE");
            }
            break;

        case REVERSE:
            if (now - phaseStartMs >= DRIVE_MS) {
                bot.drive.stop();
                motionPhase = PAUSE_AFTER_REVERSE;
                phaseStartMs = now;
                Serial.println("[Motion] PAUSE");
            }
            break;

        case PAUSE_AFTER_REVERSE:
            if (now - phaseStartMs >= PAUSE_MS) {
                bot.drive.straight(DRIVE_SPEED_CM_S);
                motionPhase = FORWARD;
                phaseStartMs = now;
                Serial.println("[Motion] FORWARD");
            }
            break;
    }
}

void loop() {
    updateMotion();

    // Refresh the elapsed-time display once per second.
    static uint32_t lastClockMs = 0;
    if (millis() - lastClockMs >= 1000) {
        lastClockMs = millis();
        updateElapsedDisplay();
    }

    // Log battery voltage once per minute.
    if (millis() - lastLogMs >= LOG_INTERVAL_MS) {
        lastLogMs = millis();
        float v = bot.battery.voltage();
        Serial.print("[Log] Battery voltage: ");
        Serial.println(v, 3);
        updateVoltageDisplay(v);
        postVoltage(v);
    }

    // Drive subsystem must be updated frequently for PID/ramping to work.
    bot.drive.update();
}
