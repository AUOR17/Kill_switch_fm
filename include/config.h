#pragma once
#include <Arduino.h>

/********* UART & BAUD *********/
#ifndef USB_BAUD
#define USB_BAUD 115200
#endif

#ifndef GPS_BAUD
#define GPS_BAUD 9600
#endif

#ifndef GPS_UART_NUM
#define GPS_UART_NUM 1
#endif

#ifndef GPS_RX_PIN
#define GPS_RX_PIN 16
#endif
#ifndef GPS_TX_PIN
#define GPS_TX_PIN 17
#endif

/********* DRV8833 PINS *********/
#ifndef DRV1_AIN1
#define DRV1_AIN1 23
#endif
#ifndef DRV1_AIN2
#define DRV1_AIN2 22
#endif
#ifndef DRV1_BIN1
#define DRV1_BIN1 19
#endif
#ifndef DRV1_BIN2
#define DRV1_BIN2 18
#endif

#ifndef DRV2_AIN1
#define DRV2_AIN1 13
#endif
#ifndef DRV2_AIN2
#define DRV2_AIN2 21
#endif
#ifndef DRV2_BIN1
#define DRV2_BIN1 32
#endif
#ifndef DRV2_BIN2
#define DRV2_BIN2 33
#endif

#ifndef DRV_STBY
#define DRV_STBY 27
#endif

/********* SPEED SENSORS *********/
#ifndef SPEED_S1_PIN
#define SPEED_S1_PIN 25
#endif
#ifndef SPEED_S2_PIN
#define SPEED_S2_PIN 26
#endif
#ifndef SPEED_S3_PIN
#define SPEED_S3_PIN 36
#endif
#ifndef SPEED_S4_PIN
#define SPEED_S4_PIN 39
#endif

/********* PWM *********/
#ifndef PWM_FREQ
#define PWM_FREQ 20000
#endif
#ifndef PWM_RES_BITS
#define PWM_RES_BITS 10
#endif

/********* RATES (ya venían) *********/
#ifndef GPS_HZ
#define GPS_HZ 1
#endif
#ifndef WHEELS_HZ
#define WHEELS_HZ 10
#endif
#ifndef ACTUATOR_HZ
#define ACTUATOR_HZ 2
#endif
