#include <LiquidCrystal_I2C.h>

#include <Adafruit_CircuitPlayground.h>
#include <Adafruit_Circuit_Playground.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;

const int IR1 = 2;    // Entry IR sensor
const int IR2 = 3;    // Exit IR sensor

const int MAX_SLOTS = 4;
int Slot = MAX_SLOTS;

bool gateOpen = false;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);

  myservo.attach(4);
  myservo.write(0);  // Gate closed

  lcd.setCursor(0, 0);
  lcd.print("  ARDUINO PARK  ");
  lcd.setCursor(0, 1);
  lcd.print("   SYSTEM       ");
  delay(2000);
  lcd.clear();

  updateLCD("WELCOME!");
}

void loop() {
  // Car entering
  if (digitalRead(IR1) == LOW && !gateOpen) {
    if (Slot > 0) {
      Serial.println("Car entering detected at IR1");
      openGate();
      waitForSensor(IR2);
      closeGate();

      Slot = max(Slot - 1, 0);
      updateLCD("WELCOME!");
    } else {
      // Parking full - do not open gate, show message
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Parking Full!");
      lcd.setCursor(0, 1);
      lcd.print("No Entry Allowed");
      Serial.println("Parking Full! Gate stays closed.");
      delay(2000);
      updateLCD("");
    }
    delay(1000);
  }

  // Car exiting
  if (digitalRead(IR2) == LOW && !gateOpen) {
    Serial.println("Car exiting detected at IR2");
    openGate();
    waitForSensor(IR1);
    closeGate();

    Slot = min(Slot + 1, MAX_SLOTS);
    updateLCD("BYE!");
    delay(1000);
  }
}

void openGate() {
  myservo.write(90);
  gateOpen = true;
  Serial.println("Gate opened");
  delay(1000);
}

void closeGate() {
  myservo.write(0);
  gateOpen = false;
  Serial.println("Gate closed");
  delay(1000);
}

void waitForSensor(int sensorPin) {
  Serial.print("Waiting for sensor ");
  Serial.println(sensorPin);
  unsigned long startTime = millis();

  while (digitalRead(sensorPin) == HIGH) {
    if (millis() - startTime > 5000) {
      Serial.println("Sensor wait timeout");
      break;
    }
    delay(50);
  }
  delay(500);
}

void updateLCD(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);

  if (Slot == 0) {
    lcd.print("Parking Full!");
  } else {
    lcd.print("Slots Left: ");
    lcd.print(Slot);
  }

  lcd.setCursor(0, 1);
  lcd.print(message);
}
