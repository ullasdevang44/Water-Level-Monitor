#define BLYNK_TEMPLATE_ID "TMPL3g7D9CgMe"
#define BLYNK_TEMPLATE_NAME "Water Level Monitor"
#define BLYNK_AUTH_TOKEN "xYVQfDR-1koBCU9IER4YkIXJP0BspS-0"

#include <WiFi.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic sensor pins for Pump 1 (automatic)
const int TRIG_PIN = 18;
const int ECHO_PIN = 19;

// Motor driver pins for Pump 1
const int MOTOR1_IN1 = 12;  // ESP32 GPIO14 (IN1)
const int MOTOR1_IN2 = 14;  // ESP32 GPIO12 (IN2)

// Motor driver pins for Pump 2 (manual on/off)
const int MOTOR2_IN3 = 27;  // ESP32 GPIO13 (IN3)
const int MOTOR2_IN4 = 26;  // ESP32 GPIO27 (IN4)

// Switch for Pump 2
const int PUMP2_SWITCH_PIN = 26; // ESP32 GPIO26

// Tank calibration for 15cm tank
const int MIN_DISTANCE_CM = 4;   // Tank full (sensor near water surface)
const int MAX_DISTANCE_CM = 15;  // Tank empty (sensor at tank bottom)

int waterLevelPercentage = 0;
int lastDisplayedPercentage = -1;

// Flag to distinguish manual control from auto for Pump 1 via Blynk
bool pump1ManualControl = false;
int pump1BlynkState = 0;

BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR2_IN3, OUTPUT);
  pinMode(MOTOR2_IN4, OUTPUT);
  pinMode(PUMP2_SWITCH_PIN, INPUT_PULLUP);

  // Motors off initially
  digitalWrite(MOTOR1_IN1, LOW);
  digitalWrite(MOTOR1_IN2, LOW);
  digitalWrite(MOTOR2_IN3, LOW);
  digitalWrite(MOTOR2_IN4, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  delay(2000);

  WiFiManager wifiManager;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Config");
  lcd.setCursor(0, 1);
  lcd.print("AP: WaterLevel");
  delay(1500);
  wifiManager.autoConnect("WaterLevel");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  delay(1000);

  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());

  // Timer task to send water level every 1 second
  timer.setInterval(1000L, sendWaterLevel);
}

// Blynk Write event for virtual pin V0 for pump control
BLYNK_WRITE(V0) {
  pump1BlynkState = param.asInt(); // 0 or 1
  if (pump1BlynkState == 1) {
    // Turn Pump 1 ON manually from Blynk
    digitalWrite(MOTOR1_IN1, HIGH);
    digitalWrite(MOTOR1_IN2, LOW);
    pump1ManualControl = true;
    Serial.println("Pump 1 ON (Manual Blynk)");
  } else {
    // Turn Pump 1 OFF manually from Blynk
    digitalWrite(MOTOR1_IN1, LOW);
    digitalWrite(MOTOR1_IN2, LOW);
    pump1ManualControl = false;
    Serial.println("Pump 1 OFF (Manual Blynk)");
  }
}

void sendWaterLevel() {
  Blynk.virtualWrite(V1, waterLevelPercentage);
}

void loop() {
  Blynk.run();
  timer.run();

  long pulseDuration;
  int distanceCm;

  // Measure ultrasonic distance
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  pulseDuration = pulseIn(ECHO_PIN, HIGH, 30000);
  distanceCm = pulseDuration * 0.034 / 2;

  waterLevelPercentage = map(distanceCm, MIN_DISTANCE_CM, MAX_DISTANCE_CM, 100, 0);
  waterLevelPercentage = constrain(waterLevelPercentage, 0, 100);

  // Display water level on LCD if changed
  if (waterLevelPercentage != lastDisplayedPercentage) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water Level");
    lcd.setCursor(0, 1);
    lcd.print(waterLevelPercentage);
    lcd.print("%");
    lastDisplayedPercentage = waterLevelPercentage;
  }
  
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.print(" cm, Water Level: ");
  Serial.print(waterLevelPercentage);
  Serial.println("%");

  // Pump 1 control: only automatic if NOT manual override via Blynk
  if (!pump1ManualControl) {
    if (waterLevelPercentage < 10) {
      digitalWrite(MOTOR1_IN1, HIGH);
      digitalWrite(MOTOR1_IN2, LOW);
      Serial.println("Pump 1 ON (Auto)");
    } else if (waterLevelPercentage >= 95) {
      digitalWrite(MOTOR1_IN1, LOW);
      digitalWrite(MOTOR1_IN2, LOW);
      Serial.println("Pump 1 OFF (Auto)");
    }
  }

  // Pump 2 logic remains manual switch locally
  if (digitalRead(PUMP2_SWITCH_PIN) == LOW) {
    digitalWrite(MOTOR2_IN3, HIGH);
    digitalWrite(MOTOR2_IN4, LOW);
    Serial.println("Pump 2 ON (Switch)");
  } else {
    digitalWrite(MOTOR2_IN3, LOW);
    digitalWrite(MOTOR2_IN4, LOW);
    Serial.println("Pump 2 OFF (Switch)");
  }

  delay(500);
}
