/****************************************************
 * ESP32 + DHT22 + SSD1306 OLED + Button + Blynk
 * Pins (from Wokwi diagram):
 * DHT22 SDA -> GPIO23
 * OLED SDA  -> GPIO21
 * OLED SCL  -> GPIO22
 * Button    -> GPIO5 (active LOW)
 ****************************************************/

#define BLYNK_TEMPLATE_ID "TMPL6XWVSlKE6"
#define BLYNK_TEMPLATE_NAME "DHT by Raees"
#define BLYNK_AUTH_TOKEN "K53kffIUeq94KEk9hNZZP7ErsHpp_F98"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// ------------ WiFi credentials (for wokwi) ------------
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ------------ Pins (match your Wokwi diagram) ------------
#define DHTPIN   23
#define DHTTYPE  DHT22  // <--- CHANGED TO DHT22

#define BUTTON_PIN 5

// ------------ OLED settings ------------
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;
int lastButtonState = HIGH;

// Forward declaration
void readAndDisplayAndSend();

void periodicSend() {
  readAndDisplayAndSend();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("ESP32 DHT22 + OLED + Blynk starting...");

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // I2C + OLED
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Booting...");
  display.display();

  // DHT sensor
  dht.begin();

  // Blynk
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(5000L, periodicSend);
}

void readAndDisplayAndSend() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius

  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("DHT Error!");
    display.display();
    return;
  }

  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print(" *C, Hum: ");
  Serial.print(h);
  Serial.println(" %");

  // --- Update OLED ---
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Environment Node");
  display.println("-----------------");
  display.print("Temp: ");
  display.print(t, 1);
  display.println(" C");
  display.print("Hum : ");
  display.print(h, 1);
  display.println(" %");
  display.println();
  display.println("BTN -> manual update");
  display.display();

  // --- Send to Blynk ---
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

void loop() {
  Blynk.run();
  timer.run();

  int currentState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && currentState == LOW) {
    Serial.println("Button pressed: manual DHT read");
    readAndDisplayAndSend();
  }
  lastButtonState = currentState;
}