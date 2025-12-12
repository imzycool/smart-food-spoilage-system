#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// Pins
int moisturePin = A0;
int gasPin = A1;
int redLED = 8;
int greenLED = 9;
int buzzer = 13;
int spoilThreshold = 180;

// DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool foodSpoiled = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Warming MQ4");
  lcd.setCursor(0,1);
  lcd.print("sensor...");
  delay(30000);
}

void loop() {

  int moisture = analogRead(moisturePin);
  int gasLevel = analogRead(gasPin);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp)) temp = 0;
  if (isnan(hum)) hum = 0;

  // ---------- SCORING SYSTEM ----------
  int score = 0;

  // Moisture scoring
  if (moisture < 260) score += 2;
  else if (moisture < 290) score += 1;

  // Gas scoring
  if (gasLevel > spoilThreshold + 5) score += 3;
  else if (gasLevel > spoilThreshold) score += 2;

  // Humidity scoring
  if (hum > 80) score += 1;

  // Final decision
  foodSpoiled = (score >= 4);
  // ------------------------------------


  // Screen 1: Moisture
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Moisture Level:");
  lcd.setCursor(0,1);
  lcd.print(moisture);
  Serial.print("Moisture Level: "); Serial.println(moisture);
  blinkLEDsAndBuzzer(foodSpoiled);
  delay(500);

  // Screen 2: Temp & Humidity
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp:"); lcd.print(temp); lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Hum:"); lcd.print(hum); lcd.print("%");
  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" C  Humidity: "); Serial.println(hum);
  blinkLEDsAndBuzzer(foodSpoiled);
  delay(500);

  // Screen 3: Gas Level
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Gas Level:");
  lcd.setCursor(0,1);
  lcd.print(gasLevel);
  Serial.print("Gas Level: "); Serial.println(gasLevel);
  blinkLEDsAndBuzzer(foodSpoiled);
  delay(500);

  // Screen 4: Food Status
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Food Status:");
  lcd.setCursor(0,1);
  if (foodSpoiled) {
    lcd.print("Spoiled");
    Serial.println("Food Status: Spoiled");
  } else {
    lcd.print("Fresh");
    Serial.println("Food Status: Fresh");
  }
  blinkLEDsAndBuzzer(foodSpoiled);
  delay(500);
}

void blinkLEDsAndBuzzer(bool spoiled) {
  if (spoiled) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
    delay(1000);
  } else {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
    delay(1000);
    digitalWrite(greenLED, LOW);
    delay(1000);
  }
}

