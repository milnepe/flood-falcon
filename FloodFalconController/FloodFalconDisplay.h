#ifndef _FLOOD_FALCON_DISPLAY_H_
#define _FLOOD_FALCON_DISPLAY_H_

#include "FloodFalcon.h"
#include "epd2in9_V2.h"
#include "epdpaint.h"
#include "img/rslogo.h"
#include "img/sun_1.h"
#include "img/sun_2.h"
#include "img/sun_3.h"
#include "img/sun_4.h"
#include "img/sun_5.h"

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
