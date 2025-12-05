#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <Wire.h>

#include "secrets.h"

constexpr int IN1 = D1;
constexpr int IN2 = D2;
constexpr int IN3 = D5;
constexpr int IN4 = D6;

constexpr int ENA = D7;
constexpr int ENB = D8;

constexpr int TRIG_PIN = 21;
constexpr int ECHO_PIN = 19;

float duration_us, distance_cm;

unsigned long prevMillis = 0;
unsigned long interval = 30000;

WiFiManager wm;

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

void initMotor() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loopMotor() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 700);
  analogWrite(ENB, 700);
  delay(200);
  
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(1000);
}

void setup() {
  Serial.begin(115200);

  initWifi();
  initMotor();
  // pinMode(TRIG_PIN, OUTPUT);
  // pinMode(ECHO_PIN, INPUT);
}

void loop() {
  loopMotor();
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