
#include "WeatherConfig.h"
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFunHTU21D.h" //Humidity sensor - Search "SparkFun HTU21D" and install from Library Manager

MPL3115A2 myPressure; //Create an instance of the pressure sensor
HTU21D myHumidity; //Create an instance of the humidity sensor





WeatherConfig::WeatherConfig()
{
  //Set initial values for private vars
  // MPL3115A2 myPressure; //Create an instance of the pressure sensor
  // HTU21D myHumidity; //Create an instance of the humidity sensor
  pinMode(LED_BUILTIN, OUTPUT);

}

void WeatherConfig::calibration(){

  //Configure the pressure sensor
  myPressure.begin(); // Get sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

  //Configure the humidity sensor
  myHumidity.begin();


  while (get_humidity() == 998) //Humidty sensor failed to respond
   {
     digitalWrite(LED_BUILTIN, HIGH);
     Serial.println("I2C communication to sensors is not working. Check solder connections.");

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
     delay(2000);
   }
   digitalWrite(LED_BUILTIN, LOW);
}

float WeatherConfig::get_humidity(){
  return myHumidity.readHumidity();
}

float WeatherConfig::get_temperature(){
  return myHumidity.readTemperature();
}

float WeatherConfig::get_pressure(){
  return myPressure.readPressure();
}

float WeatherConfig::get_temperature_f(){
  return myPressure.readTempF();
}

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float WeatherConfig::get_light_level(float value1, float value2)
{
  float operatingVoltage = value1;

  float lightSensor = value2;

  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V

  lightSensor = operatingVoltage * lightSensor;

  return (lightSensor);
}

float WeatherConfig::get_battery_level(float value1, float value2)
{
  float operatingVoltage = value1;

  float rawVoltage = value2;

  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V

  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin

  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage

  return (rawVoltage);
}

//Read the wind direction sensor, return heading in degrees
int WeatherConfig::get_wind_direction(const byte WDIR)
{
    unsigned int adc;

    adc = analogRead(WDIR); // get the current reading from the sensor


    // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
    // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
    // Note that these are not in compass degree order! See Weather Meters datasheet for more information.


    if (adc < 245) return (113);
    if (adc < 255) return (68);
    if (adc < 265) return (90);
    if (adc < 300) return (158);
    if (adc < 330) return (135);
    if (adc < 375) return (203);
    if (adc < 410) return (180);//Sul
    if (adc < 490) return (23);
    if (adc < 530) return (45);
    if (adc < 620) return (248);
    if (adc < 645) return (225);
    if (adc < 695) return (338);
    if (adc < 755) return (0); //Norte
    if (adc < 785) return (293);
    if (adc < 830) return (315);
    if (adc < 900) return (270);
    return (-1); // error, disconnected?
}
