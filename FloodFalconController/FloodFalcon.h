#ifndef _FLOOD_FALCON_H_
#define _FLOOD_FALCON_H_

#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_Soundboard.h>
#include "falcon_config.h"

#define DATESTR_LEN 17  // "2022-12-19T15:20:31" -> "2022-12-19 15:20"
#define FLOOD_AREA_LEN 12 // Flood area description

enum flap_speeds {VFAST = 1, FAST = 2, SLOW = 4, VSLOW = 8};

enum warning_levels {NONE, SEVERE_FLOOD_WARNING, FLOOD_WARNING, FLOOD_ALERT, NO_LONGER, INIT};

struct floodWarning {
    char time_raised[DATESTR_LEN] = {'\0'};
    int severityLevel = 0;
    char flood_area_id[FLOOD_AREA_LEN] = {'\0'};
};

class FloodFalcon {
  public:
    floodWarning * _warning; // Flood warning data
    int state;
    FloodFalcon(Adafruit_Soundboard *sfx, Adafruit_PWMServoDriver *pwm, floodWarning *warning);
 private:
    Adafruit_PWMServoDriver *_pwm;
    Adafruit_Soundboard *_sfx;
    int _servo = 0;  // default servo 0
    uint16_t _pulselen = WINGS_START;
 public:
    void init(int servo, uint16_t pos);
    int doAction(boolean audio);
    int updateState();
    void StartPos(uint16_t start_pos);
    void Flap(uint16_t down_pos, uint16_t up_pos, int speed_idx, int flaps);
    void PassOut(uint16_t end_pos, int speed_idx);
    void Dead(uint16_t end_pos, int speed_idx);
    void Tweet(uint8_t track, boolean audio);
};

#endif
