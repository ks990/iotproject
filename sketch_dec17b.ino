#define BLYNK_TEMPLATE_ID "TMPL2M9UB5jU0"
#define BLYNK_TEMPLATE_NAME "LIVESTOCK"
#define BLYNK_AUTH_TOKEN "N1hpw5mriYUFp3jb1WGwsny7hijVSFX4"
char auth[] = "N1hpw5mriYUFp3jb1WGwsny7hijVSFX4";

#include <LiquidCrystal.h>
#include <BlynkSimpleStream.h>

// Constants for sensors and actuators
const int tempSensorPin = A0;
const int foodTrigPin = 7;
const int foodEchoPin = 6;
const int waterTrigPin = 10;
const int waterEchoPin = 9;
const int rs1 = 12, en1 = 11, d41 = 5, d51 = 4, d61 = 3, d71 = 2; // LCD 1 pins
const int rs2 = A2, en2 = A1, d42 = A4, d52 = A5, d62 = 13, d72 = A3; // LCD 2 pins, D72 on A3
const int containerHeight = 200;
const int waterContainerHeight = 100;



// Objects
LiquidCrystal lcd1(rs1, en1, d41, d51, d61, d71); // LCD for Food/Water
LiquidCrystal lcd2(rs2, en2, d42, d52, d62, d72); // LCD for Temperature/Status

BlynkTimer timer;
const unsigned long interval = 10000;

void setup() {
  Serial.begin(9600);
  Blynk.begin(Serial, auth);
  pinMode(foodTrigPin, OUTPUT);
  pinMode(foodEchoPin, INPUT);
  pinMode(waterTrigPin, OUTPUT);
  pinMode(waterEchoPin, INPUT);
  pinMode(13, OUTPUT); //Setting the pin 13 as output

  lcd1.begin(16, 2);
  lcd1.print("Food:   ");
  lcd1.setCursor(0, 1);
  lcd1.print("Water:   ");

  lcd2.begin(16, 2);
  lcd2.print("Temp:  ");
  lcd2.setCursor(0, 1);
  lcd2.print("ST:    ");

  timer.setInterval(interval, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendSensorData() {
  // 1. Temperature Reading (LM35)
  int tempReading = analogRead(tempSensorPin);
  int temperatureC = (tempReading * 5.0 / 1024.0) * 100.0;

  Blynk.virtualWrite(V1, temperatureC);

  // Displaying the temperature on the second LCD
  lcd2.setCursor(6, 0);
  lcd2.print(" "); // Clear previous value
  lcd2.setCursor(6, 0);
  lcd2.print(temperatureC);
  lcd2.print(" C");

  // Displaying the status on the second LCD
  lcd2.setCursor(3, 1);
  lcd2.print("           "); // Clear previous status (longer for potential longer text)
  lcd2.setCursor(3, 1);
  if (temperatureC > 30) {
    lcd2.print("Too hot       ");
  } else if (temperatureC < 15) {
    lcd2.print("Too cold      ");
  } else if (temperatureC >= 20 && temperatureC <= 28) {
    lcd2.print("Normal      ");
  }

  // 2. Water Level Reading (Ultrasonic)
  long durationWater, distanceWater;
  digitalWrite(waterTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(waterTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(waterTrigPin, LOW);
  durationWater = pulseIn(waterEchoPin, HIGH);
  distanceWater = durationWater * 0.034 / 2;
  int waterLevel = distanceWater;

  if (durationWater == 0) {
    lcd1.setCursor(7, 1);
    lcd1.print("Error!     ");
  } else {
    distanceWater = durationWater * 0.034 / 2;
    int waterLevel = distanceWater;

    Blynk.virtualWrite(V3, waterLevel);

    lcd1.setCursor(7, 1);
    if (waterLevel < 20) {
      lcd1.print("Low    ");
    } else {
      lcd1.print("Available   ");
    }
  }

  // 3. Food Level Reading (Ultrasonic)
  long duration, distance;
  digitalWrite(foodTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(foodTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(foodTrigPin, LOW);
  duration = pulseIn(foodEchoPin, HIGH);
  distance = duration * 0.034 / 2;
  int foodLevel = distance;

  if (duration == 0) {
    lcd1.setCursor(5, 0);
  } else {
    distance = duration * 0.034 / 2;
    int foodLevel = distance;
    Blynk.virtualWrite(V2, foodLevel);

    lcd1.setCursor(0, 0);
    lcd1.print("Food:     ");

    if (foodLevel <= 0) {
      lcd1.setCursor(5, 0);
      lcd1.print("EMPTY!  ");
    } else if (foodLevel < 80 && foodLevel > 0) {
      lcd1.setCursor(5, 0);
      lcd1.print("Low    ");
    } else {
      lcd1.setCursor(5, 0);
      lcd1.print("Normal   ");
    }
  }
  Blynk.virtualWrite(V1, temperatureC);
  Blynk.virtualWrite(V2, foodLevel);
  Blynk.virtualWrite(V3, waterLevel);
  delay(100);
}