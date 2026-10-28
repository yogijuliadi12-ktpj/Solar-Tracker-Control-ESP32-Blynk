#include "Pressure.h"

/*****************************************************************
 *                  PRESSURE SENSOR
 *****************************************************************/

const uint8_t PRESSURE1_PIN = 36; // ADC1 (Sensor VP)

const float ADC_REFERENCE = 3.3;
const int ADC_RESOLUTION = 4095;

uint16_t adcRaw = 0;
int32_t adcCorrected = 0;
int32_t pressureADCOffset = 0;

float pressureVoltage = 0.0;
float sensorVoltage = 0.0;
float pressureMPa = 0.0;
float pressureBar = 0.0;

void calibratePressureZero()
{
    Serial.println("Calibrating Pressure Sensor (Zeroing)...");
    
    long sum = 0;
    
    // Mengambil 20 sampel data pembacaan awal untuk nilai rata-rata offset
    for (int i = 0; i < 20; i++)
    {
        sum += analogRead(PRESSURE1_PIN);
        delay(50); 
    }
    
    pressureADCOffset = sum / 20;
    
    Serial.println("Calibration Done.");
}

void setupPressure()
{
    pinMode(PRESSURE1_PIN, INPUT);

    delay(2000);
    calibratePressureZero();
}

void readPressureADC()
{
    adcRaw = analogRead(PRESSURE1_PIN);

    adcCorrected = adcRaw - pressureADCOffset;
    
    if (adcCorrected < 0)
    {
        adcCorrected = 0;
    }

    pressureVoltage = (float)adcCorrected * ADC_REFERENCE / (float)ADC_RESOLUTION;
}

void calculateSensorVoltage()
{
    sensorVoltage = pressureVoltage * (37.0 / 27.0);
}

void calculatePressure()
{
    // Pengurangan -0.5 dihapus karena base 0.5V sudah dibuang di tahap adcCorrected
    pressureMPa = (sensorVoltage / 4.0) * 1.2;

    if (pressureMPa < 0)
        pressureMPa = 0;

    if (pressureMPa > 1.2)
        pressureMPa = 1.2;

    pressureBar = pressureMPa * 10.0;
}

// Fungsi pembungkus
void processPressure()
{
    readPressureADC();
    calculateSensorVoltage();
    calculatePressure();
}

void printPressure()
{
    Serial.println("========== PRESSURE ==========");
    Serial.println("--- SENSOR P1");

    Serial.print("ADC Raw       : ");
    Serial.println(adcRaw);

    Serial.print("ADC Corrected : ");
    Serial.println(adcCorrected);

    Serial.print("Pressure      : ");
    Serial.print(pressureBar, 2);
    Serial.println(" bar\n");
}