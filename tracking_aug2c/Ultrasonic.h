#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

// Variabel Global
extern float rawDistance;
extern float filteredDistance;
extern float waterHeight;
extern float waterVolume;
extern bool firstEMA;
extern bool firstFilter; // Variabel baru ditambahkan
extern bool pumpState;

// Function Prototype
void setupUltrasonic();
void readUltrasonic();
void sortArray(float arr[]);
void filterUltrasonic();
void calculateVolume();
void controlPump();
void printData();
void processUltrasonicPump();

#endif