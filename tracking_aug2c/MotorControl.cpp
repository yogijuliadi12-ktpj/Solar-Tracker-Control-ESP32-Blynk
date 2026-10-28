#include "MotorControl.h"
#include "LDR_Filter.h"
#include <AccelStepper.h>

// PIN CONFIGURATION STEPPER
const uint8_t STEP_PIN = 23;
const uint8_t DIR_PIN  = 14;
const uint8_t EN_PIN   = 27;

// PWM CONFIGURATION
const uint32_t PWM_FREQUENCY = 1000;      // Hz
const uint8_t PWM_RESOLUTION = 8;         // 0-255

// HYSTERESIS
const int START_TRACKING = 100;
const int STOP_TRACKING = 40;
bool stepperMoving = false;

// STEPPER CONSTANT
const int DEAD_BAND = 80;
const int MAX_ERROR = 1500;
const float MIN_SPEED = 50;
const float MAX_SPEED = 600;
const float ACCELERATION = 300;

// ACTUATOR CONSTANT
const int DEAD_BAND_Y = 80;
const int MAX_ERROR_Y = 1500;
const uint8_t MIN_PWM = 120;
const uint8_t MAX_PWM = 255;

AccelStepper stepper(
    AccelStepper::DRIVER,
    STEP_PIN,
    DIR_PIN
);

enum TrackingState
{
    TRACK_IDLE,
    TRACKING,
    LOW_LIGHT
};

TrackingState trackingState = TRACK_IDLE;

void setupStepper()
{
    pinMode(STEP_PIN, OUTPUT);
    digitalWrite(STEP_PIN, LOW);
    pinMode(DIR_PIN, OUTPUT);
    digitalWrite(DIR_PIN, LOW);
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, HIGH);      // Driver OFF saat awal menyala
    stepper.setMaxSpeed(MAX_SPEED);
    stepper.setAcceleration(ACCELERATION);
}

void controlStepper()
{
    if(!trackingEnable)
    {
        digitalWrite(EN_PIN, HIGH);  // Matikan Driver jika tracking disable
        stepper.stop();
        return;
    }
    if(abs(errorHorizontal) <= DEAD_BAND)
    {
        digitalWrite(EN_PIN, HIGH);  // Matikan Driver saat sudah di posisi yang tepat (hemat daya & tidak panas)
        stepper.stop();
        return;
    }
    // Nyalakan Driver karena motor harus bergerak
    digitalWrite(EN_PIN, LOW); 
    float speed = map(
        abs(errorHorizontal),
        DEAD_BAND,
        MAX_ERROR,
        MIN_SPEED,
        MAX_SPEED
    );
    speed = constrain(
        speed,
        MIN_SPEED,
        MAX_SPEED
    );
    if(errorHorizontal > 0)
        stepper.setSpeed(speed);
    else
        stepper.setSpeed(-speed);

    stepper.runSpeed();
}

void updateTrackingState()
{
    if (averageLight < MIN_TRACKING_LIGHT)
    {
        trackingState = LOW_LIGHT;
        return;
    }
    trackingState = TRACKING;
}

void trackingTask()
{
    updateTrackingState();

    switch (trackingState)
    {
        case LOW_LIGHT:
            digitalWrite(EN_PIN, HIGH); // Matikan Driver saat low light
            stepper.stop();
            actuatorStop();
            break;

        case TRACKING:
            controlStepper();
            controlActuator();
            break;

        default:
            digitalWrite(EN_PIN, HIGH); // Matikan Driver saat default/idle
            stepper.stop();
            actuatorStop();
            break;
    }
}

void setupActuator()
{
    ledcAttach(RPWM_PIN, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttach(LPWM_PIN, PWM_FREQUENCY, PWM_RESOLUTION);

    actuatorStop();
}

void actuatorStop()
{
    ledcWrite(RPWM_PIN, 0);
    ledcWrite(LPWM_PIN, 0);
}

void actuatorExtend(uint8_t pwm)
{
    ledcWrite(RPWM_PIN, pwm);
    ledcWrite(LPWM_PIN, 0);
}

void actuatorRetract(uint8_t pwm)
{
    ledcWrite(RPWM_PIN, 0);
    ledcWrite(LPWM_PIN, pwm);
}

uint8_t calculatePWM(float error)
{
    error = constrain(error,
                      DEAD_BAND_Y,
                      MAX_ERROR_Y);

    float pwm =
        MIN_PWM +
        ((error - DEAD_BAND_Y) *
        (MAX_PWM - MIN_PWM)) /
        (MAX_ERROR_Y - DEAD_BAND_Y);

    pwm = constrain(pwm,
                    MIN_PWM,
                    MAX_PWM);

    return (uint8_t)pwm;
}

void controlActuator()
{
    if (!trackingEnable)
    {
        actuatorStop();
        return;
    }

    int error = abs(errorVertical);

    if (error <= DEAD_BAND_Y)
    {
        actuatorStop();
        return;
    }

    uint8_t pwm = calculatePWM(error);

    if (errorVertical > 0)
    {
        actuatorExtend(pwm);
    }
    else
    {
        actuatorRetract(pwm);
    }
}