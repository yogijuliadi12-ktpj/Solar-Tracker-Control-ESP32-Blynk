#include "LDR_Filter.h"

// FILTER CONFIGURATION
const uint8_t MEDIAN_SIZE = 5;
const float EMA_ALPHA = 0.20f;

// TRACKING CONSTANT
const float LIGHT_THRESHOLD = 800.0;
const float MIN_TRACKING_LIGHT = 800.0;

// FILTER VARIABLE
uint16_t bufferTL[MEDIAN_SIZE];
uint16_t bufferTR[MEDIAN_SIZE];
uint16_t bufferBL[MEDIAN_SIZE];
uint16_t bufferBR[MEDIAN_SIZE];

uint8_t filterIndex = 0;

uint16_t medianTL;
uint16_t medianTR;
uint16_t medianBL;
uint16_t medianBR;

float emaTL = 0;
float emaTR = 0;
float emaBL = 0;
float emaBR = 0;

// GLOBAL VARIABLE
uint16_t ldrTL = 0;
uint16_t ldrTR = 0;
uint16_t ldrBL = 0;
uint16_t ldrBR = 0;

// TRACKING VARIABLE
float averageLight = 0.0f;
float errorHorizontal = 0.0f;
float errorVertical = 0.0f;
bool trackingEnable = false;

// LDR CALIBRATION
float gainTL = 1.000;
float gainTR = 1.000;
float gainBL = 1.000;
float gainBR = 1.000;

float calibTL;
float calibTR;
float calibBL;
float calibBR;

void readLDR()
{
    ldrTL = 4095 - analogRead(PIN_LDR_TL);
    ldrTR = 4095 - analogRead(PIN_LDR_TR);
    ldrBL = 4095 - analogRead(PIN_LDR_BL);
    ldrBR = 4095 - analogRead(PIN_LDR_BR);
}

void printFiltered()
{
    Serial.print("TL : ");
    Serial.print(calibTL);

    Serial.print(" | TR : ");
    Serial.print(calibTR);

    Serial.print(" | BL : ");
    Serial.print(calibBL);

    Serial.print(" | BR : ");
    Serial.println(calibBR);
}

void updateFilter()
{
    bufferTL[filterIndex] = ldrTL;
    bufferTR[filterIndex] = ldrTR;
    bufferBL[filterIndex] = ldrBL;
    bufferBR[filterIndex] = ldrBR;

    filterIndex++;

    if(filterIndex >= MEDIAN_SIZE)
        filterIndex = 0;

    medianTL = medianFilter(bufferTL);
    medianTR = medianFilter(bufferTR);
    medianBL = medianFilter(bufferBL);
    medianBR = medianFilter(bufferBR);

    emaTL = EMA_ALPHA * medianTL + (1.0f - EMA_ALPHA) * emaTL;
    emaTR = EMA_ALPHA * medianTR + (1.0f - EMA_ALPHA) * emaTR;
    emaBL = EMA_ALPHA * medianBL + (1.0f - EMA_ALPHA) * emaBL;
    emaBR = EMA_ALPHA * medianBR + (1.0f - EMA_ALPHA) * emaBR;
}

uint16_t medianFilter(uint16_t *buffer)
{
    uint16_t temp[MEDIAN_SIZE];

    for(uint8_t i=0;i<MEDIAN_SIZE;i++)
        temp[i]=buffer[i];

    for(uint8_t i=0;i<MEDIAN_SIZE-1;i++)
    {
        for(uint8_t j=i+1;j<MEDIAN_SIZE;j++)
        {
            if(temp[j] < temp[i])
            {
                uint16_t swap=temp[i];
                temp[i]=temp[j];
                temp[j]=swap;
            }
        }
    }

    return temp[MEDIAN_SIZE/2];
}

void updateEMA()
{
    emaTL = EMA_ALPHA * ldrTL + (1.0 - EMA_ALPHA) * emaTL;
    emaTR = EMA_ALPHA * ldrTR + (1.0 - EMA_ALPHA) * emaTR;
    emaBL = EMA_ALPHA * ldrBL + (1.0 - EMA_ALPHA) * emaBL;
    emaBR = EMA_ALPHA * ldrBR + (1.0 - EMA_ALPHA) * emaBR;
}

void calibrateLDR(){
    calibTL = emaTL * gainTL;
    calibTR = emaTR * gainTR;
    calibBL = emaBL * gainBL;
    calibBR = emaBR * gainBR;
}
void calculateAverage(){
    averageLight =
        (calibTL +
         calibTR +
         calibBL +
         calibBR) / 4.0;
    trackingEnable = (averageLight >= LIGHT_THRESHOLD);
}
void calculateError(){
    errorHorizontal =
        (calibTL + calibBL) -
        (calibTR + calibBR);

    errorVertical =
        (calibTL + calibTR) -
        (calibBL + calibBR);
}
void printTrackingData()
{
    Serial.println();
    Serial.println("==============================");
    Serial.print("TL : ");
    Serial.println(calibTL);
    Serial.print("TR : ");
    Serial.println(calibTR);
    Serial.print("BL : ");
    Serial.println(calibBL);
    Serial.print("BR : ");
    Serial.println(calibBR);
    Serial.println("------------------------------");
    Serial.print("Average : ");
    Serial.println(averageLight);
    Serial.print("Horizontal Error : ");
    Serial.println(errorHorizontal);
    Serial.print("Vertical Error : ");
    Serial.println(errorVertical);
    Serial.print("Tracking : ");

    if(trackingEnable)
        Serial.println("ENABLE");
    else
        Serial.println("DISABLE");

    Serial.println("==============================");
}