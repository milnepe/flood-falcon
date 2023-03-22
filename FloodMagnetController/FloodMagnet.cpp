#include "FloodMagnet.h"

FloodMagnet::FloodMagnet(floodWarning *warning) {
  _warning = warning;  
}

void FloodMagnet::init() {
  state = INIT;
}

// Sets the rules for changing state
int FloodMagnet::updateState() {
  //Serial.println("Updating state...");
  //_warning->severityLevel = 1;  // mock  
  state = _warning->severityLevel;
  return state;
}
