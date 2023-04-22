#ifndef _FLOOD_BUZZER_H_
#define _FLOOD_BUZZER_H_

// Pizo buzzer
#define BUZZER_PIN 15

static void buzzer_on() {
  digitalWrite(BUZZER_PIN, HIGH);
}

static void buzzer_off() {
  digitalWrite(BUZZER_PIN, LOW);
}

static void bip() {
  buzzer_on();
  delay(100);
  buzzer_off();
}

static void buzzer_init() {
  pinMode(BUZZER_PIN, OUTPUT);
  bip();
}

#endif