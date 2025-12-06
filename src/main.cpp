#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
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
  String html = R"=====(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>RC Car Web Server</title>
      <style>
        body { font-family: Arial; text-align: center; padding: 20px; }
        button { font-size: 2em; padding: 20px 40px; margin: 10px; width: 90%; border-radius: 8px; }
        .fwd { background:#4CAF50; color:white; }
        .rev { background:#f44336; color:white; }
        .stop { background:#555; color:white; }
      </style>
    </head>
    <body>
      <h1>RC Car Controller</h1>
      
      <h2>Left Motor: )=====" + motor1State + R"=====(</h2>
      <a href="/m1/fwd"><button class="fwd">FORWARD</button></a>
      <a href="/m1/rev"><button class="rev">REVERSE</button></a>
      <a href="/m1/stop"><button class="stop">STOP</button></a><br><br>
      
      <h2>Right Motor: )=====" + motor2State + R"=====(</h2>
      <a href="/m2/fwd"><button class="fwd">FORWARD</button></a>
      <a href="/m2/rev"><button class="rev">REVERSE</button></a>
      <a href="/m2/stop"><button class="stop">STOP</button></a>
    </body>
    </html>
  )=====";
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
  server.on("/", HTTP_GET, handleRoot);
  server.on("/m1/fwd", HTTP_GET, m1_fwd);
  server.on("/m1/rev", HTTP_GET, m1_rev);
  server.on("/m1/stop", HTTP_GET, m1_stop);
  server.on("/m2/fwd", HTTP_GET, m2_fwd);
  server.on("/m2/rev", HTTP_GET, m2_rev);
  server.on("/m2/stop", HTTP_GET, m2_stop);

  server.begin();
  Serial.println("HTTP server");
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