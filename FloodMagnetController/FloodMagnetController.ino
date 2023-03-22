/*
  Arduino Nano 33 IoT client for Gov.co.uk Flood Warning Service

  Install the following libraries using the Arduino Libary Manager:
  Arduino WiFiNINA https://github.com/arduino-libraries/WiFiNINA
  Benoît Blanchon ArduinoJson https://arduinojson.org/
  Evert-arias EasyButton https://github.com/evert-arias/EasyButton
  Adafruit Soundboard https://github.com/adafruit/Adafruit_Soundboard_library
  Adafruit PWM Servo Driver https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

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

#define SERVO 0        // Flapping servo
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz

#define SFX_RST 4  // Sound board RST pin

// Flood warning data
static floodWarning warning;

WiFiClient client;

// Sound board connected to Serial1 - must be set to 9600 baud
Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST);

FloodMagnet myMagnet = FloodMagnet(&sfx, &warning);

FloodMagnetDisplay epd = FloodMagnetDisplay(&myMagnet);

int status = WL_IDLE_STATUS;
boolean updateDisplayFlag = false;
unsigned long lastReconnectAttempt = 0;
boolean playBackFlag = false;

// Define pins
const int wifiLed = 10;  // Optional debug LED (Green - On if connected)
const int rButton = 2;
const int lButton = 3;
const int dButton = 9;

EasyButton rightButton(rButton);  // RH button
EasyButton leftButton(lButton);   // LH button
EasyButton demoButton(dButton);   // External demo button

int demo_state = NONE;

void setup() {
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, LOW);

  // Initialize Serial Port
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  delay(2000);

  // Serial 1 used for sound board at 9600 baud
  Serial1.begin(9600);
  while (!Serial1) {
    ;  // wait for serial port to connect
  }
  Serial.print("Starting client version: ");
  Serial.println(soft_version);

  Serial.println("Serial1 attached");

  // Initialize buttons
  rightButton.begin();
  rightButton.onPressed(playback);        // Short press triggers playback
  rightButton.onPressedFor(2000, audio);  // Long press toggles audio
  leftButton.begin();
  leftButton.onPressed(demo);  // Short press for demo
  demoButton.begin();
  demoButton.onPressed(demo);  // Short press external button for demo
  // Press reset button (middle) to exit demo

  // Setup display and show greeting
  epd.initDisplay();
  epd.showGreeting();

  myMagnet.init();  // Set statrting posture
  myMagnet.doAction(epd.audioOn);    // Trigger intro action

  leftButton.read();
  demoButton.read();
  if (leftButton.isPressed() || demoButton.isPressed()) {
    epd.demoOn = true;
  }

  delay(12000);
}

void loop() {
  // Continuously update the button states
  rightButton.read();
  leftButton.read();
  demoButton.read();

  if (!epd.demoOn) {  // Standard mode
    if (WiFi.status() != WL_CONNECTED) {
      // Connect wifi if it's not connected
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
    if ((now - lastReconnectAttempt > ALERT_INTERVAL) || (updateDisplayFlag) || (playBackFlag)) {
      updateDisplayFlag = false;
      playBackFlag = false;

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
  myMagnet.doAction(epd.audioOn);
  epd.updateDisplay();
  printData();
}

void doDemo() {
  warning.severityLevel = demo_state;
  myMagnet.updateState();
  epd.updateDisplay();
  myMagnet.doAction(epd.audioOn);


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
  client.println("GET /flood-monitoring/id/floodAreas/" AREA_CODE
                 " HTTP/1.0");
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
                                                                                                                         //warning.severityLevel = 1; // mock level
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
void playback() {
  Serial.println("Playback button pressed!");
  playBackFlag = true;
}

void audio() {
  Serial.println("Audio button pressed!");
  epd.audioOn = !epd.audioOn;
  updateDisplayFlag = true;
}

void demo() {
  Serial.println("Demo button pressed!");
  epd.demoOn = true;
}

void printData() {
  Serial.print("Flood Area: ");
  Serial.println(warning.flood_area_id);

  Serial.print("Warning Level: ");
  Serial.println(warning.severityLevel);

  Serial.print("Time Raised: ");
  Serial.println(warning.time_raised);
}
