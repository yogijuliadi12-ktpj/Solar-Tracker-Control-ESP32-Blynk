#include "SpeedSensor.h"

/*****************************************************************
 *                  SPEED SENSOR CONFIGURATION
 *****************************************************************/
const uint8_t SPEED_PIN = 13; // Catatan: Pastikan pin 13 tidak bentrok dengan Pressure Sensor 3 Anda sebelumnya
const uint8_t SLOT_NUMBER = 1;

volatile uint32_t pulseCount = 0;

float rpmRaw = 0;
float rpmFiltered = 0;

unsigned long lastRPMTime = 0;

const uint8_t FILTER_SIZE = 5;
float rpmBuffer[FILTER_SIZE];
uint8_t bufferIndex = 0;

void IRAM_ATTR speedISR()
{
    pulseCount++;
}

void setupSpeedSensor()
{
    pinMode(SPEED_PIN, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(SPEED_PIN),
        speedISR,
        FALLING
    );

    for(int i = 0; i < FILTER_SIZE; i++)
    {
        rpmBuffer[i] = 0;
    }

    Serial.println("Speed Sensor Initialized");
}

void calculateRPM()
{
    unsigned long now = millis();

    // Timer interval 1 detik bawaan fungsi
    if(now - lastRPMTime >= 1000)
    {
        noInterrupts();
        uint32_t pulse = pulseCount;
        pulseCount = 0;
        interrupts();

        rpmRaw = ((float)pulse / (float)SLOT_NUMBER) * 60.0;

        lastRPMTime = now;
    }
}

void filterRPM()
{
    rpmBuffer[bufferIndex] = rpmRaw;
    bufferIndex++;

    if(bufferIndex >= FILTER_SIZE)
        bufferIndex = 0;

    float temp[FILTER_SIZE];

    for(int i = 0; i < FILTER_SIZE; i++)
        temp[i] = rpmBuffer[i];

    // Mengurutkan array untuk mencari Median
    for(int i = 0; i < FILTER_SIZE - 1; i++)
    {
        for(int j = i + 1; j < FILTER_SIZE; j++)
        {
            if(temp[i] > temp[j])
            {
                float t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }

    float median = temp[FILTER_SIZE / 2];
    float sum = 0;

    // Menghitung Rata-rata (Average)
    for(int i = 0; i < FILTER_SIZE; i++)
        sum += rpmBuffer[i];

    float average = sum / FILTER_SIZE;

    // Filter gabungan
    rpmFiltered = (median + average) / 2.0;
}

void printRPM()
{
    Serial.println("========== TURBINE ==========");

    Serial.print("RPM Raw    : ");
    Serial.println(rpmRaw);

    Serial.print("RPM Filter : ");
    Serial.println(rpmFiltered);

    Serial.println();
}