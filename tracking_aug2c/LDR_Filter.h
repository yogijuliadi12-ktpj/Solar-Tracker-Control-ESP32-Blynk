#ifndef LDR_FILTER_H
#define LDR_FILTER_H

#include <Arduino.h>

// PIN CONFIGURATION LDR (Diletakkan di sini agar terbaca oleh setup() di .ino)
const uint8_t PIN_LDR_TL = 32;     // Top Left
const uint8_t PIN_LDR_TR = 33;     // Top Right
const uint8_t PIN_LDR_BL = 34;     // Bottom Left
const uint8_t PIN_LDR_BR = 35;     // Bottom Right

// Variabel yang dibutuhkan oleh modul lain
extern float averageLight;
extern float errorHorizontal;
extern float errorVertical;
extern bool trackingEnable;
extern const float MIN_TRACKING_LIGHT;

// Function Prototype
void readLDR();
void printLDR();
void printFiltered();
void updateFilter();
void updateEMA();
void calibrateLDR();
void calculateAverage();
void calculateError();
void printTrackingData();
uint16_t medianFilter(uint16_t *buffer);

#endif