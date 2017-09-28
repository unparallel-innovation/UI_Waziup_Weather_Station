#ifndef WEATHER_STATION
#define WEATHER_STATION

#include <Arduino.h>

#define ENABLE_DEBUG


#define VBATPIN A7    // Voltage Battery Voltage Pin
#define WDIR_CALIBRATION 0.973    // (990/1017) 990 = max value table, 1017 max ADC acquired

// LoRa module pins to Adafruit Feather M0
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Define Baudrate
#define BAUDRATE 9600

// Include Weather Station Objects
#ifdef ENABLE_INA
#include "Adafruit_INA219.h"
#endif
#include "SensorRTC.h"
#include "WeatherRecord.h"
#include "WeatherComunication.h"
#include "SensorTemperature.h"
#include "SensorHumidity.h"
#include "SensorPressure.h"
#include "SensorRain.h"
#include "SensorWind.h"


class WeatherStation
{

	public:
	//Public Functions
	WeatherStation(float rf_lora_freq, int wind_speed, int rain, int wind_dir, int period_time);
	WeatherStation(float rf_lora_freq, int wind_speed, int rain, int wind_dir, int period_time, const char* channel);
	void init();
	int begin();
	int beginSensors();
	void clearValues();
	void getWeatherValues();
	void getWeatherAverage();
	#ifdef ENABLE_INA
	void compareCurrents(float current);
	#endif
	void task();
	float getBatteryVoltage();



	//Public Variables
	private:
	#ifdef ENABLE_INA
	Adafruit_INA219 ina219;
	#endif
	SensorRTC rtc;
	WeatherRecord wr;
	WeatherComunication wc;
	SensorTemperature st;
	SensorHumidity sh;
	SensorPressure sp;
	SensorRain sr;
	SensorWind sw;




	//Private Functions

	//Private Variables
	unsigned long minutes;
	unsigned long time_before;
	unsigned long time_after;


	int period;

	//HardwareSerial* DefaultSerial;		Delete

	float * temperatures;
	float * humidities;
	float * pressures;
	float * voltages;

	float temperature;
	float humidity;
	float pressure;
	float batteryVoltage;

	float windDirection;
	float windGust;
	float windSpeed;
	float amountRain;

	#ifdef ENABLE_INA
	float * voltages2;
	float batteryVoltage2;
	float currentNow;
	float currentMin;
	float currentMax;
	#endif
};

#endif
