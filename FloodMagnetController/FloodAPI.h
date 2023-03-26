#ifndef _FLOOD_API_H_
#define _FLOOD_API_H_

#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "magnet_config.h"
#include "led.h"

#define DATESTR_LEN 17  // "2022-12-19T15:20:31" -> "2022-12-19 15:20"
#define FLOOD_AREA_LEN 12 // Flood area description

enum warning_levels {INIT = -1, NONE, SEVERE_FLOOD_WARNING, FLOOD_WARNING, FLOOD_ALERT, NO_LONGER};

struct floodWarning {
    char time_raised[DATESTR_LEN] = {'\0'};
    int severityLevel = 0;
    char flood_area_id[FLOOD_AREA_LEN] = {'\0'};
};

class FloodAPI {
  public:
    floodWarning warning; // Flood warning data
    int state;
    FloodAPI();
 public:
    void init();
    int updateState(int state);
    void getData();
    void demo();
};

#endif
