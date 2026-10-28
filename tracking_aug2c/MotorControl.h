#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

// PIN CONFIGURATION BTS7960 (Diletakkan di sini agar terbaca oleh setup() di .ino)
const uint8_t RPWM_PIN = 25;
const uint8_t LPWM_PIN = 26;

// Function Prototype
void setupStepper();
void controlStepper();
void updateTrackingState();
void trackingTask();
void setupActuator();
void actuatorStop();
void actuatorExtend(uint8_t pwm);
void actuatorRetract(uint8_t pwm);
uint8_t calculatePWM(float error);
void controlActuator();

#endif