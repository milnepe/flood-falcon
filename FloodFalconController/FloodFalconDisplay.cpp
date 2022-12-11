#include "FloodFalconDisplay.h"
#include "arduino_secrets.h"

void FloodFalconDisplay::initDisplay(void) {
  if (_epd.Init() != 0) {
    return;
  }
  Serial.println("EDP attached");

  _epd.ClearFrameMemory(0xFF);  // bit set = white, bit reset = black
  _epd.DisplayFrame();

  delay(2000);

  _epd.SetFrameMemory_Base(RSLOGO);
  _epd.DisplayFrame();
}

void FloodFalconDisplay::showGreeting(void) {
  _paint.SetWidth(120);
  _paint.SetHeight(32);
  _paint.SetRotate(ROTATE_180);

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "  Flood  ", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 140, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "  Falcon  ", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 120, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "Concept:", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 80, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 4, "Jude Pullen", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 60, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, "Code:", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 20, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, "Pete Milne", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 0, _paint.GetWidth(), _paint.GetHeight());

  _epd.DisplayFrame_Partial();
}

// void FloodFalconDisplay::updateDisplay() {
//   Serial.println("Updating display...");
//   int severityLevel = (_falcon->_warning->items_currentWarning_severityLevel);
//   Serial.println(severityLevel);
// }

void FloodFalconDisplay::updateDisplay() {
  Serial.println("Updating display...");
  int severityLevel = _falcon->_warning->items_currentWarning_severityLevel;
  //  char single_digit[] = {'0', '\0'};
  //  char double_digit[] = {'0', '0', '\0'};
  //  char three_digit[] = {'0', '/', '0', '\0'};
  //  char four_digit[] = {'0', '/', '0', '0',  '\0'};
  //
  // Set background
  if (_epd.Init() != 0) {
    return;
  }
  _epd.ClearFrameMemory(0xFF);  // bit set = white, bit reset = black
  _epd.DisplayFrame();

  delay(500);
  switch (severityLevel) {
    case INIT:
      _epd.SetFrameMemory_Base(epd_flood_alert);
      break;
    case SEVERE_FLOOD_WARNING:
      _epd.SetFrameMemory_Base(epd_flood_warning_severe);
      break;
    case FLOOD_WARNING:
      _epd.SetFrameMemory_Base(epd_flood_warning);
      break;
    case FLOOD_ALERT:
      _epd.SetFrameMemory_Base(epd_flood_alert);
      break;
    case NO_LONGER:
      _epd.SetFrameMemory_Base(epd_flood_alert);
  }

  _epd.DisplayFrame();

  // Static text
  _paint.SetWidth(120);
  _paint.SetHeight(40);
  _paint.SetRotate(ROTATE_180);

  // _paint.Clear(UNCOLORED);
  // _paint.DrawStringAt(0, 0, "UV Max", &Font16, COLORED);
  // _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 140, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, LINE_1, &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 120, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, LINE_2, &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 100, _paint.GetWidth(), _paint.GetHeight());
  
   _paint.Clear(UNCOLORED);
   _paint.DrawStringAt(0, 0, LINE_3, &Font16, COLORED);
   _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 80, _paint.GetWidth(), _paint.GetHeight());
  
   _paint.Clear(UNCOLORED);
   _paint.DrawStringAt(0, 0, LINE_4, &Font16, COLORED);
   _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 60, _paint.GetWidth(), _paint.GetHeight());
  
   _paint.Clear(UNCOLORED);
   _paint.DrawStringAt(0, 0, LINE_5, &Font16, COLORED);
   _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 40, _paint.GetWidth(), _paint.GetHeight());
  
   _paint.Clear(UNCOLORED);
   _paint.DrawStringAt(0, 0, LINE_6, &Font16, COLORED);
   _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 20, _paint.GetWidth(), _paint.GetHeight());
  
     _paint.Clear(UNCOLORED);
   _paint.DrawStringAt(0, 0, LINE_7, &Font16, COLORED);
   _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 10, _paint.GetWidth(), _paint.GetHeight());
  
  //
  //  // Dynamic text
  //  _paint.SetWidth(24);
  //  _paint.SetHeight(40);
  //  _paint.SetRotate(ROTATE_180);
  //
  //  // Max UV day 0
  //  _paint.Clear(UNCOLORED);
  //  single_digit[0] = max_uv % 100 % 10 + '0';
  //  _paint.DrawStringAt(0, 0, single_digit, &Font16, COLORED);  // UV
  //  _epd.SetFrameMemory_Partial(_paint.GetImage(), 8, 140, _paint.GetWidth(), _paint.GetHeight());
  //
  //  // Max temp day 0
  //  _paint.Clear(UNCOLORED);
  //  if (max_temp > 9) {
  //    double_digit[0] = max_temp % 100 / 10 + '0';
  //    double_digit[1] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, double_digit, &Font16, COLORED);
  //  } else {
  //    single_digit[0] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, single_digit, &Font16, COLORED);
  //  }
  //  _epd.SetFrameMemory_Partial(_paint.GetImage(), 8, 120, _paint.GetWidth(), _paint.GetHeight());
  //
  //  // Burn time
  //  _paint.Clear(UNCOLORED);
  //  double_digit[0] = burn_time % 100 / 10 + '0';
  //  double_digit[1] = burn_time % 100 % 10 + '0';
  //  _paint.DrawStringAt(0, 0, double_digit, &Font16, COLORED);
  //  _epd.SetFrameMemory_Partial(_paint.GetImage(), 8, 100, _paint.GetWidth(), _paint.GetHeight());
  //
  //  _paint.SetWidth(72);
  //  _paint.SetHeight(40);
  //  _paint.SetRotate(ROTATE_180);
  //
  //  // Day 1
  //  _paint.Clear(UNCOLORED);
  //  max_temp = (_falcon->getTempMax(1));
  //  max_uv = (_falcon->getUVMax(1));
  //  if (max_temp > 9) {
  //    four_digit[0] = max_uv % 100 % 10 + '0';
  //    four_digit[2] = max_temp % 100 / 10 + '0';
  //    four_digit[3] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, four_digit, &Font16, COLORED);
  //  } else {
  //    three_digit[0] = max_uv % 100 % 10 + '0';
  //    three_digit[2] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, three_digit, &Font16, COLORED);
  //  }
  //  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 80, _paint.GetWidth(), _paint.GetHeight());
  //
  //  // Day 2
  //  _paint.Clear(UNCOLORED);
  //  max_temp = (_falcon->getTempMax(2));
  //  max_uv = (_falcon->getUVMax(2));
  //  if (max_temp > 9) {
  //    four_digit[0] = max_uv % 100 % 10 + '0';
  //    four_digit[2] = max_temp % 100 / 10 + '0';
  //    four_digit[3] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, four_digit, &Font16, COLORED);
  //  } else {
  //    three_digit[0] = max_uv % 100 % 10 + '0';
  //    three_digit[2] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, three_digit, &Font16, COLORED);
  //  }
  //  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 60, _paint.GetWidth(), _paint.GetHeight());
  //
  //  // Day 3
  //  _paint.Clear(UNCOLORED);
  //  max_temp = (_falcon->getTempMax(3));
  //  max_uv = (_falcon->getUVMax(3));
  //  if (max_temp > 9) {
  //    four_digit[0] = max_uv % 100 % 10 + '0';
  //    four_digit[2] = max_temp % 100 / 10 + '0';
  //    four_digit[3] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, four_digit, &Font16, COLORED);
  //  } else {
  //    three_digit[0] = max_uv % 100 % 10 + '0';
  //    three_digit[2] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, three_digit, &Font16, COLORED);
  //  }
  //  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 40, _paint.GetWidth(), _paint.GetHeight());
  //
  //  // Day 4
  //  _paint.Clear(UNCOLORED);
  //  max_temp = (_falcon->getTempMax(4));
  //  max_uv = (_falcon->getUVMax(4));
  //  if (max_temp > 9) {
  //    four_digit[0] = max_uv % 100 % 10 + '0';
  //    four_digit[2] = max_temp % 100 / 10 + '0';
  //    four_digit[3] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, four_digit, &Font16, COLORED);
  //  } else {
  //    three_digit[0] = max_uv % 100 % 10 + '0';
  //    three_digit[2] = max_temp % 100 % 10 + '0';
  //    _paint.DrawStringAt(0, 0, three_digit, &Font16, COLORED);
  //  }
  //  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 20, _paint.GetWidth(), _paint.GetHeight());
  //
  //  // Status indicators
  //  _paint.SetWidth(120);
  //  _paint.SetHeight(40);
  //  _paint.SetRotate(ROTATE_180);
  //
  //  _paint.Clear(UNCOLORED);
  //  if (demoOn) {
  //    _paint.DrawStringAt(0, 0, "Demo Mode", &Font16, COLORED);
  //  }
  //  else if (audioOn && wifiOn) {
  //    _paint.DrawStringAt(0, 0, "Wifi Audio", &Font16, COLORED);
  //  }
  //  else if (wifiOn) {
  //    _paint.DrawStringAt(0, 0, "Wifi", &Font16, COLORED);
  //  }
  //  else if (audioOn) {
  //    _paint.DrawStringAt(0, 0, "Audio", &Font16, COLORED);
  //  }
  //  else {
  //    _paint.DrawStringAt(0, 0, "", &Font16, COLORED);
  //  }
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 0, _paint.GetWidth(), _paint.GetHeight());

  _epd.DisplayFrame_Partial();
}
