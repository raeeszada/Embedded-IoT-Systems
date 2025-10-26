/*
  Project: Button Press Detection (Short / Long Press)
  Name: Raees Ul Mujtaba
  Reg No: 23-NTU-CS-1276
  Date: 26-Oct-2025
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin Definitions ---
#define BTN 25       // Button pin
#define LED 5        // LED pin
#define BUZZER 18   // Buzzer pin

// --- OLED Display Setup (I2C) ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// --- Variables ---
bool ledState = false;           // to store LED ON/OFF state
unsigned long pressTime = 0;     // to store the time when button is pressed
bool pressed = false;            // flag to check button press status

// --- Function to show text on OLED ---
void showText(String msg) {
  display.clearDisplay();         // clear old text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);       // position for message
  display.println(msg);           // print message
  display.display();              // update OLED screen
}

void setup() {
  pinMode(BTN, INPUT_PULLUP);     // button as input with internal pull-up
  pinMode(LED, OUTPUT);           // LED as output
  pinMode(BUZZER, OUTPUT);        // buzzer as output

  // --- Initialize the OLED Display ---
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showText("Ready...");           // startup message
}

void loop() {
  // --- Check if button is pressed down ---
  if (digitalRead(BTN) == LOW && !pressed) {
    pressed = true;               // mark button as pressed
    pressTime = millis();         // save press start time
  }

  // --- Check if button is released ---
  if (digitalRead(BTN) == HIGH && pressed) {
    unsigned long duration = millis() - pressTime;   // calculate how long it was held
    pressed = false;              // reset press flag

    // --- Long Press Detection (>1.5s) ---
    if (duration > 1500) {
      tone(BUZZER, 1000, 500);    // play buzzer tone
      showText("Long Press → Buzzer");
    } 
    // --- Short Press Detection ---
    else {
      ledState = !ledState;       // toggle LED state
      digitalWrite(LED, ledState);
      showText("Short Press → LED Toggle");
    }
  }
}
