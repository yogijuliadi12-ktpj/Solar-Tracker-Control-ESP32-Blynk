#include "Ultrasonic.h"

/*****************************************************************
 *                      PIN
 *****************************************************************/
const uint8_t TRIG_PIN = 17;      
const uint8_t ECHO_PIN = 39;      
const uint8_t RELAY_PIN = 16;     

/*****************************************************************
 *                 TANK
 *****************************************************************/
const float TANK_HEIGHT = 15.0;          // cm
const float TANK_RADIUS = 4.5;           // cm
const float TANK_AREA = 63.617f;         // cm²
const float TANK_VOLUME = 954.3f;        // mL

/*****************************************************************
 *               PUMP SETPOINT
 *****************************************************************/
const uint16_t PUMP_ON_VOLUME = 300;     // mL
const uint16_t PUMP_OFF_VOLUME = 200;    // mL

/*****************************************************************
 *               FILTER
 *****************************************************************/
const uint8_t FILTER_SIZE = 5;
const float EMA_ALPHA = 0.25;

float rawDistance = 0;
float filteredDistance = 0;
float waterHeight = 0;
float waterVolume = 0;

bool firstEMA = true;
bool firstFilter = true; // Variabel baru
bool pumpState = false;

float medianBuffer[FILTER_SIZE];

void setupUltrasonic()
{
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);

    digitalWrite(TRIG_PIN, LOW);
    digitalWrite(RELAY_PIN, HIGH);

    Serial.println("HC-SR04 Ready");
}

void readUltrasonic()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    // Timeout diubah menjadi 5000 mikrodetik
    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if(duration == 0)
        return;

    rawDistance = duration * 0.0343 / 2.0;
}

void sortArray(float arr[])
{
    for(int i=0;i<FILTER_SIZE-1;i++)
    {
        for(int j=i+1;j<FILTER_SIZE;j++)
        {
            if(arr[j]<arr[i])
            {
                float temp=arr[i];
                arr[i]=arr[j];
                arr[j]=temp;
            }
        }
    }
}

void filterUltrasonic()
{
    // Inisialisasi buffer saat pembacaan pertama
    if (firstFilter)
    {
        for (int i = 0; i < FILTER_SIZE; i++)
        {
            medianBuffer[i] = rawDistance;
        }

        filteredDistance = rawDistance;

        firstEMA = false;
        firstFilter = false;

        return;
    }

    // Geser data lama
    for (int i = 0; i < FILTER_SIZE - 1; i++)
    {
        medianBuffer[i] = medianBuffer[i + 1];
    }

    // Masukkan data terbaru
    medianBuffer[FILTER_SIZE - 1] = rawDistance;

    // Copy buffer
    float temp[FILTER_SIZE];

    for (int i = 0; i < FILTER_SIZE; i++)
    {
        temp[i] = medianBuffer[i];
    }

    // Urutkan
    sortArray(temp);

    // Ambil median
    float median = temp[FILTER_SIZE / 2];

    // EMA
    filteredDistance =
        EMA_ALPHA * median +
        (1.0f - EMA_ALPHA) * filteredDistance;
}

void calculateVolume()
{
    waterHeight = TANK_HEIGHT - filteredDistance;

    if(waterHeight<0)
        waterHeight=0;

    if(waterHeight>TANK_HEIGHT)
        waterHeight=TANK_HEIGHT;

    waterVolume = waterHeight*TANK_AREA;
}

void controlPump()
{
    if(!pumpState)
    {
        if(waterVolume>=PUMP_ON_VOLUME)
        {
            pumpState=true;
            digitalWrite(RELAY_PIN,LOW);
        }
    }
    else
    {
        if(waterVolume<=PUMP_OFF_VOLUME)
        {
            pumpState=false;
            digitalWrite(RELAY_PIN,HIGH);
        }
    }
}

void printData()
{
    Serial.println("--------------------------");

    // Print diperbarui sesuai permintaan
    Serial.print("Raw Distance      : ");
    Serial.print(rawDistance, 2);
    Serial.println(" cm");

    Serial.print("Filtered Distance : ");
    Serial.print(filteredDistance, 2);
    Serial.println(" cm");

    Serial.print("Water Level       : ");
    Serial.print(waterHeight, 2);
    Serial.println(" cm");

    Serial.print("Water Volume      : ");
    Serial.print(waterVolume, 1);
    Serial.println(" mL");

    Serial.print("Pump              : ");

    if(pumpState)
        Serial.println("ON");
    else
        Serial.println("OFF");

    Serial.println();
}

void processUltrasonicPump()
{
    readUltrasonic();
    filterUltrasonic();
    calculateVolume();
    controlPump();
}