#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial esp(A0, A1);

const uint8_t IN1 = 10, IN2 = 9, ENA = 11;
const uint8_t IN3 = 8, IN4 = 7, ENB = 6;

const uint8_t TRIG1 = 12, ECHO1 = 4;
const uint8_t TRIG2 = 3, ECHO2 = 2;

const int MIN_MAG = 2;
const int DEADZONE = 6;
const int MIN_PWM = 120;
const int MAX_PWM = 1023;

static int mapInput(int mag) {
  if (mag <= MIN_MAG) return 0;
  return map(mag, MIN_MAG, 127, MIN_PWM, MAX_PWM);
}


void driveMotor(int l , int r) {
  auto applyMotor = [](int val, int pin1, int pin2, int en) {
    int mag = abs(val);
    int pwm = mapInput(mag);

    if (pwm == 0) {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      analogWrite(en, 0);
      return;
    }

    if (val > 0) {
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
    } else {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
    }

    analogWrite(en, pwm);

  };

  applyMotor(l, IN1, IN2, ENA);
  applyMotor(r, IN3, IN4, ENB);
}

float getDistance() {
  digitalWrite(TRIG1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG1, LOW);
  long time1 = pulseIn(ECHO1, HIGH, 30000);
  
  digitalWrite(TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG2, LOW);
  long time2 = pulseIn(ECHO2, HIGH, 30000);

  float distance1 = time1 ? 999.0f : time1 * 0.017f;
  float distance2 = time2 ? 999.0f : time2 * 0.017f;

  return min(distance1, distance2);
}

void initSensor() {
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  digitalWrite(TRIG1, LOW);
  digitalWrite(ECHO1, LOW);
  digitalWrite(TRIG2, LOW);
  digitalWrite(ECHO2, LOW);
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

void getCommands() {
  static String buf = "";
  while (esp.available()) {
    char c = esp.read();
    if (c == '\n') {
      buf.trim();
      if (buf.startsWith("JOY:")) {
        int l, r;
        if (sscanf(buf.c_str(), "JOY:%d:%d", &l, &r) == 2) {
          driveMotor(l, r);
        }
      }
      buf = "";
    } else if (c != '\r') {
      buf += c;
    }
  }

  float distance = getDistance();
  if (distance > 2 && distance < 20) {
    driveMotor(0, 0);
  }
  delay(10);
}

void setup() {
  initSensor();
  initMotor();
  driveMotor(0, 0);
  Serial.println("Arduino UNO ONLINE! - Send Commands");
}

void loop() {
  getCommands();
}