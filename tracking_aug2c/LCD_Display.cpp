#include "LCD_Display.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Include modul lain untuk membaca variabel global
#include "Pressure.h"
#include "Temperature.h"
#include "Ultrasonic.h"
#include "SpeedSensor.h"
#include "LDR_Filter.h" 

LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t page = 0;
unsigned long lastPage = 0;
const uint16_t PAGE_TIME = 3000;

void setupLCD()
{
    Wire.begin(21, 22);

    lcd.init();
    lcd.backlight();
    lcd.clear();

    // Splash screen saat awal menyala
    lcd.setCursor(0, 0);
    lcd.print("CSP Rankine");
    lcd.setCursor(0, 1);
    lcd.print("Starting...");
}

void updateLCD()
{
    if (millis() - lastPage > PAGE_TIME)
    {
        lastPage = millis();
        page++;

        // Batas halaman diubah menjadi 5 karena ada tambahan halaman TC3 & TC4
        if (page > 5)
            page = 0;

        lcd.clear();
    }

    switch (page)
    {
        case 0:
            lcd.setCursor(0, 0);
            lcd.print("RPM:");
            lcd.print(rpmFiltered, 0);

            lcd.setCursor(0, 1);
            lcd.print("P:");
            lcd.print(pressureBar, 2); 
            lcd.print("bar");
            break;

        case 1:
            lcd.setCursor(0, 0);
            lcd.print("TC1:");
            lcd.print(temp1, 1); 

            lcd.setCursor(0, 1);
            lcd.print("TC2:");
            lcd.print(temp2, 1); 
            break;

        case 2:
            lcd.setCursor(0, 0);
            lcd.print("Tank:");
            lcd.print(waterVolume, 0); 
            lcd.print("mL");

            lcd.setCursor(0, 1);
            lcd.print("Pump:");
            if (pumpState)
                lcd.print("ON ");
            else
                lcd.print("OFF");
            break;

        case 3:
            lcd.setCursor(0, 0);
            lcd.print("Fan:");
            if (fanState)
                lcd.print("ON ");
            else
                lcd.print("OFF");

            lcd.setCursor(0, 1);
            lcd.print("Light:");
            lcd.print(averageLight);
            break;

        case 4:
            lcd.setCursor(0, 0);
            lcd.print("Track:");
            if (trackingEnable)
                lcd.print("ON ");
            else
                lcd.print("OFF");

            lcd.setCursor(0, 1);
            lcd.print("Err:");
            lcd.print(errorHorizontal);
            break;

        // HALAMAN BARU UNTUK TC3 DAN TC4
        case 5:
            lcd.setCursor(0, 0);
            lcd.print("TC3:");
            lcd.print(temp3, 1); 

            lcd.setCursor(0, 1);
            lcd.print("TC4:");
            lcd.print(temp4, 1); 
            break;
    }
}