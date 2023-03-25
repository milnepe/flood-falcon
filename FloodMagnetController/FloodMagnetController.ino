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

// Button connections
#define B1_PIN 21
#define B2_PIN 20
#define B3_PIN 19
#define B4_PIN 18
#define B5_PIN 17
#define B6_PIN 16

// LEDs
#define L1_RED_PIN 15
#define L2_AMBER_PIN 14
#define L3_GREEN_PIN 10
#define RGB_RED_PIN 4
#define RGB_GREEN_PIN 3
#define RGB_BLUE_PIN 2

// Pizo buzzer
#define BUZZER_PIN 9

enum led_colours { RED,
                  AMBER,
                  GREEN,
                  BLUE,
                  WHITE };

const char* soft_version = "0.1.0";

// Flood warning data
static floodWarning warning;

WiFiClient client;

FloodMagnet myMagnet = FloodMagnet(&warning);

FloodMagnetDisplay epd = FloodMagnetDisplay(&myMagnet);

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
int demo_state = NONE;

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

  myMagnet.init();  // Set statrting posture

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

void led_init() {
  pinMode(L1_RED_PIN, OUTPUT);
  pinMode(L2_AMBER_PIN, OUTPUT);
  pinMode(L3_GREEN_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  digitalWrite(RGB_RED_PIN, HIGH);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  digitalWrite(RGB_GREEN_PIN, HIGH);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  digitalWrite(RGB_BLUE_PIN, HIGH);

  led_colour(RED);
  delay(500);
  led_colour(AMBER);
  delay(500);
  led_colour(GREEN);
  delay(500);

  rgb_colour(RED);
  delay(500);
  rgb_colour(GREEN);
  delay(500);
  rgb_colour(BLUE);
  delay(500);
  rgb_colour(WHITE);
  delay(500);
}

void led_colour(led_colours colour) {
  switch (colour) {
    case RED:
      digitalWrite(L1_RED_PIN, HIGH);
      digitalWrite(L2_AMBER_PIN, LOW);
      digitalWrite(L3_GREEN_PIN, LOW);
      break;
    case AMBER:
      digitalWrite(L1_RED_PIN, LOW);
      digitalWrite(L2_AMBER_PIN, HIGH);
      digitalWrite(L3_GREEN_PIN, LOW);
      break;
    case GREEN:
      digitalWrite(L1_RED_PIN, LOW);
      digitalWrite(L2_AMBER_PIN, LOW);
      digitalWrite(L3_GREEN_PIN, HIGH);
      break;
    default:  // All off
      digitalWrite(L1_RED_PIN, LOW);
      digitalWrite(L2_AMBER_PIN, LOW);
      digitalWrite(L3_GREEN_PIN, LOW);
      break;
  }
}

void rgb_colour(led_colours colour) {
  switch (colour) {
    case RED:
      digitalWrite(RGB_RED_PIN, LOW);
      digitalWrite(RGB_GREEN_PIN, HIGH);
      digitalWrite(RGB_BLUE_PIN, HIGH);
      break;
    case GREEN:
      digitalWrite(RGB_RED_PIN, HIGH);
      digitalWrite(RGB_GREEN_PIN, LOW);
      digitalWrite(RGB_BLUE_PIN, HIGH);
      break;
    case BLUE:
      digitalWrite(RGB_RED_PIN, HIGH);
      digitalWrite(RGB_GREEN_PIN, HIGH);
      digitalWrite(RGB_BLUE_PIN, LOW);
      break;
    default:  // White
      digitalWrite(RGB_RED_PIN, LOW);
      digitalWrite(RGB_GREEN_PIN, LOW);
      digitalWrite(RGB_BLUE_PIN, LOW);
      break;
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
  // Inject mock timestamp
  memcpy(warning.time_raised, "2023-01-01 00:01:00", DATESTR_LEN - 1);
  myMagnet.updateState();
  epd.updateDisplay();

  switch (demo_state) {
    case NONE:
      demo_state = FLOOD_ALERT;
      led_colour(GREEN);
      break;
    case SEVERE_FLOOD_WARNING:
      led_colour(RED);
      demo_state = NO_LONGER;
      break;
    case FLOOD_WARNING:
      led_colour(RED);
      demo_state = SEVERE_FLOOD_WARNING;
      break;
    case FLOOD_ALERT:
      led_colour(AMBER);
      demo_state = FLOOD_WARNING;
      break;
    case NO_LONGER:
      led_colour(GREEN);
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
  Serial.print("Flood Area: https://check-for-flooding.service.gov.uk/target-area/");
  Serial.println(warning.flood_area_id);

  Serial.print("Warning Level: ");
  Serial.println(warning.severityLevel);

  Serial.print("Time Raised: ");
  Serial.println(warning.time_raised);
}
