#ifndef _FLOOD_LED_H_
#define _FLOOD_LED_H_

// LEDs
#define L1_RED_PIN 6
#define L2_AMBER_PIN 9
#define L3_GREEN_PIN 10
#define RGB_RED_PIN 5
#define RGB_GREEN_PIN 3
#define RGB_BLUE_PIN 2

#define RED 1
#define AMBER 2
#define GREEN 3
#define BLUE 4
#define WHITE 5

// Common anode - 0 is on 255 is off
static void rgb_colour(int colour) {
  switch (colour) {
    case RED:
      analogWrite(RGB_RED_PIN, 127);  // Half brightness
      analogWrite(RGB_GREEN_PIN, 0);
      digitalWrite(RGB_BLUE_PIN, LOW);
      break;
    case GREEN:
      analogWrite(RGB_RED_PIN, 0);
      analogWrite(RGB_GREEN_PIN, 127);  // Half brightness
      digitalWrite(RGB_BLUE_PIN, LOW);
      break;
    case BLUE:
      analogWrite(RGB_RED_PIN, 0);
      analogWrite(RGB_GREEN_PIN, 0);
      digitalWrite(RGB_BLUE_PIN, HIGH);  // Full brightness
      break;
    case WHITE:
      analogWrite(RGB_RED_PIN, 255);
      analogWrite(RGB_GREEN_PIN, 255);
      digitalWrite(RGB_BLUE_PIN, HIGH);
      break;
    default:  // White
      analogWrite(RGB_RED_PIN, 255);
      analogWrite(RGB_GREEN_PIN, 255);
      digitalWrite(RGB_BLUE_PIN, HIGH);
      break;
  }
}

static void led_colour(int colour) {
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

static void led_init() {
  pinMode(L1_RED_PIN, OUTPUT);
  pinMode(L2_AMBER_PIN, OUTPUT);
  pinMode(L3_GREEN_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

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

#endif
