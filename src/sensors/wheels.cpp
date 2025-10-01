#include "wheels.h"
#include "config.h"

// ======= Calibración por defecto (puedes sobreescribir desde ini o runtime) =======
#ifndef WHEEL_PULSES_PER_REV
#define WHEEL_PULSES_PER_REV  20    // <-- cámbialo a los PPV reales de tu sensor/rueda
#endif
#ifndef WHEEL_DIAMETER_M
#define WHEEL_DIAMETER_M      0.30f // 30 cm de diámetro (ejemplo); ajusta a tu rueda
#endif

// Pines (vienen de config.h)
static const int PIN_W1 = SPEED_S1_PIN;  // 25 (con PU interna)
static const int PIN_W2 = SPEED_S2_PIN;  // 26 (con PU interna)
static const int PIN_W3 = SPEED_S3_PIN;  // 36 (input-only)
static const int PIN_W4 = SPEED_S4_PIN;  // 39 (input-only)

// Estado interno
static volatile uint32_t s_pulses[4] = {0,0,0,0};
static uint32_t s_lastWindowMs = 0;
static uint16_t s_ppv = WHEEL_PULSES_PER_REV;
static float    s_diam_m = WHEEL_DIAMETER_M;

// ======= ISRs =======
void IRAM_ATTR isr_w1() { s_pulses[0]++; }
void IRAM_ATTR isr_w2() { s_pulses[1]++; }
void IRAM_ATTR isr_w3() { s_pulses[2]++; }
void IRAM_ATTR isr_w4() { s_pulses[3]++; }

// ======= Helpers =======
static inline float circumference_m() {
  return PI * s_diam_m;
}

static float pulses_to_kph(uint32_t pulses, float dt_ms) {
  if (s_ppv == 0 || dt_ms <= 0) return 0.0f;

  // vueltas en la ventana
  const float revs = (float)pulses / (float)s_ppv;
  // distancia (m) recorrida
  const float dist_m = revs * circumference_m();
  // velocidad m/s
  const float v_ms = dist_m / (dt_ms / 1000.0f);
  // a km/h
  return v_ms * 3.6f;
}

// ======= API =======
void wheels_set_calibration(uint16_t pulses_per_rev, float wheel_diameter_m) {
  if (pulses_per_rev > 0) s_ppv = pulses_per_rev;
  if (wheel_diameter_m > 0) s_diam_m = wheel_diameter_m;
}

void wheels_get_calibration(uint16_t &pulses_per_rev, float &wheel_diameter_m) {
  pulses_per_rev = s_ppv;
  wheel_diameter_m = s_diam_m;
}

void wheels_setup() {
  // Entradas:
  // 25/26 soportan PULLUP interno; 36/39 son input-only (sin pullups internos).
  pinMode(PIN_W1, INPUT_PULLUP);
  pinMode(PIN_W2, INPUT_PULLUP);
  pinMode(PIN_W3, INPUT);   // si tu sensor es open-collector, usa resistencia externa 10k a 3V3
  pinMode(PIN_W4, INPUT);

  // Limpia contadores
  noInterrupts();
  s_pulses[0] = s_pulses[1] = s_pulses[2] = s_pulses[3] = 0;
  interrupts();

  // Selecciona flanco según tu sensor (RISING suele funcionar si el sensor entrega pulso alto)
  attachInterrupt(digitalPinToInterrupt(PIN_W1), isr_w1, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_W2), isr_w2, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_W3), isr_w3, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_W4), isr_w4, RISING);

  s_lastWindowMs = millis();
}

bool wheels_read_batch(WheelSample out[4]) {
  const uint32_t now = millis();
  const uint32_t period_ms = (WHEELS_HZ > 0) ? (1000u / (uint32_t)WHEELS_HZ) : 100;

  if (now - s_lastWindowMs < period_ms) {
    return false;  // Aún no toca muestrear
  }

  // Tomamos snapshot atómico de los contadores y los reiniciamos
  uint32_t snap[4];
  noInterrupts();
  for (int i=0; i<4; ++i) { snap[i] = s_pulses[i]; s_pulses[i] = 0; }
  interrupts();

  const float dt_ms = (float)(now - s_lastWindowMs);
  s_lastWindowMs = now;

  // Calcula velocidades
  for (int i=0; i<4; ++i) {
    out[i].wheel     = (uint8_t)(i+1);
    out[i].pulses    = (uint16_t) min<uint32_t>(snap[i], 0xFFFF);
    out[i].speed_kph = pulses_to_kph(snap[i], dt_ms);
    out[i].ts_ms     = now;
  }
  return true;
}
