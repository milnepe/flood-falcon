#ifndef _FLOOD_API_DISPLAY_H_
#define _FLOOD_API_DISPLAY_H_

#include "FloodAPI.h"
#include "epd2in9_V2.h"
#include "epdpaint.h"
#include "img/rslogo.h"

// Image converter https://javl.github.io/image2cpp/ 
#include "img/flood_warning_removed.h"    // Level 0
#include "img/flood_warning_severe.h"     // Level 1
#include "img/flood_warning.h"            // Level 2
#include "img/flood_alert.h"              // Level 3
#include "img/flood_warning_removed.h"    // Level 4

#define COLORED     0
#define UNCOLORED   1



class FloodMagnetDisplay {
  public:
  bool wifiOn = false;
  bool demoOn = false;
  unsigned char image[1024];
  Epd _epd; // default reset: 8, dc: 9, cs: 10, busy: 7
  Paint _paint = Paint(image, 0, 0);
  FloodAPI* _magnet;

  FloodMagnetDisplay(FloodAPI* magnet) : _magnet(magnet) {};
  void initDisplay(void);
  void updateDisplay(void);
  void showGreeting(void);
};

#endif
