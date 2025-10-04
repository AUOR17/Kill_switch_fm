#include <Arduino.h>
#include <algorithm>
#include "config.h"

// --- HALs de tu src/ ---
#include "actuators/motor.h"
#include "sensors/wheels.h"
#include "sensors/gps.h"

// ---------- Helpers ----------
static void wait_for_serial() {
  Serial.begin(USB_BAUD);           // 115200 recomendado en config.h
  uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 2000) { delay(10); }
  delay(200);
}

static void println_hr() {
  Serial.println(F("--------------------------------------------"));
}

static void show_menu() {
  println_hr();
  Serial.println(F("========== KS UNIT TESTS =========="));
  Serial.println(F("1) PWM motores (LEDC)"));
  Serial.println(F("2) Ruedas/Encoders (kph/pulses)"));
  Serial.println(F("3) GPS (TinyGPS++ diag)"));
  Serial.println(F("------------------------------------"));
  Serial.println(F("q) Salir / volver al menu"));
  Serial.print(F("Selecciona una opcion: "));
}

static bool key_pressed(char &out) {
  if (Serial.available()) {
    out = (char)Serial.read();
    return true;
  }
  return false;
}

// ---------- (1) Test PWM 4 canales ----------
static void test_pwm() {
  println_hr();
  Serial.println(F("[PWM] Inicializando LEDC..."));
  motor_setup();

  Serial.println(F("[PWM] Controles:"));
  Serial.println(F(" - Teclas 1..4: seleccionar canal activo"));
  Serial.println(F(" - Teclas [+] y [-]: subir/bajar duty del canal activo (pasos 5%)"));
  Serial.println(F(" - Tecla [a]: aplicar duty a TODOS los canales"));
  Serial.println(F(" - Tecla [s]: sweep auto (0% -> 100% -> 0%)"));
  Serial.println(F(" - Tecla [0]: duty = 0% a todos"));
  Serial.println(F(" - Tecla [q]: salir"));
  println_hr();

  uint8_t active = 1;                 // canal activo 1..4
  uint16_t duty = 0;                  // 0..(2^PWM_RES_BITS -1)
  const uint16_t DUTY_MAX = (1 << PWM_RES_BITS) - 1;
  auto pct_to_duty = [&](int pct){ 
    if (pct < 0) pct = 0; if (pct > 100) pct = 100;
    return (uint16_t)((pct * DUTY_MAX) / 100);
  };

  // estado inicial: todo en 0
  for (int i=1; i<=4; ++i) motor_set(i, 0);
  Serial.printf("[PWM] Canal activo: %u | Duty: %u/%u (~%d%%) | Freq=%lu Hz | Res=%u bits\n",
                active, duty, DUTY_MAX, (int)(100.0f * duty / DUTY_MAX), (unsigned long)PWM_FREQ, PWM_RES_BITS);

  bool sweep_mode = false;
  int sweep_dir = +1;      // +1 sube, -1 baja
  uint32_t lastSweep = millis();

  while (true) {
    char c;
    if (key_pressed(c)) {
      if (c == 'q' || c == 'Q') {
        Serial.println(F("\n[PWM] Fin de test."));
        // apaga todo por seguridad
        for (int i=1; i<=4; ++i) motor_set(i, 0);
        break;
      }
      if (c >= '1' && c <= '4') {
        active = (uint8_t)(c - '0');
        Serial.printf("[PWM] Canal activo -> %u\n", active);
      } else if (c == '+') {
        sweep_mode = false;
        int step = DUTY_MAX / 20; // ~5%
        duty = static_cast<uint16_t>(std::min<int>(DUTY_MAX, duty + step));
        motor_set(active, duty);
        Serial.printf("[PWM] Canal %u duty -> %u/%u (~%d%%)\n", active, duty, DUTY_MAX, (int)(100.0f * duty / DUTY_MAX));
      } else if (c == '-') {
        sweep_mode = false;
        int step = DUTY_MAX / 20;
        duty = static_cast<uint16_t>(std::max<int>(0, duty - step));
        motor_set(active, duty);
        Serial.printf("[PWM] Canal %u duty -> %u/%u (~%d%%)\n", active, duty, DUTY_MAX, (int)(100.0f * duty / DUTY_MAX));
      } else if (c == 'a' || c == 'A') {
        // aplica duty actual a todos
        for (int i=1; i<=4; ++i) motor_set(i, duty);
        Serial.printf("[PWM] ALL = %u/%u (~%d%%)\n", duty, DUTY_MAX, (int)(100.0f * duty / DUTY_MAX));
      } else if (c == '0') {
        sweep_mode = false;
        duty = 0;
        for (int i=1; i<=4; ++i) motor_set(i, 0);
        Serial.println(F("[PWM] ALL = 0"));
      } else if (c == 's' || c == 'S') {
        sweep_mode = true;
        Serial.println(F("[PWM] Sweep AUTO ON (0%↔100%)"));
      }
    }

    if (sweep_mode && millis() - lastSweep >= 30) {
      lastSweep = millis();
      int step = DUTY_MAX / 100; // ~1%
      duty = (uint16_t) constrain((int)duty + sweep_dir * step, 0, (int)DUTY_MAX);
      for (int i=1; i<=4; ++i) motor_set(i, duty);
      if (duty == 0 || duty == DUTY_MAX) sweep_dir = -sweep_dir;
    }

    delay(2);
  }
}

// ---------- (2) Test Ruedas/Encoders ----------
static void test_wheels() {
  println_hr();
  Serial.println(F("[WHEELS] Inicializando..."));
  wheels_setup();
  Serial.println(F("[WHEELS] Leyendo batch (4 ruedas). 'q' para salir.\n"));

  WheelSample batch[4];
  uint32_t lastPrint = 0;

  while (true) {
    char c;
    if (key_pressed(c) && (c == 'q' || c == 'Q')) {
      Serial.println(F("\n[WHEELS] Fin de test."));
      break;
    }
    // wheels_read_batch gobierna la ventana según WHEELS_HZ
    if (wheels_read_batch(batch)) {
      if (millis() - lastPrint > 200) {
        lastPrint = millis();
        for (int i=0; i<4; ++i) {
          Serial.printf("w%u: %5.1f kph  pulses=%3u  ts=%lu  |  ",
                        batch[i].wheel, batch[i].speed_kph, batch[i].pulses, (unsigned long)batch[i].ts_ms);
        }
        Serial.println();
      }
    }
    delay(5);
  }
}

// ---------- (3) Test GPS (TinyGPS++ diag) ----------
static void test_gps() {
  println_hr();
  Serial.println(F("[GPS] Inicializando..."));
  gps_setup();
  Serial.printf("[GPS] UART2: RX=%d TX=%d @ %lu bps\n", GPS_RX_PIN, GPS_TX_PIN, (unsigned long)GPS_BAUD);
  Serial.println(F("[GPS] 'q' para salir. (Recuerda: bajo techo no da FIX)\n"));

  GPSFix fix{};
  uint32_t last = 0;

  while (true) {
    char c;
    if (key_pressed(c) && (c == 'q' || c == 'Q')) {
      Serial.println(F("\n[GPS] Fin de test."));
      break;
    }
    if (gps_read(fix) && millis() - last > 1000) {
      last = millis();
      Serial.printf("lat=%.6f lon=%.6f spd=%.1fkm/h alt=%.1fm hdop=%.2f fix=%d\n",
                    fix.lat, fix.lon, fix.spd_kph, fix.alt, fix.hdop, fix.fix);
    }
    delay(5);
  }
}

// ---------- Entradas ----------
void setup() {
  wait_for_serial();
  show_menu();
}

void loop() {
  if (!Serial.available()) { delay(10); return; }
  char c = (char)Serial.read();
  switch (c) {
    case '1': test_pwm();      show_menu(); break;
    case '2': test_wheels();   show_menu(); break;
    case '3': test_gps();      show_menu(); break;
    case '\n':
    case '\r':
    case ' ': break;
    default:
      Serial.println(F("\nOpcion no valida."));
      show_menu();
      break;
  }
}
