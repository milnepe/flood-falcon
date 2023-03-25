/*
  Arduino Nano 33 IoT client for Gov.co.uk Flood Warning Service

  Install the following libraries using the Arduino Libary Manager:
  Arduino WiFiNINA https://github.com/arduino-libraries/WiFiNINA
  Benoît Blanchon ArduinoJson https://arduinojson.org/
  Evert-arias EasyButton https://github.com/evert-arias/EasyButton

  Author: Peter Milne
  Date: 22 March 2023

  Copyright 2022 Peter Milne
  Released under GNU GENERAL PUBLIC LICENSE
  Version 3, 29 June 2007
*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include <EasyButton.h>
#include "FloodAPI.h"
#include "FloodMagnetDisplay.h"
#include "led.h"

// Button connections
#define B1_PIN 21
#define B2_PIN 20
#define B3_PIN 19
#define B4_PIN 18
#define B5_PIN 17
#define B6_PIN 16

// Pizo buzzer
#define BUZZER_PIN 9

const char* soft_version = "0.1.0";

FloodAPI myFloodAPI = FloodAPI();

FloodMagnetDisplay epd = FloodMagnetDisplay(&myFloodAPI);

int status = WL_IDLE_STATUS;
boolean updateDisplayFlag = false;
unsigned long lastReconnectAttempt = 0;

EasyButton button1(B1_PIN);
EasyButton button2(B2_PIN);
EasyButton button3(B3_PIN);
EasyButton button4(B4_PIN);
EasyButton button5(B5_PIN);
EasyButton button6(B6_PIN);

enum mode { DEMO_MODE,
            STD_MODE,
            REPLAY_MODE };
int mode = STD_MODE;  // Start in STANDARD mode

void setup() {
  led_init();

  // Initialize Serial Port
  Serial.begin(115200);
  // while (!Serial) {
  //   ;  // wait for serial port to connect. Needed for native USB port only
  // }
  delay(2000);

  Serial.print("Starting client version: ");
  Serial.println(soft_version);

  // Initialize buttons
  button1.begin();
  button1.onPressed(replay);  // Short press triggers replay
  //button1.onPressedFor(2000, audio);  // Long press toggles audio
  button2.begin();
  button2.onPressed(demo);  // Short press for demo
  button3.begin();
  button3.onPressed(demo);  // Short press external demo button / reset to exit
  button4.begin();
  button4.onPressed(button4_callback);  // Place holder
  button5.begin();
  button5.onPressed(button5_callback);  // Place holder

  // Go straight to demo mode with no wifi
  button2.read();
  button3.read();
  if (button2.isPressed() || button3.isPressed()) {
    Serial.println("Demo is pressed");
    mode = DEMO_MODE;
  }

  // Setup display and show greeting
  epd.initDisplay();
  epd.showGreeting();

  myFloodAPI.init();  // Set statrting posture

  delay(5000);
}

void loop() {
  // Continuously update the button states
  button1.read();
  button2.read();
  button3.read();
  button4.read();
  button5.read();

  if (mode == STD_MODE || mode == REPLAY_MODE) {
    if (WiFi.status() != WL_CONNECTED) {  // Connect wifi
      rgb_colour(RED);
      epd.wifiOn = false;
      reconnectWiFi();
      delay(2000);
      if (WiFi.status() == WL_CONNECTED) {
        rgb_colour(GREEN);
        epd.wifiOn = true;
        Serial.println("Wifi connected...");
      }
      doUpdate();  // Initial update
    }
    unsigned long now = millis();
    if ((now - lastReconnectAttempt > ALERT_INTERVAL) || (updateDisplayFlag) || (mode == REPLAY_MODE)) {
      updateDisplayFlag = false;
      mode = STD_MODE;  // Clear replay

      doUpdate();
      lastReconnectAttempt = now;
    }
  } else {  // Demo mode - reset to exit so everything re-initialises
    doDemo();
  }
}

void doUpdate() {
  myFloodAPI.getData();
  myFloodAPI.updateState(myFloodAPI.warning.severityLevel);
  epd.updateDisplay();
  printData();
}

void doDemo() {
  epd.demoOn = true;
  while (1) {
    myFloodAPI.demo();
    epd.updateDisplay();
    delay(DEMO_INTERVAL);  // Delay between state change
  }
}

int reconnectWiFi() {
  // WL_IDLE_STATUS     = 0
  // WL_NO_SSID_AVAIL   = 1
  // WL_SCAN_COMPLETED  = 2
  // WL_CONNECTED       = 3
  // WL_CONNECT_FAILED  = 4
  // WL_CONNECTION_LOST = 5
  // WL_DISCONNECTED    = 6

  WiFi.disconnect();  // Force a disconnect
  delay(1000);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  return WiFi.status();
}

// Button callbacks
void replay() {
  Serial.println("Replay button pressed!");
  mode = REPLAY_MODE;
}

void demo() {
  Serial.println("Demo button pressed!");
  mode = DEMO_MODE;
}

void button4_callback() {
  Serial.println("Button 4 pressed!");
}

void button5_callback() {
  Serial.println("Button 5 pressed!");
}

// Debug output
void printData() {
  Serial.print("Flood Area: https://check-for-flooding.service.gov.uk/target-area/");
  Serial.println(myFloodAPI.warning.flood_area_id);

  Serial.print("Warning Level: ");
  Serial.println(myFloodAPI.warning.severityLevel);

  Serial.print("Time Raised: ");
  Serial.println(myFloodAPI.warning.time_raised);
}
