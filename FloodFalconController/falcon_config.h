// Your WiFi settings
#define SECRET_SSID "your-ssid"
#define SECRET_PASS "your-wifi-password"

// Your flood area code from Gov.co.uk Flood Warning Service
#define AREA_CODE "065WAF441"

// Your serveo / wing positions - adjust as required
// If the servo is chattering at the end positions,
// adjust the min or max value by 5 ish
#define WINGS_START 470
#define WINGS_DOWN 470
#define WINGS_UP_A_BIT 420
#define WINGS_UP_A_LOT 400

// Your time intervals
#define ALERT_INTERVAL 15 * 60 * 1000  // 15 mins
#define DEMO_INTERVAL 10 * 1000        // 10 sec

// Audio track numbers
// #define NONE 0                   // T00.ogg
// #define SEVERE_FLOOD_WARNING 1   // T01.ogg
// #define FLOOD_WARNING 2          // T02.ogg
// #define FLOOD_ALERT 3            // T03.ogg
// #define FLOOD_WARNING_REMOVED 4  // T04.ogg
// #define INIT 5                   // T05.ogg
