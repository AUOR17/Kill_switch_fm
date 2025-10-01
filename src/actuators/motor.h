#pragma once
#include <Arduino.h>

// Inicializa LEDC, pines y saca STBY del DRV8833
void motor_setup();

// Habilita/inhabilita ambos DRV8833 (pin STBY compartido)
void motor_enable(bool en);

// Cambia dirección del motor idx (1..4). forward=true=fwd, false=rev
void motor_set_dir(uint8_t idx, bool forward);

// Escribe duty raw al motor idx (0..(2^PWM_RES_BITS-1))
void motor_set(uint8_t idx, uint16_t duty_raw);

// Escribe duty en porcentaje (0..100)
void motor_set_pct(uint8_t idx, int pct);

// Freno “rápido” (IN de dirección alto y PWM 100%)
void motor_brake(uint8_t idx);

// Libre (coast): IN de dirección LOW y PWM 0
void motor_coast(uint8_t idx);

// Apaga todos los motores (coast)
void motor_all_off();
