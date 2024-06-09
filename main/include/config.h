// #include "driver/ledc.h"
// #include "driver/gpio.h"


// ##### WiFi Config #####

#define SSID "FloatWIT" 

#define LED                 (15)
//##### Stepper Config #####

// Pins
#define STEP_PIN            (2)    // The step pin
#define DIR_PIN             (4)   // the dir pin
#define ENABLE              (32)   // the dir pin

// PWM
#define LEDC_TIMER                              LEDC_TIMER_0
#define LEDC_MODE                               LEDC_HIGH_SPEED_MODE

#define LEDC_CHANNEL                            LEDC_CHANNEL_0
#define LEDC_DUTY_RES                           LEDC_TIMER_13_BIT 
#define LEDC_MAX_FREQUENCY                      5000 // Frequency in Hertz
#define LEDC_MIN_FREQUENCY                      0
#define LEDC_DEFAULT_FREQUENCY                  LEDC_MAX_FREQUENCY / 2 


//##### SD-Card Config #####

