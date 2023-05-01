#include "FloodAPI.h"

// Flood warning data
//static floodWarning warning;
FloodAPI::FloodAPI() {
}

void FloodAPI::init() {
  state = INIT;
}

int FloodAPI::updateState(warning_levels state) {
  static warning_levels previous_state = NONE;
  if (state != previous_state) {
    previous_state = state;
    switch (state) {
      case NONE:
        led_colour(GREEN);
        break;
      case SEVERE_FLOOD_WARNING:
        led_colour(RED);
        buzzer_on();
        break;
      case FLOOD_WARNING:
        led_colour(RED);
        buzzer_on();
        break;
      case FLOOD_ALERT:
        led_colour(AMBER);
        buzzer_on();
        break;
      case NO_LONGER:
        led_colour(GREEN);
        break;
      default:
        break;
    }
  }
  return state;
}

// Advance through states and wrap around
void FloodAPI::demo(modes m) {
  // Inject mock timestamp
  memcpy(warning.time_raised, "2023-01-01 00:01:00", DATESTR_LEN - 1);
  static warning_levels state = NONE;
  warning.severityLevel = state;
  Serial.println(warning.severityLevel);
  updateState(warning.severityLevel);
  switch (state) {
    case NONE:
      state = FLOOD_ALERT;
      break;
    case SEVERE_FLOOD_WARNING:
      state = NO_LONGER;
      break;
    case FLOOD_WARNING:
      state = SEVERE_FLOOD_WARNING;
      break;
    case FLOOD_ALERT:
      state = FLOOD_WARNING;
      break;
    case NO_LONGER:
      state = NONE;
      break;
  }
}

void FloodAPI::getData() {
  WiFiClient client;
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