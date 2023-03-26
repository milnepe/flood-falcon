/*
  Arduino Nano 33 IoT client for Gov.co.uk Flood Warning Service

These APIs are provided as open data under the Open Government Licence with no requirement for registration.
If you make use of this data please acknowledge this with the following attribution statement:

"this uses Environment Agency flood and river level data from the real-time data API (Beta)"

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

const char* soft_version = "0.2.1";

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
  button1.onPressed(dry);
  //button1.onPressedFor(2000, audio);  // Long press toggles audio
  button2.begin();
  button2.onPressed(rain);  // Place holder
  button3.begin();
  button3.onPressed(flood);  // Place holder
  button4.begin();
  button4.onPressed(replay);
  button5.begin();
  // Hold down B5 while pressing reset to enter demo mode
  // Press reset to exit back to stdard mode
  button5.onPressed(demo);
  button6.begin();
  button6.onPressed(clock_sync_ap_mode);  // Place holder

  // Go straight to demo mode with no wifi
  button2.read();
  button3.read();
  if (button5.isPressed()) {
    Serial.println("Starting demo mode...");
    mode = DEMO_MODE;
  }

  // Setup display and show greeting
  epd.initDisplay();
  epd.showGreeting();

  myFloodAPI.init();

  delay(3000);
}

void loop() {
  // Continuously update the button states
  button1.read();
  button2.read();
  button3.read();
  button4.read();
  // button5.read(); Read only in setup
  button6.read();

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
void dry() {
  Serial.println("B1 button pressed...");
}

void rain() {
  Serial.println("B2 button pressed...");
}

void flood() {
  Serial.println("B3 button pressed...");
}

void replay() {
  Serial.println("B4 button pressed...");
  mode = REPLAY_MODE;
}

void demo() {
  Serial.println("B5 button pressed...");
  mode = DEMO_MODE;
}

void clock_sync_ap_mode() {
  Serial.println("B6 button pressed...");
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
