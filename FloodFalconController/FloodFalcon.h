#ifndef _FLOOD_FALCON_H_
#define _FLOOD_FALCON_H_

#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_Soundboard.h>

// Wing positions - adjust as required
// If the servo is chattering at the end positions,
// adjust the min or max value by 5ish
#define WINGS_START 470
#define WINGS_DOWN 460  // Max position
#define WINGS_UP_A_BIT 420
#define WINGS_UP_A_LOT 400
#define PASS_OUT_POS 380
#define DEAD_POS 350  // Min position

// Audio track numbers
#define CHEEP_CHEEP 0           // T00.ogg
#define NO_PROTECTION_A 1       // T01.ogg
#define NO_PROTECTION_B 2       // T02.ogg
#define SOME_PROTECTION 3       // T03.ogg
#define PROTECTION_ESSENTIAL 4  // T04.ogg
#define EXTRA_PROTECTION 5      // T05.ogg

#define DATESTR_LEN 12  // "2022-06-15Z"
#define DESCSTR_LEN 128 // Flood area description

enum flap_speeds {VFAST = 1, FAST, SLOW, VSLOW};

struct floodWarning {
    char datestr[DATESTR_LEN] = {'\0'};
    int items_currentWarning_severityLevel = 0;
    char items_description[DESCSTR_LEN] = {'\0'};
};

class FloodFalcon {
  public:
    floodWarning * _warning; // Flood warning data
    int state = 0;
    FloodFalcon(Adafruit_Soundboard *sfx, Adafruit_PWMServoDriver *pwm, floodWarning *warning);
 private:
    Adafruit_PWMServoDriver *_pwm;
    Adafruit_Soundboard *_sfx;
    int _servo = 0;  // default servo 0
    uint16_t _pulselen = WINGS_START;
 public:
    void init(int servo, uint16_t pos);
    const char* dow(int i);
    int doAction(boolean audio);
    int updateState();
    int getUVMax(int day_idx);
    int getTempMax(int day_idx);
    void StartPos(uint16_t start_pos);
    void Flap(uint16_t down_pos, uint16_t up_pos, int speed_idx, int flaps);
    void PassOut(uint16_t end_pos, int speed_idx);
    void Dead(uint16_t end_pos, int speed_idx);
    void Tweet(uint8_t track, boolean audio);
};

#endif
