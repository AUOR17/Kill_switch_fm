#pragma once
#include <Arduino.h>

// ======= Modelo de salida (igual al que usa tu unit_test) =======
struct WheelSample {
  uint8_t  wheel;       // 1..4
  float    speed_kph;   // km/h calculada en la ventana
  uint16_t pulses;      // pulsos contados en la ventana
  uint32_t ts_ms;       // timestamp de muestreo
};

// ======= API =======
void wheels_setup();

/**
 * Llena 4 muestras (w=1..4) cuando se cumple la ventana (1/WHEELS_HZ).
 * Retorna true si entregó un lote; false si aún no toca muestrear.
 */
bool wheels_read_batch(WheelSample out[4]);

/** Calibración runtime (opcional).  */
void wheels_set_calibration(uint16_t pulses_per_rev, float wheel_diameter_m);

/** Obtén la calibración actual (por si quieres imprimirla). */
void wheels_get_calibration(uint16_t &pulses_per_rev, float &wheel_diameter_m);
