#include <Wire.h> // library for I2c LCD
#include <LiquidCrystal_I2C.h> // library for display on LCD
#include "DHT.h" // DHT11 sensor library 

// defining sensor pins
int moisturePin = A0;
int gasPin = A1;
int redLED = 8;
int greenLED = 9;
int buzzer = 13;
int spoilThreshold = 180;

// DHT11 sensor setup
#define DHTPIN 2 // data pin
#define DHTTYPE DHT11 // dht type 
DHT dht(DHTPIN, DHTTYPE); // dht object

// LCD (I2c address, columns, rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool foodSpoiled = false; // tracks food status 

void setup() {
  Serial.begin(9600); starts serial monitor
  // initialize LCD
  lcd.init();
  lcd.backlight();
  dht.begin(); // start dht sensor

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

// displays startup message
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Warming MQ4");
  lcd.setCursor(0,1);
  lcd.print("sensor..."); // waits 30 seconds for gas sensor to warm up to give accurate readings 
  delay(30000);
}

void loop() {
// loop to read each sensor
  int moisture = analogRead(moisturePin);
  int gasLevel = analogRead(gasPin);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

// checks if sensor readings are valid and also sets it back to 0 if the reading fails 
  if (isnan(temp)) temp = 0;
  if (isnan(hum)) hum = 0;

// using the score system we calculate the spoilage score
  int score = 0;

  // moisture scoring (lower value = wetter )
  if (moisture < 260) score += 2; // very wet
  else if (moisture < 290) score += 1; // moderately wet / not wet 

  // gas scoring ( higher value = more gas )
  if (gasLevel > spoilThreshold + 5) score += 3; // high gas
  else if (gasLevel > spoilThreshold) score += 2; // moderate gas  

  // humidity scoring
  if (hum > 80) score += 1; // high humidity

  // determines if the food is spoiled (score 4 or more = spoiled)
  foodSpoiled = (score >= 4);

  // Screen 1: displays moisture level
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Moisture Level:");
  lcd.setCursor(0,1);
  lcd.print(moisture);
  Serial.print("Moisture Level: "); Serial.println(moisture);
  blinkLEDsAndBuzzer(foodSpoiled); // updates leds and buzzer
  delay(500); // displays for 0.5 seconds

  // Screen 2: displays temperature & humidity 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp:"); lcd.print(temp); lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Hum:"); lcd.print(hum); lcd.print("%");
  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" C  Humidity: "); Serial.println(hum);
  blinkLEDsAndBuzzer(foodSpoiled);
  delay(500);

  // Screen 3: displays gas levels
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Gas Level:");
  lcd.setCursor(0,1);
  lcd.print(gasLevel);
  Serial.print("Gas Level: "); Serial.println(gasLevel);
  blinkLEDsAndBuzzer(foodSpoiled);
  delay(500);

  // Screen 4: displays food status
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

// function to control leds and buzzer based on the status of the food
void blinkLEDsAndBuzzer(bool spoiled) {
  if (spoiled) {
    // if spoiled, red led, the buzzer is on while the green led is off
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzer, HIGH);
    delay(1000);
    // turns off red led and buzzer
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
    delay(1000); // stays on for 1 second
  } else {
    // if fresh, green led is on while the red led and buzzer is off
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
    delay(1000);
    // turns off the green led
    digitalWrite(greenLED, LOW);
    delay(1000);
  }
}
