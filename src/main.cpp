#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
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

String header;
String outputIN1State = "off";
String outputIN3State ="off";

unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
unsigned long timeoutMillis = 2000;

WiFiManager wm;
WiFiServer server(80);

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

void buildServer() {
  WiFiClient client = server.available();
  
  if (client) {
    currentMillis = millis();
    previousMillis = currentMillis;
    Serial.println("New Client.");
    String currentLine = "";

    while(client.connected() && currentMillis - previousMillis <= timeoutMillis) {
      currentMillis = millis();

      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /D1/high") >= 0) {
              Serial.println("IN1 (D1) on");
              outputIN1State = "on";
              digitalWrite(IN1, HIGH);
              analogWrite(ENA, 700);
            } else if (header.indexOf("GET /D1/low") >= 0) {
              Serial.println("IN1 (D1) off");
              outputIN1State = "off";
              digitalWrite(IN1, LOW);
              analogWrite(ENA, 0);
            } else if (header.indexOf("GET /D3/high") >= 0) {
              Serial.println("IN3 (D3) on");
              outputIN3State = "on";
              digitalWrite(IN3, HIGH);
              analogWrite(ENB, 700);
            } else if (header.indexOf("GET /D3/low") >= 0) {
              Serial.println("IN3 (D3) off");
              outputIN3State = "off";
              digitalWrite(IN3, LOW);
              analogWrite(ENB, 0);
            }

            // HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta charset=\"UTF-8\">");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<title>RC Car Controller</title>");
            
            // CSS
            client.println("<style> html { font-family: Arial, Helvetica, sans-serif; display: inline-block; margin: 0px auto; text-align: center; }");
            client.println(".motor-btn { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 2rem; margin: 2px; cursor: pointer; }");
            client.println(".motor-btn2 { background-color: #555555; } </style>");
            
            // Body
            client.println("<body><h1>RC Car Controller</h1>");
            client.println("<p>Motor 1 - State</p>");
            client.println("<p>IN1 - State" + outputIN1State + "</p>");
            if (outputIN1State == "off") {
              client.println("<p><a href=\"/D1/low\"><button class=\"motor-btn\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/D1/off\"><button class=\"motor-btn\">ON</button></a></p>");
            }
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
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
  buildServer();
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