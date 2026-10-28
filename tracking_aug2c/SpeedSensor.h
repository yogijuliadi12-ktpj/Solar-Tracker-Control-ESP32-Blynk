#ifndef SPEED_SENSOR_H
#define SPEED_SENSOR_H

#include <Arduino.h>

// Variabel Eksternal
extern volatile uint32_t pulseCount;
extern float rpmRaw;
extern float rpmFiltered;

// Prototipe Fungsi
void setupSpeedSensor();
void IRAM_ATTR speedISR();
void calculateRPM();
void filterRPM();
void printRPM();

#endif