#ifndef PRESSURE_H
#define PRESSURE_H

#include <Arduino.h>

// Deklarasi variabel eksternal tunggal (bukan array)
extern uint16_t adcRaw;
extern int32_t adcCorrected;
extern int32_t pressureADCOffset;

extern float pressureVoltage;
extern float sensorVoltage;
extern float pressureMPa;
extern float pressureBar;

// Prototipe fungsi
void setupPressure();
void calibratePressureZero();
void readPressureADC();
void calculateSensorVoltage();
void calculatePressure();
void processPressure();
void printPressure();

#endif