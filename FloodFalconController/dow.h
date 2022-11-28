#ifndef _FALCON_DOW_H_
#define _FALCON_DOW_H_

#define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ))     // from time-lib

uint16_t getYearInt(const char* datestr) {
  char year_bit[4] = {0};
  memcpy(year_bit, datestr + 0 /* Offset */, 4 /* Length */);
  return atoi(year_bit);
}

uint8_t getMonthInt(const char* datestr) {
  char month_bit[2] = {0};
  memcpy(month_bit, datestr + 5, 2);
  return atoi(month_bit);
}

uint8_t getDayInt(const char* datestr) {
  char day_bit[2] = {0};
  memcpy(day_bit, datestr + 8, 2);
  return (uint8_t)atoi(day_bit);
}

int dayOfWeek(uint16_t year, uint8_t month, uint8_t day)
{
  uint16_t months[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
  };   // days until 1st of month

  uint32_t days = year * 365;        // days until year
  for (uint16_t i = 4; i < year; i += 4) if (LEAP_YEAR(i) ) days++;     // adjust leap years, test only multiple of 4 of course

  days += months[month - 1] + day;  // add the days of this year
  if ((month > 2) && LEAP_YEAR(year)) days++;  // adjust 1 if this year is a leap year, but only after febr

  return days % 7;   // remove all multiples of 7
}
#endif
