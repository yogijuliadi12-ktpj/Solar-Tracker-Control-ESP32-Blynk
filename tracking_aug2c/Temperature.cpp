#include "Temperature.h"

/*****************************************************************
 *                  PIN CONFIGURATION
 *****************************************************************/
// Sesuaikan pin-pin ini dengan wiring Anda
const int MAX6675_SCK = 18;
const int MAX6675_SO = 19;
const int TC1_CS = 5; 
const int TC2_CS = 15;
const int TC3_CS = 4;
const int TC4_CS = 2;

const uint8_t FAN_RELAY_PIN = 12; // Ganti sesuai GPIO yang tersedia untuk Relay Kipas
// Active LOW
const bool RELAY_ON  = LOW;
const bool RELAY_OFF = HIGH;

/*****************************************************************
 *                  MAX6675
 *****************************************************************/
MAX6675 thermocouple1(MAX6675_SCK, TC1_CS, MAX6675_SO);
MAX6675 thermocouple2(MAX6675_SCK, TC2_CS, MAX6675_SO);
MAX6675 thermocouple3(MAX6675_SCK, TC3_CS, MAX6675_SO);
MAX6675 thermocouple4(MAX6675_SCK, TC4_CS, MAX6675_SO);

float temp1 = 0.0;
float temp2 = 0.0;
float temp3 = 0.0;
float temp4 = 0.0;

/*****************************************************************
 *                  FAN CONTROL VARIABLES
 *****************************************************************/
const float FAN_ON_TEMP = 50.0;
const float FAN_OFF_TEMP = 40.0;

const uint32_t FAN_MIN_ON_TIME = 30000;   // 30 detik
const uint32_t FAN_BOOT_DELAY = 3000;     // 3 detik

bool fanState = false; // Hanya ditulis satu kali di sini
unsigned long fanStartTime = 0;

/*****************************************************************
 *                  FUNCTIONS
 *****************************************************************/
void setupTemperature()
{
    // 1. Paksa inisialisasi pin jalur komunikasi secara manual
    pinMode(MAX6675_SCK, OUTPUT);
    pinMode(MAX6675_SO, INPUT);

    // 2. Jadikan semua pin CS sebagai OUTPUT
    pinMode(TC1_CS, OUTPUT);
    pinMode(TC2_CS, OUTPUT);
    pinMode(TC3_CS, OUTPUT);
    pinMode(TC4_CS, OUTPUT);

    // 3. WAJIB: Set semua CS ke HIGH (Matikan semua sensor agar tidak bentrok di awal)
    digitalWrite(TC1_CS, HIGH);
    digitalWrite(TC2_CS, HIGH);
    digitalWrite(TC3_CS, HIGH);
    digitalWrite(TC4_CS, HIGH);
    
    // 4. Inisialisasi Relay Fan
    pinMode(FAN_RELAY_PIN, OUTPUT);
    digitalWrite(FAN_RELAY_PIN, RELAY_OFF);

    Serial.println("MAX6675 Initialized & SPI Bus Secured");

    delay(500);
}

void readTemperature()
{
    temp1 = thermocouple1.readCelsius();
    delay(10); // Jeda stabilitas jalur data
    temp2 = thermocouple2.readCelsius();
    delay(10); // Jeda stabilitas jalur data
    temp3 = thermocouple3.readCelsius();
    delay(10); // Jeda stabilitas jalur data
    temp4 = thermocouple4.readCelsius();
}

void controlFan()
{
    //==========================================================
    // BOOT SAFETY
    //==========================================================
    if (millis() < FAN_BOOT_DELAY)
    {
        fanState = false;
        digitalWrite(FAN_RELAY_PIN, RELAY_OFF);
        return;
    }

    //==========================================================
    // FAN OFF
    //==========================================================
    if (!fanState)
    {
        if (temp2 >= FAN_ON_TEMP)
        {
            fanState = true;
            fanStartTime = millis();
            digitalWrite(FAN_RELAY_PIN, RELAY_ON);
        }
    }
    //==========================================================
    // FAN ON
    //==========================================================
    else
    {
        if (millis() - fanStartTime >= FAN_MIN_ON_TIME)
        {
            if (temp2 <= FAN_OFF_TEMP)
            {
                fanState = false;
                digitalWrite(FAN_RELAY_PIN, RELAY_OFF);
            }
        }
    }
}

// Fungsi pembungkus agar rapi di file utama .ino
void processTemperature()
{
    readTemperature();
    controlFan();
}

void printTemperature()
{
    Serial.println("========== TEMPERATURE ==========");

    Serial.print("TC1 : ");
    Serial.print(temp1);
    Serial.println(" C");

    Serial.print("TC2 : ");
    Serial.print(temp2);
    Serial.println(" C");

    Serial.print("TC3 : ");
    Serial.print(temp3);
    Serial.println(" C");

    Serial.print("TC4 : ");
    Serial.print(temp4);
    Serial.println(" C");
    
    Serial.print("Fan : ");
    if(fanState)
        Serial.println("ON");
    else
        Serial.println("OFF");

    Serial.println();
}