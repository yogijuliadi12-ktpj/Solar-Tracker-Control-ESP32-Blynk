#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include <max6675.h> // Pastikan library ini sudah ter-include

// Deklarasi pin MAX6675 (Sesuaikan jika ada perbedaan)
extern const int MAX6675_SCK;
extern const int MAX6675_SO;
extern const int TC1_CS;
extern const int TC2_CS;
extern const int TC3_CS;
extern const int TC4_CS;

// Deklarasi variabel suhu
extern float temp1;
extern float temp2;
extern float temp3;
extern float temp4;

// Deklarasi variabel Fan
extern bool fanState;

// Prototipe fungsi
void setupTemperature();
void readTemperature();
void controlFan();
void processTemperature();
void printTemperature();

#endif