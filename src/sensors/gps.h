#pragma once
#include <Arduino.h>

struct GPSFix {
  double lat, lon, alt;
  float hdop;
  int   fix;      // 0,2,3
  double spd_kph;
};

void gps_setup();             
bool gps_read(GPSFix& out);   
