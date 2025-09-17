#include <Arduino.h>
#include <TinyGPSPlus.h>

#define RX_PIN 16
#define TX_PIN 17
#define GPS_BAUD 9600   // usa el que te funcionó en RAW

TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n[TinyGPS++ DIAG] iniciando...");

  // (opcional, ayuda si tu GPS habla mucho)
  Serial2.setRxBufferSize(1024);

  Serial2.begin(GPS_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop() {
  // 1) Consumir TODO lo que llegue del GPS
  while (Serial2.available()) {
    gps.encode(Serial2.read());
  }

  // 2) Cada 1 s, reportar estado — AUNQUE NO HAYA FIX
  static uint32_t last = 0;
  if (millis() - last >= 1000) {
    last = millis();

    // Métricas para saber si TinyGPS++ está recibiendo algo
    Serial.printf("[diag] chars=%u, good=%u, failed=%u, sentencesWithFix=%u\n",
                  gps.charsProcessed(),
                  gps.passedChecksum(),
                  gps.failedChecksum(),
                  gps.sentencesWithFix());

    if (gps.location.isValid()) {
      Serial.printf("loc: lat=%.6f lon=%.6f  spd=%.2f km/h  alt=%.1f m  hdop=%.2f  sats=%u  fix=YES\n",
                    gps.location.lat(),
                    gps.location.lng(),
                    gps.speed.kmph(),
                    gps.altitude.meters(),
                    gps.hdop.hdop(),
                    gps.satellites.value());
    } else {
      // No hay fix todavía, pero mostramos info útil
      Serial.printf("loc: (sin fix)  sats=%u  hdop=%.2f  time=%02d:%02d:%02d UTC\n",
                    gps.satellites.value(),
                    gps.hdop.hdop(),
                    gps.time.hour(), gps.time.minute(), gps.time.second());
    }

    // Si después de 5 s no procesó nada, probablemente es BAUD o wiring
    static uint32_t t0 = millis();
    if (millis() - t0 > 5000 && gps.charsProcessed() < 10) {
      Serial.println(">> No llegan NMEA a TinyGPS++ (¿baud equivocado? ¿pines TX/RX?)");
    }
  }
}
