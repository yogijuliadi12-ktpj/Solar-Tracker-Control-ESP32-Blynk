// ==========================================================
// KONFIGURASI BLYNK (TARUH DI BARIS PALING ATAS)
// ==========================================================
#define BLYNK_TEMPLATE_ID "TMPL6BMAxfBL8"
#define BLYNK_TEMPLATE_NAME "Monitoring CSP"
#define BLYNK_AUTH_TOKEN "-M0gWkM0CRhHYs_jf_LNBIRHTdjH8NUx"

#define BLYNK_PRINT Serial // Mengaktifkan print debug Blynk



// ==========================================================
// KONFIGURASI WIFI
// ==========================================================
char ssid[] = "realme 3 Pro";
char pass[] = "00000000";

// CONSTANT UNTUK TIMER
const uint32_t SERIAL_BAUD = 115200;
const uint16_t LDR_SAMPLE_TIME = 50;       
const uint16_t TEMP_SAMPLE_TIME = 50;     
const uint16_t PRESS_SAMPLE_TIME = 50;    
const uint16_t ULTRA_SAMPLE_TIME = 500;     
const uint16_t BLYNK_SAMPLE_TIME = 50; // Kirim ke Blynk tiap 2 detik
const uint16_t SPEED_SAMPLE_TIME = 50;

unsigned long previousMillisLDR = 0;
unsigned long previousMillisTemp = 0;
unsigned long previousMillisPress = 0;
unsigned long previousMillisUltra = 0; 
unsigned long previousMillisBlynk = 0; 
unsigned long previousMillisSpeed = 0;

void setup()
{
    Serial.begin(SERIAL_BAUD);
    analogReadResolution(12);          

    // --- INISIALISASI NON-BLOCKING WIFI & BLYNK ---
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, pass);
    Blynk.config(BLYNK_AUTH_TOKEN); 
    
    // --- INISIALISASI LDR ---
    pinMode(PIN_LDR_TL, INPUT);
    pinMode(PIN_LDR_TR, INPUT);
    pinMode(PIN_LDR_BL, INPUT);
    pinMode(PIN_LDR_BR, INPUT);

    setupTemperature(); 
    setupPressure(); 
    setupUltrasonic(); 
    setupLCD(); 
    setupStepper();
    setupActuator();
    setupSpeedSensor(); // Letakkan bersama inisialisasi sensor lain

    Serial.println("\n==========================================");
    Serial.println("    CSP SOLAR TRACKER V1.6 + BLYNK");
    Serial.println("    TEST ALL SENSORS & AXIS DATA");
    Serial.println("==========================================");
}

void loop()
{
    // --- BLYNK KEEPALIVE ---
    Blynk.run(); 

    // --- JADWAL 1: LDR (500 ms) ---
    if (millis() - previousMillisLDR >= LDR_SAMPLE_TIME)
    {
        previousMillisLDR = millis();
        
        readLDR();
        updateEMA();
        calibrateLDR();
        calculateAverage();
        calculateError();
        
        printTrackingData();
    }

    // --- JADWAL 2: SUHU & FAN (1000 ms) ---
    if (millis() - previousMillisTemp >= TEMP_SAMPLE_TIME)
    {
        previousMillisTemp = millis();
        
        processTemperature(); 
        printTemperature();
    }

    // --- JADWAL 3: TEKANAN (1000 ms) ---
    if (millis() - previousMillisPress >= PRESS_SAMPLE_TIME)
    {
        previousMillisPress = millis();
        processPressure();
        printPressure();
    }

    // --- JADWAL 4: ULTRASONIK & POMPA (1000 ms) ---
    if (millis() - previousMillisUltra >= ULTRA_SAMPLE_TIME)
    {
        previousMillisUltra = millis();
        
        readUltrasonic();
        filterUltrasonic();
        calculateVolume();
        controlPump();
        printData();       
    }

    // --- JADWAL 5: PENGIRIMAN DATA KE BLYNK (2000 ms) ---
    if (millis() - previousMillisBlynk >= BLYNK_SAMPLE_TIME)
    {
        previousMillisBlynk = millis();

        // Hanya kirim data jika Blynk sudah terhubung ke internet
        if (Blynk.connected()) 
        {
            // Data Suhu (V1-V4) & Kipas (V5)
            Blynk.virtualWrite(V1, temp1);
            Blynk.virtualWrite(V2, temp2);
            Blynk.virtualWrite(V3, temp3);
            Blynk.virtualWrite(V4, temp4);
            Blynk.virtualWrite(V5, fanState ? 1 : 0);

            // Data Tekanan (V6-V8)
            Blynk.virtualWrite(V6, pressureBar);

            // Data Tangki (V9-V10) & Pompa (V11)
            Blynk.virtualWrite(V9, waterHeight);
            Blynk.virtualWrite(V10, waterVolume);
            Blynk.virtualWrite(V11, pumpState ? 1 : 0);

            // ==================================================
            // DATA BARU: PERGERAKAN AXIS & STATUS
            // ==================================================
            Blynk.virtualWrite(V12, errorHorizontal); // Sumbu X (Stepper)
            Blynk.virtualWrite(V13, errorVertical);   // Sumbu Y (Actuator)

            String statusTracker = "TRACKING";
            if (!trackingEnable) 
            {
                statusTracker = "STOPPED";
            } 
            else if (averageLight < MIN_TRACKING_LIGHT) 
            {
                statusTracker = "LOW LIGHT";
            }
            Blynk.virtualWrite(V14, statusTracker);

            // ==================================================
            // DATA BARU: SPEED SENSOR / TURBIN (V15)
            // ==================================================
            Blynk.virtualWrite(V15, rpmFiltered);
        }
    }
    
    // --- JADWAL 6: SPEED SENSOR (TURBINE) (1000 ms) ---
    if(millis() - previousMillisSpeed >= SPEED_SAMPLE_TIME)
    {
        previousMillisSpeed = millis();

        calculateRPM();
        filterRPM();
        printRPM();
    }
    
    // --- TASK LCD (Diberi timer agar I2C tidak bikin Lag) ---
    static unsigned long previousMillisLCD = 0;
    if(millis() - previousMillisLCD >= 500)
    {
        previousMillisLCD = millis();
        updateLCD();
    }
    // --- TASK MOTOR ---
    trackingTask();
}