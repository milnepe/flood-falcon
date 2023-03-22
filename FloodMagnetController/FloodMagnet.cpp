#include "FloodMagnet.h"

FloodMagnet::FloodMagnet(Adafruit_Soundboard *sfx, floodWarning *warning) {
  _sfx = sfx;
  _warning = warning;  
}

void FloodMagnet::init() {
  state = INIT;
}

int FloodMagnet::doAction(boolean audio) {
  Serial.println("Activating...");
  Serial.print("State: ");
  Serial.println(state);
  switch (state) {
    case NONE:
      Serial.println("Wings up a bit, very slowly");
      Tweet(NONE, audio);
      break;
    case SEVERE_FLOOD_WARNING:
      Serial.println("Wings up alot, very fast");
      Tweet(SEVERE_FLOOD_WARNING, audio);    
      break;
    case FLOOD_WARNING:
      Serial.println("Wings up a bit, fast");
      Tweet(FLOOD_WARNING, audio);    
      break;
    case FLOOD_ALERT:
      Serial.println("Wings down, wings up a bit");
      Tweet(FLOOD_ALERT, audio);  
      break;
    case NO_LONGER:
      Serial.println("Wings down, wings up a bit, slowly");
      Tweet(NO_LONGER, audio);  
      break;
    case INIT:
      Serial.println("Wings down");
      Tweet(INIT, audio);
      break;
  }
  return state;
}

// Sets the rules for changing state
int FloodMagnet::updateState() {
  Serial.println("Updating state...");
  //_warning->severityLevel = 1;  // mock  
  state = _warning->severityLevel;
  return state;
}

void FloodMagnet::Tweet(uint8_t track, boolean audio = true) {
  if (audio) {
    if (!_sfx->playTrack(track)) {
      ;
    }
  }
}
