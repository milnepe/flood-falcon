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
#include "FloodMagnet.h"
#include "FloodMagnetDisplay.h"

const char* soft_version = "0.1.0";

// Flood warning data
static floodWarning warning;

WiFiClient client;

FloodMagnet myMagnet = FloodMagnet(&warning);

FloodMagnetDisplay epd = FloodMagnetDisplay(&myMagnet);

int status = WL_IDLE_STATUS;
boolean updateDisplayFlag = false;
boolean replayFlag = false;
unsigned long lastReconnectAttempt = 0;

// Define pins
const int wifiLed = 10;  // Optional debug LED (Green - On if connected)
enum buttons { REPLAY = 21,
               DEMO = 20,
               EXT_DEMO = 19,
               BUTTON4 = 18,
               BUTTON5 = 17 };

EasyButton button1(REPLAY);
EasyButton button2(DEMO);
EasyButton button3(EXT_DEMO);
EasyButton button4(BUTTON4);
EasyButton button5(BUTTON5);

enum mode { DEMO_MODE,
            STD_MODE,
            REPLAY_MODE };
int mode = STD_MODE;  // Start in STANDARD mode
int demo_state = NONE;

void setup() {
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, LOW);

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

  // Setup display and show greeting
  epd.initDisplay();
  epd.showGreeting();

  myMagnet.init();  // Set statrting posture

  button2.read();
  button3.read();
  if (button2.isPressed() || button3.isPressed()) {
    epd.demoOn = true;
  }

  delay(12000);
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
      digitalWrite(wifiLed, LOW);
      epd.wifiOn = false;
      reconnectWiFi();
      delay(2000);
      if (WiFi.status() == WL_CONNECTED) {
        digitalWrite(wifiLed, HIGH);
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
  getData();
  myMagnet.updateState();
  epd.updateDisplay();
  printData();
}

void doDemo() {
  epd.demoOn = true;
  warning.severityLevel = demo_state;
  myMagnet.updateState();
  epd.updateDisplay();

  switch (demo_state) {
    case NONE:
      demo_state = FLOOD_ALERT;
      break;
    case SEVERE_FLOOD_WARNING:
      demo_state = NO_LONGER;
      break;
    case FLOOD_WARNING:
      demo_state = SEVERE_FLOOD_WARNING;
      break;
    case FLOOD_ALERT:
      demo_state = FLOOD_WARNING;
      break;
    case NO_LONGER:
      demo_state = NONE;
      break;
    default:
      break;
  }
  delay(DEMO_INTERVAL);  // Delay between state change
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
  // Creds from arduino_secrets.h
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  return WiFi.status();
}

void getData() {
  // Connect to host
  Serial.println("Connecting to environment.data.gov.uk");
  if (!client.connect("environment.data.gov.uk", 80)) {
    Serial.println("Failed to connect to server");
    return;
  }

  // Send HTTP request
  client.println("GET /flood-monitoring/id/floodAreas/" AREA_CODE " HTTP/1.0");
  client.println("Host: environment.data.gov.uk");
  client.println("Connection: close");
  client.println();

  // Check status code
  char status[32] = { 0 };
  client.readBytesUntil('\r', status, sizeof(status));
  // should be "HTTP/1.0 200 OK"
  if (memcmp(status + 9, "200 OK", 6) != 0) {
    Serial.print("Unexpected HTTP status");
    Serial.println(status);
    client.stop();
    return;
  }

  // Skip response headers
  client.find("\r\n\r\n");

  // Stream& input;
  StaticJsonDocument<128> filter;

  // Filter data objects so the response fits into memory
  JsonObject filter_items = filter.createNestedObject("items");
  filter_items["currentWarning"]["severityLevel"] = true;
  filter_items["currentWarning"]["floodAreaID"] = true;
  filter_items["currentWarning"]["timeRaised"] = true;

  StaticJsonDocument<1024> doc;

  DeserializationError error = deserializeJson(doc, client, DeserializationOption::Filter(filter));

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Update warning struct
  warning.severityLevel = doc["items"]["currentWarning"]["severityLevel"];                                               // 3
  if (warning.severityLevel) {                                                                                           // only update these items if the level is not zero
    memcpy(warning.flood_area_id, doc["items"]["currentWarning"]["floodAreaID"].as<const char*>(), FLOOD_AREA_LEN - 1);  // "Tributaries between Dorchester and ...

    memcpy(warning.time_raised, doc["items"]["currentWarning"]["timeRaised"].as<const char*>(), DATESTR_LEN - 1);  // "2022-12-19T15:20:31"
    for (int i = 0; i < DATESTR_LEN; i++) {
      if (warning.time_raised[i] == 'T') {
        warning.time_raised[i] = ' ';
      }
    }
  }
  // Close the connection to the server
  client.stop();

  Serial.println("Flood data received!");
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
  Serial.print("Flood Area: ");
  Serial.println(warning.flood_area_id);

  Serial.print("Warning Level: ");
  Serial.println(warning.severityLevel);

  Serial.print("Time Raised: ");
  Serial.println(warning.time_raised);
}
