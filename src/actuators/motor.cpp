#include "motor.h"
#include "config.h"

// Mapeo de pines (según tu tabla):
// DRV8833 #1  -> M1 (AIN1=pwm, AIN2=dir)  |  M2 (BIN1=pwm, BIN2=dir)
// DRV8833 #2  -> M3 (AIN1=pwm, AIN2=dir)  |  M4 (BIN1=pwm, BIN2=dir)

static const int PWM_PINS[4] = {
  DRV1_AIN1,   // M1 PWM
  DRV1_BIN1,   // M2 PWM
  DRV2_AIN1,   // M3 PWM
  DRV2_BIN1    // M4 PWM
};

static const int DIR_PINS[4] = {
  DRV1_AIN2,   // M1 DIR
  DRV1_BIN2,   // M2 DIR
  DRV2_AIN2,   // M3 DIR
  DRV2_BIN2    // M4 DIR
};

// 4 canales LEDC (0..3)
static const int LEDC_CH[4] = {0, 1, 2, 3};

static inline uint16_t dutyMax() {
  return (1u << PWM_RES_BITS) - 1u;
}

void motor_enable(bool en) {
  pinMode(DRV_STBY, OUTPUT);
  digitalWrite(DRV_STBY, en ? HIGH : LOW);
}

void motor_setup() {
  // Habilitar drivers
  motor_enable(true);

  // Configurar pines y PWM
  for (int i = 0; i < 4; ++i) {
    pinMode(DIR_PINS[i], OUTPUT);
    digitalWrite(DIR_PINS[i], LOW);              // forward por defecto

    ledcSetup(LEDC_CH[i], PWM_FREQ, PWM_RES_BITS);
    ledcAttachPin(PWM_PINS[i], LEDC_CH[i]);
    ledcWrite(LEDC_CH[i], 0);                    // duty 0 (coast)
  }
}

void motor_set_dir(uint8_t idx, bool forward) {
  if (idx < 1 || idx > 4) return;
  // Usamos el pin DIR como selección de sentido:
  //   DIR LOW  = forward
  //   DIR HIGH = reverse
  digitalWrite(DIR_PINS[idx - 1], forward ? LOW : HIGH);
}

void motor_set(uint8_t idx, uint16_t duty_raw) {
  if (idx < 1 || idx > 4) return;
  if (duty_raw > dutyMax()) duty_raw = dutyMax();
  ledcWrite(LEDC_CH[idx - 1], duty_raw);
}

void motor_set_pct(uint8_t idx, int pct) {
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  const uint16_t d = (uint16_t)((pct * dutyMax()) / 100);
  motor_set(idx, d);
}

void motor_brake(uint8_t idx) {
  if (idx < 1 || idx > 4) return;
  // Freno: DIR=HIGH y duty al máximo
  digitalWrite(DIR_PINS[idx - 1], HIGH);
  ledcWrite(LEDC_CH[idx - 1], dutyMax());
}

void motor_coast(uint8_t idx) {
  if (idx < 1 || idx > 4) return;
  digitalWrite(DIR_PINS[idx - 1], LOW);
  ledcWrite(LEDC_CH[idx - 1], 0);
}

void motor_all_off() {
  for (int i = 1; i <= 4; ++i) motor_coast(i);
}
