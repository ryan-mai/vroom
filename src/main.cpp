#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <Wire.h>

#include "secrets.h"

int TRIG_PIN = 21;
int ECHO_PIN = 19;

float duration_us, distance_cm;

unsigned long prevMillis = 0;
unsigned long interval = 30000;

WiFiManager wm;
// void initWifi() {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi...");
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(1000);
//   }
//   Serial.println(WiFi.localIP());
//   Serial.print("RSSI: ");
//   Serial.println(WiFi.RSSI());
// }

void initWifi() {
  // ONLY RUN that sh once unless you changing wifi
  // wm.resetSettings();
  if (!wm.autoConnect("RC-Car-Network")) {
    Serial.println("Couldn't connect: Restarting...");
    ESP.restart();
  }
  Serial.println("Connected!");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.RSSI());
}

// void reconnectWifi() {
//   unsigned long currentMillis = millis();
//   if ((WiFi.status() != WL_CONNECTED) && (currentMillis - prevMillis >= interval)) {
//     Serial.print(millis());
//     Serial.println("Reconnecting to WiFi...");
//     WiFi.disconnect();
//     WiFi.reconnect();
//     prevMillis = currentMillis;
//   }
// }

void setup() {
  Serial.begin(115200);

  initWifi();

  // pinMode(TRIG_PIN, OUTPUT);
  // pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // reconnectWifi();
  // digitalWrite(TRIG_PIN, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(TRIG_PIN, LOW);

  // duration_us = pulseIn(ECHO_PIN, HIGH);
  // distance_cm = 0.017 * duration_us;

  // Serial.print("distance: ");
  // Serial.print(distance_cm);
  // Serial.println(" cm");
}