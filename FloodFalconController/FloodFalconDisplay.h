#ifndef _FLOOD_FALCON_DISPLAY_H_
#define _FLOOD_FALCON_DISPLAY_H_

#include "FloodFalcon.h"
#include "epd2in9_V2.h"
#include "epdpaint.h"
#include "img/rslogo.h"

#include "img/flood_alert_none.h"  // Level 0
#include "img/flood_warning_severe.h"  // Level 1
#include "img/flood_warning.h"  // Level 2
#include "img/flood_alert.h"  // Level 3
#include "img/flood_warning_no_longer.h"  // Lvel 4

#define COLORED     0
#define UNCOLORED   1

class FloodFalconDisplay {
  public:
  bool wifiOn = false;
  bool demoOn = false;
  bool audioOn = true;
  unsigned char image[1024];
  Epd _epd; // default reset: 8, dc: 9, cs: 10, busy: 7
  Paint _paint = Paint(image, 0, 0);
  FloodFalcon* _falcon;

  FloodFalconDisplay(FloodFalcon* falcon) : _falcon(falcon) {};
  void initDisplay(void);
  void updateDisplay(void);
  void showGreeting(void);
};

#endif
