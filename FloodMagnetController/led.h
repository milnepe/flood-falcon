#ifndef _FLOOD_LED_H_
#define _FLOOD_LED_H_

// LEDs
#define L1_RED_PIN 15
#define L2_AMBER_PIN 14
#define L3_GREEN_PIN 10
#define RGB_RED_PIN 4
#define RGB_GREEN_PIN 3
#define RGB_BLUE_PIN 2

#define RED 1
#define AMBER 2
#define GREEN 3
#define BLUE 4
#define WHITE 5

static void rgb_colour(int colour) {
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

#endif
