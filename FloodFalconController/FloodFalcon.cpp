#include "FloodFalcon.h"
#include "dow.h"

FloodFalcon::FloodFalcon(Adafruit_Soundboard *sfx, Adafruit_PWMServoDriver *pwm, uvData *fcst) {
  _sfx = sfx;
  _pwm = pwm;
  _fcst = fcst;
}

void FloodFalcon::init(int servo, uint16_t pos) {
  _servo = servo;
  StartPos(pos);
  state = 0;
}

int FloodFalcon::doAction(boolean audio) {
  Serial.println("Activating...");
  Serial.print("State: ");
  Serial.println(state);
  switch (state) {
    case 0:  // init
      Serial.println("Wings down");
      StartPos(WINGS_DOWN);
      Tweet(CHEEP_CHEEP, audio);      
      break;
    case 1:  // UV 0
      Serial.println("Wings down");
      StartPos(WINGS_DOWN);
      Tweet(NO_PROTECTION_A, audio);
      break;
    case 2:  // UV 1 - 2
      Serial.println("Wings down");
      Flap(WINGS_DOWN, WINGS_UP_A_BIT, VSLOW, 3);
      Tweet(NO_PROTECTION_B, audio);
      break;
    case 3:  // UV 3 - 5
      Serial.println("Wings down, wings up a bit");
      Flap(WINGS_DOWN, WINGS_UP_A_BIT, SLOW, 3);
      Tweet(SOME_PROTECTION, audio);
      break;
    case 4:  // UV 6 - 7
      Serial.println("Wings down, wings up a lot");
      Flap(WINGS_DOWN, WINGS_UP_A_LOT, FAST, 4);
      Tweet(PROTECTION_ESSENTIAL, audio);
      break;
    case 5:  // UV 8 plus
      Serial.println("Wings pass out");
      PassOut(PASS_OUT_POS, FAST);
      Tweet(EXTRA_PROTECTION, audio);
      //while (1);// Program ends!! Reboot
  }
  return state;
}

// Sets the rules for changing state
// UV states
// UV_ZERO = 0
// UV_ONE = 1 - 2
// UV_TWO = 3 - 5
// UV_THREE = 6 - 7
// UV_FOUR = 8 - 11
int FloodFalcon::updateState() {
  Serial.println("Updating state...");
  switch (getUVMax(0)) {
    case 0:
      state = 1;
      break;
    case 1 ... 2:
      state = 2;
      break;
    case 3 ... 5:
      state = 3;
      break;
    case 6 ... 7:
      state = 4;
      break;
    case 8 ... 11:
      state = 5;
  }
  return state;
}

// Only return single digit UV
int FloodFalcon::getUVMax(int day_idx) {
  char max_uv = 0;
  for (int i = 0; i < 8; ++i) {
    char uv = _fcst[day_idx].uv[i];
    if (uv > 9) {
      uv = 9;
    }
    max_uv = max(max_uv, uv);
  }
  return (int)max_uv;
}

// Return up to 2 digit positive temps
int FloodFalcon::getTempMax(int day_idx) {
  char max_temp = 0;
  for (int i = 0; i < 8; ++i) {
    char temp = _fcst[day_idx].temp[i];
    if (temp > 99) {
      temp = 99;
    }
    max_temp = max(max_temp, temp);
  }
  return (int)max_temp;
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
    if (! _sfx->playTrack(track)) {
      ;
    }
  }
}

// Return day number as day of week - day 6 (Thu)
const char* FloodFalcon::dow(int i) {
  static const char* dname[] = {
    "Sat", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri"
  };

  uint16_t yyyy = getYearInt(_fcst[i].datestr);
  uint8_t mm = getMonthInt(_fcst[i].datestr);
  uint8_t dd = getDayInt(_fcst[i].datestr);
  int n = dayOfWeek(yyyy, mm, dd);

  return dname[n];
}
