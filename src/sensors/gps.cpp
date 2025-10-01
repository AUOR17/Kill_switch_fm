#include "gps.h"
#include <TinyGPSPlus.h>
#include <config.h>

static TinyGPSPlus gps;

void gps_setup() {
  Serial2.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

bool gps_read(GPSFix& out) {
  while (Serial2.available()) gps.encode(Serial2.read());
  if (!gps.location.isValid()) return false;

  out.lat     = gps.location.lat();
  out.lon     = gps.location.lng();
  out.alt     = gps.altitude.meters();
  out.spd_kph = gps.speed.kmph();
  out.hdop    = gps.hdop.hdop();   // puede ser 0 si no lo emite tu módulo
  out.fix     = 3;                 // simplificado
  return true;
}
