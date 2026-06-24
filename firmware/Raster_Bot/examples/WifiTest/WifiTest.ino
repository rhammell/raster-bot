/*
 * WifiTest - WiFi connectivity test for Raster_Bot library
 *
 * Connects to a WiFi network and serves a live status page at the
 * bot's IP address. The page shows battery voltage, charging state,
 * uptime, and signal strength, refreshing automatically via a
 * /status JSON endpoint (no full-page reload).
 *
 * The TFT displays the connection status and the URL to visit.
 * Serial mirrors all status updates.
 *
 * Setup: edit WIFI_SSID and WIFI_PASS below, then upload.
 *
 * Screen: 240x320 (Portrait, Rotation 0)
 */

#include <Raster_Bot.h>
#include <WiFi.h>
#include <WebServer.h>
#include "wifi_page.h"

Raster_Bot bot;
WebServer server(80);

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";

// Print a centered string at a given Y position
void printCentered(const char* text, int y, uint16_t color, uint8_t size = 2) {
  int w = strlen(text) * 6 * size;
  bot.display.setTextSize(size);
  bot.display.setTextColor(color);
  bot.display.setCursor((240 - w) / 2, y);
  bot.display.print(text);
}

void drawStaticLayout() {
  bot.display.fillScreen(ILI9341_BLACK);

  bot.display.setTextColor(ILI9341_CYAN);
  bot.display.setTextSize(2);
  bot.display.setCursor(10, 10);
  bot.display.print("WiFi Test");
  bot.display.setTextColor(ILI9341_WHITE);
  bot.display.setTextSize(1);
  bot.display.setCursor(10, 30);
  bot.display.print("Raster Bot");
}

void showConnecting() {
  bot.display.fillRect(0, 80, 240, 160, ILI9341_BLACK);
  printCentered("Connecting...", 120, ILI9341_YELLOW);

  char buf[28];
  snprintf(buf, sizeof(buf), "%s", WIFI_SSID);
  printCentered(buf, 150, ILI9341_WHITE);
}

void showConnected() {
  bot.display.fillRect(0, 80, 240, 160, ILI9341_BLACK);
  printCentered("Connected!", 85, ILI9341_GREEN);

  char ipBuf[20];
  snprintf(ipBuf, sizeof(ipBuf), "%s", WiFi.localIP().toString().c_str());
  printCentered(ipBuf, 130, ILI9341_WHITE, 2);

  printCentered("Open in browser", 175, ILI9341_DARKGREY);

  char rssiBuf[20];
  snprintf(rssiBuf, sizeof(rssiBuf), "RSSI: %d dBm", WiFi.RSSI());
  printCentered(rssiBuf, 210, ILI9341_DARKGREY);
}

void showDisconnected() {
  bot.display.fillRect(0, 80, 240, 160, ILI9341_BLACK);
  printCentered("Disconnected", 120, ILI9341_RED);
  printCentered("Retrying...", 150, ILI9341_YELLOW);
}

void handleRoot() {
  server.send_P(200, "text/html", PAGE_HTML);
}

void handleStatus() {
  char json[128];
  snprintf(json, sizeof(json),
    "{\"uptime_s\":%lu,\"voltage\":%.2f,\"usb\":%s,\"charging\":%s,\"rssi\":%d}",
    (unsigned long)(millis() / 1000),
    bot.battery.voltage(),
    bot.battery.chargerConnected() ? "true" : "false",
    bot.battery.charging() ? "true" : "false",
    WiFi.RSSI()
  );
  server.send(200, "application/json", json);
}

void connectWiFi() {
  showConnecting();
  Serial.print("[WiFi] Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint32_t startMs = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startMs < 15000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WiFi] Connected, IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    showConnected();
  } else {
    Serial.println("[WiFi] Connection failed");
    showDisconnected();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Raster Bot WiFi Test");

  if (!bot.begin()) {
    Serial.println("Failed to initialize Raster Bot!");
    while (1);
  }
  Serial.println("Raster Bot initialized successfully");

  bot.display.showSplash();
  drawStaticLayout();

  connectWiFi();

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("[WiFi] HTTP server started on port 80");
}

void loop() {
  server.handleClient();

  // Reconnect if WiFi drops.
  static uint32_t lastCheckMs = 0;
  if (millis() - lastCheckMs >= 5000) {
    lastCheckMs = millis();

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WiFi] Connection lost, reconnecting...");
      showDisconnected();
      connectWiFi();
    }
  }
}
