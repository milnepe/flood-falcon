#include "FloodFalconDisplay.h"

// Flood warning text
static char w1[5][12] = { "No Flood", "Severe", "Flood", "Flood", "Warning no" };
static char w2[5][12] = { "Warnings", "Flood", "Warning", "Alert", "Longer in" };
static char w3[5][12] = { "", "Warning", "", "", "Force" };

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

void FloodFalconDisplay::updateDisplay() {
  Serial.println("Updating display...");
  int severityLevel = _falcon->_warning->severityLevel;
  // Index warning string based on severity level
  //int warning_idx = severityLevel ? severityLevel : 0;
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
    case NONE:
      _epd.SetFrameMemory_Base(epd_flood_warning_removed);
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
      _epd.SetFrameMemory_Base(epd_flood_warning_removed);
      break;
    default:
      break;
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
  _paint.DrawStringAt(0, 0, w1[severityLevel], &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 120, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, w2[severityLevel], &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 100, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, w3[severityLevel], &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 80, _paint.GetWidth(), _paint.GetHeight());

  // _paint.Clear(UNCOLORED);
  // _paint.DrawStringAt(0, 0, LINE_4, &Font16, COLORED);
  // _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 60, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, "Updated", &Font16, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 40, _paint.GetWidth(), _paint.GetHeight());

  _paint.Clear(UNCOLORED);
  _paint.DrawStringAt(0, 0, (_falcon->_warning->time_raised ? _falcon->_warning->time_raised : ""), &Font12, COLORED);
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 20, _paint.GetWidth(), _paint.GetHeight());

  // _paint.Clear(UNCOLORED);
  // _paint.DrawStringAt(0, 0, LINE_7, &Font16, COLORED);
  // _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 10, _paint.GetWidth(), _paint.GetHeight());

  // Status indicators
  _paint.SetWidth(120);
  _paint.SetHeight(40);
  _paint.SetRotate(ROTATE_180);

  _paint.Clear(UNCOLORED);
  if (demoOn) {
    _paint.DrawStringAt(0, 0, "Demo Mode", &Font16, COLORED);
  } else if (audioOn && wifiOn) {
    _paint.DrawStringAt(0, 0, "Wifi Audio", &Font16, COLORED);
  } else if (wifiOn) {
    _paint.DrawStringAt(0, 0, "Wifi", &Font16, COLORED);
  } else if (audioOn) {
    _paint.DrawStringAt(0, 0, "Audio", &Font16, COLORED);
  } else {
    _paint.DrawStringAt(0, 0, "", &Font16, COLORED);
  }
  _epd.SetFrameMemory_Partial(_paint.GetImage(), 0, 0, _paint.GetWidth(), _paint.GetHeight());

  _epd.DisplayFrame_Partial();
}
