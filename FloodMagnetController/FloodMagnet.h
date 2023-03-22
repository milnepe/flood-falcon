#ifndef _FLOOD_MAGNET_H_
#define _FLOOD_MAGNET_H_

#include "magnet_config.h"

#define DATESTR_LEN 17  // "2022-12-19T15:20:31" -> "2022-12-19 15:20"
#define FLOOD_AREA_LEN 12 // Flood area description

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
    FloodMagnet(floodWarning *warning);
 public:
    void init();
    int updateState();
};

#endif
