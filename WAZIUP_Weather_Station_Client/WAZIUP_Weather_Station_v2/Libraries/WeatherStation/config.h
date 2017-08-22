#ifndef _H_CONFIG
#define _H_CONFIG

#include "Arduino.h"
#include <avr/dtostrf.h>    // Uncomment to use in Feather M0

//----------- Uncomment to print via serial monitor -----------
//#define PRINT
//#define DEBUG
//#define DEBUG_INTERRUPTS


// Voltage Battery Voltage Pin
#define VBATPIN A7

// Define Baudrate
#define BAUDRATE 9600

//// Defines for LoRa module
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 868.0   // Change to 434.0 or other frequency, must match RX's freq!

// Defines for Weather Sheild
#define WSPEED 11
#define RAIN 12
#define WDIR A0
#define WDIR_CALIBRATION 0.973    // (990/1017) 990 = max value table , 1017 max adc acquired

#define PERIOD 10   // 60 minutes



#endif
