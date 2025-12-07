#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <Wire.h>

#include "secrets.h"

constexpr int IN1 = D1; // Motor 1 Forward
constexpr int IN2 = D2; // Motor 1 Backward
constexpr int IN3 = D5; // Motor 2 Forward
constexpr int IN4 = D6; // Motor 2 Backward

constexpr int ENA = D7; // Motor 1 Speed
constexpr int ENB = D8; // Motor 2 Speed

constexpr int TRIG_PIN = 21;
constexpr int ECHO_PIN = 19;

float duration_us, distance_cm;

String header;

String motor1State = "stop";
String motor2State = "stop";

// unsigned long currentMillis = millis();
// unsigned long previousMillis = 0;
// unsigned long timeoutMillis = 2000;

ESP8266WebServer server(80);
WiFiManager wm;

void initWifi() {
  // ONLY RUN that sh once unless you changing wifi
  // wm.resetSettings();
  if (!wm.autoConnect("RC-Car-Network")) {
    Serial.println("Couldn't connect: Restarting...");
    ESP.restart();
  }

  if (MDNS.begin("ruckus")) {
    Serial.println("Open at: http://ruckus.local"); // RUCKUS!!!
  }

  server.on("/api/drive", HTTP_POST, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST");
    
    if (!server.hasArg("plain")) {
      server.send(400, "text/plain", "Could not fetch any data! :(");
    };

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, server.arg("plain"));
    if (err) {
      server.send(400, "text/plain", "Bad JSON");
      return;
    }
  });

  Serial.println("Connected!");
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());
}

void joystick() {
  String html = R"=====(
  )=====";
  server.send(200, "text/html", html);
}

void handleRoot() {
  String html = "Ruckus RC Car API Online\nGo to http://ruckus.local to start driving!";
  server.send(200, "text/html", html);
}

void m1_fwd() {
  Serial.println("Motor 1 Forward");
  motor1State = "forward";
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 700);
  handleRoot();
}

void m1_rev() {
  Serial.println("Motor 1 Backward");
  motor1State = "backward";
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 700);
  handleRoot();
}

void m1_stop() {
  Serial.println("Motor 1 Stop");
  motor1State = "stop";
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  handleRoot();
}
void m2_fwd() {
  Serial.println("Motor 2 Forward");
  motor2State = "forward";
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 700);
  handleRoot();
}
void m2_rev() {
  Serial.println("Motor 2 Reverse");
  motor2State = "reverse";
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 700);
  handleRoot();
}
void m2_stop() {
  Serial.println("Motor 2 Stop");
  motor2State = "stop";
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
  handleRoot();
}

void initRoute() {
  server.on("/", []() { handleRoot; });
  // server.on("/m1/fwd", HTTP_GET, m1_fwd);
  // server.on("/m1/rev", HTTP_GET, m1_rev);
  // server.on("/m1/stop", HTTP_GET, m1_stop);
  // server.on("/m2/fwd", HTTP_GET, m2_fwd);
  // server.on("/m2/rev", HTTP_GET, m2_rev);
  // server.on("/m2/stop", HTTP_GET, m2_stop);

  server.begin();
  Serial.println("API Online > http://ruckus.local");
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
  
  initMotor();
  initWifi();
  initRoute();
  // pinMode(TRIG_PIN, OUTPUT);
  // pinMode(ECHO_PIN, INPUT);
}

void loop() {
  server.handleClient();
  MDNS.update();
  // loopMotor();
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