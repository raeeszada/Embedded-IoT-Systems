/*
  Project: LED Mode Controller with OLED and Buzzer
  Name: Raees Ul Mujtaba
  Reg No: 23-NTU-CS-1276
  Date: 26-Oct-2025
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin definitions ---
#define LED1 2           // LED 1 pin
#define LED2 4           // LED 2 pin
#define LED3 5           // LED 3 (PWM LED)
#define BTN_MODE 26      // Button for changing modes
#define BTN_RESET 27     // Button for resetting to OFF
#define BUZZER 15        // Buzzer pin

// --- OLED setup ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// --- Global variables ---
int mode = 1;                     // start from Mode 1
unsigned long prevMillis = 0;     // for blinking timing
bool ledState = false;            // toggle flag

// --- Function: Display text on OLED ---
void showMsg(const String &msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 25);
  display.print("Mode: ");
  display.println(msg);
  display.display();
}

// --- Function: Play beep tone ---
void beepBuzzer(int freq, int dur) {
  tone(BUZZER, freq, dur);
  delay(dur);
  noTone(BUZZER);
}

// --- Function: Startup animation ---
void startupAnimation() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15, 25);
  display.print("Initializing...");
  display.display();

  for (int i = 0; i < 3; i++) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    beepBuzzer(700 + (i * 200), 100);
    delay(150);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    delay(150);
  }

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  showMsg("System Ready!");
  delay(600);
}

void setup() {
  // --- Pin setup ---
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  // --- OLED initialization ---
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  startupAnimation();
  showMsg("Both OFF");
}

void loop() {
  // --- MODE button ---
  if (digitalRead(BTN_MODE) == LOW) {
    delay(200);  
    mode++;
    if (mode > 4) mode = 1;

    switch (mode) {
      case 1:
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        analogWrite(LED3, 0);
        showMsg("Both OFF");
        beepBuzzer(800, 120);
        break;

      case 2:
        showMsg("Alternate Blink");
        beepBuzzer(1000, 120);
        break;

      case 3:
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, HIGH);
        analogWrite(LED3, 0);
        showMsg("Both ON");
        beepBuzzer(1200, 120);
        break;

      case 4:
        showMsg("PWM Fade");
        beepBuzzer(1500, 120);
        break;
    }
  }

  // --- RESET button ---
  if (digitalRead(BTN_RESET) == LOW) {
    delay(200);
    mode = 1;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    analogWrite(LED3, 0);
    showMsg("Reset to OFF");
    beepBuzzer(500, 200);
  }

  // --- Mode behaviors ---
  if (mode == 2) { 
    if (millis() - prevMillis >= 500) {
      prevMillis = millis();
      ledState = !ledState;
      digitalWrite(LED1, ledState);
      digitalWrite(LED2, !ledState);
    }
  }

  if (mode == 4) { 
    for (int i = 0; i <= 255; i++) {
      analogWrite(LED3, i);
      delay(4);
    }
    for (int i = 255; i >= 0; i--) {
      analogWrite(LED3, i);
      delay(4);
    }
  }
}
