#pragma once
#include <Arduino.h>

#ifndef DEVICE_ID
#define DEVICE_ID "vehiculo-01"
#endif

// Communication 
#define USB_BAUD 115200   // Para debug en Serial
#define GPS_BAUD 9600     // Para módulo GPS


// Pines 
static constexpr uint8_t PIN_PWM_1 = 25;
static constexpr uint8_t PIN_PWM_2 = 26;
static constexpr uint8_t PIN_PWM_3 = 27;
static constexpr uint8_t PIN_PWM_4 = 14;

static constexpr uint8_t PIN_HALL_1 = 32;
static constexpr uint8_t PIN_HALL_2 = 33;
static constexpr uint8_t PIN_HALL_3 = 34; 
static constexpr uint8_t PIN_HALL_4 = 35; 

// Frecuencias y tiempos
#ifndef GPS_HZ
#define GPS_HZ 1
#endif
#ifndef WHEELS_HZ
#define WHEELS_HZ 10
#endif
#ifndef ACTUATOR_HZ
#define ACTUATOR_HZ 2
#endif

// PWM (LEDC)
static constexpr uint32_t PWM_FREQ = 20000; // 20 kHz
static constexpr uint8_t  PWM_RES_BITS = 10; // 0..1023

//gps
static int RX_PIN = 16;
static int TX_PIN = 17;


