#ifndef _FLOOD_MAGNET_H_
#define _FLOOD_MAGNET_H_

#include <Adafruit_Soundboard.h>
#include "magnet_config.h"

#define DATESTR_LEN 17  // "2022-12-19T15:20:31" -> "2022-12-19 15:20"
#define FLOOD_AREA_LEN 12 // Flood area description

enum flap_speeds {VFAST = 1, FAST = 2, SLOW = 4, VSLOW = 8};

enum warning_levels {NONE, SEVERE_FLOOD_WARNING, FLOOD_WARNING, FLOOD_ALERT, NO_LONGER, INIT};

struct floodWarning {
    char time_raised[DATESTR_LEN] = {'\0'};
    int severityLevel = 0;
    char flood_area_id[FLOOD_AREA_LEN] = {'\0'};
};

class FloodMagnet {
  public:
    floodWarning * _warning; // Flood warning data
    int state;
    FloodMagnet(Adafruit_Soundboard *sfx, floodWarning *warning);
 private:
    Adafruit_Soundboard *_sfx;
 public:
    void init();
    int doAction(boolean audio);
    int updateState();
    void Tweet(uint8_t track, boolean audio);
};

#endif
