#include "FloodFalcon.h"
#include "dow.h"

FloodFalcon::FloodFalcon(Adafruit_Soundboard *sfx, Adafruit_PWMServoDriver *pwm, floodWarning *warning) {
  _sfx = sfx;
  _pwm = pwm;
  _warning = warning;
}

void FloodFalcon::init(int servo, uint16_t pos) {
  _servo = servo;
  StartPos(pos);
  state = INIT;
}

int FloodFalcon::doAction(boolean audio) {
  Serial.println("Activating...");
  Serial.print("State: ");
  Serial.println(state);
  switch (state) {
    case NONE:
      Serial.println("Wings up a bit, very slowly");
      Tweet(NONE, audio);
      Flap(WINGS_DOWN, WINGS_UP_A_BIT, VSLOW, 10);
      break;
    case SEVERE_FLOOD_WARNING:
      Serial.println("Wings up alot, very fast");
      Tweet(SEVERE_FLOOD_WARNING, audio);
      Flap(WINGS_DOWN, WINGS_UP_A_LOT, VFAST, 10);      
      break;
    case FLOOD_WARNING:
      Serial.println("Wings up a bit, fast");
      Tweet(FLOOD_WARNING, audio);
      Flap(WINGS_DOWN, WINGS_UP_A_BIT, FAST, 10);      
      break;
    case FLOOD_ALERT:
      Serial.println("Wings down, wings up a bit");
      Tweet(FLOOD_ALERT, audio);
      Flap(WINGS_DOWN, WINGS_UP_A_BIT, SLOW, 10);      
      break;
    case NO_LONGER:
      Serial.println("Wings down, wings up a bit, slowly");
      Tweet(NO_LONGER, audio);
      Flap(WINGS_DOWN, WINGS_UP_A_BIT, VSLOW, 10);      
      break;
    case INIT:
      Serial.println("Wings down");
      StartPos(WINGS_DOWN);
      Tweet(INIT, audio);
      break;
  }
  return state;
}

// Sets the rules for changing state
int FloodFalcon::updateState() {
  Serial.println("Updating state...");
  //_warning->items_currentWarning_severityLevel = 1;  // mock  
  state = _warning->items_currentWarning_severityLevel;
  return state;
}

void FloodFalcon::StartPos(uint16_t start_pos) {
  // Move wings to start position
  for (; _pulselen < start_pos; _pulselen++) {
    _pwm->setPWM(_servo, 0, _pulselen);
    delay(VSLOW);
  }
}

void FloodFalcon::Flap(uint16_t down_pos, uint16_t up_pos, int speed_idx, int flaps) {
  // Move wings back and fourth
  // Note that the value of pulselen is inverted!
  // speed_idx  0 is fastest

  for (int i = 0; i < flaps; i++) {
    // Up
    for (; _pulselen > up_pos; _pulselen--) {
      _pwm->setPWM(_servo, 0, _pulselen);
      delay(speed_idx);
    }
    delay(100);
    // Down
    for (; _pulselen < down_pos; _pulselen++) {
      _pwm->setPWM(_servo, 0, _pulselen);
      delay(speed_idx);
    }
    delay(100);
  }
}

void FloodFalcon::PassOut(uint16_t end_pos, int speed_idx) {
  // Move wings to pass out position and hold it
  // Note that the value of pulselen is inverted!

  for (; _pulselen > end_pos; _pulselen--) {
    _pwm->setPWM(_servo, 0, _pulselen);
    delay(speed_idx);
  }
}


void FloodFalcon::Dead(uint16_t end_pos, int speed_idx) {
  // Move servo past tipping point then retract wings.
  // This position should only be recovered by resetting the system

  for (; _pulselen > end_pos; _pulselen--) {
    _pwm->setPWM(_servo, 0, _pulselen);
    delay(speed_idx);
  }
}

void FloodFalcon::Tweet(uint8_t track, boolean audio = true) {
  if (audio) {
    if (!_sfx->playTrack(track)) {
      ;
    }
  }
}

// Return day number as day of week - day 6 (Thu)
const char *FloodFalcon::dow(int i) {
  static const char *dname[] = {
    "Sat", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri"
  };

  uint16_t yyyy = getYearInt(_warning[i].datestr);
  uint8_t mm = getMonthInt(_warning[i].datestr);
  uint8_t dd = getDayInt(_warning[i].datestr);
  int n = dayOfWeek(yyyy, mm, dd);

  return dname[n];
}
