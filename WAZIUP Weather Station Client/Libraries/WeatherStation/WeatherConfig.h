/*
Adafruit FEATHER 34u2

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// digital I/O pins
const byte WSPEED = 1;
const byte RAIN = 0;
const byte STAT1 = 12;
const byte STAT2 = 13;

// analog I/O pins
const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;
const byte WDIR = A0;


 pinMode(STAT1, OUTPUT); //Status LED Blue
 pinMode(STAT2, OUTPUT); //Status LED Green

 pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
 pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor

 pinMode(REFERENCE_3V3, INPUT);
 pinMode(LIGHT, INPUT);


 // attach external interrupt pins to IRQ functions
  attachInterrupt(2, rainIRQ, FALLING);
  attachInterrupt(3, wspeedIRQ, FALLING);

  // turn on interrupts
  interrupts();
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


class WeatherConfig {

public:
  WeatherConfig();

  //Public Functions

void calibration();

float get_humidity();

float get_temperature();

float get_temperature_f();

float get_pressure();

float get_light_level(float value1, float value2);

float get_battery_level(float, float);

int get_wind_direction(const byte);



  //Public Variables

private:
  //Private Functions

  //Private Variables

};
