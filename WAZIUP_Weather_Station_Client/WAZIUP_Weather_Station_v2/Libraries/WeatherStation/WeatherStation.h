#ifndef WEATHER_STATION
#define WEATHER_STATION

#include <Arduino.h>

// Include Weather Station configuration
#include "config.h"

// Include Weather Station Objects
#include <Adafruit_INA219.h>
#include <SensorRTC.h>
#include <WeatherRecord.h>
#include <WeatherComunication.h>
#include <SensorTemperature.h>
#include <SensorHumidity.h>
#include <SensorPressure.h>
#include <SensorRain.h>
#include <SensorWind.h>


class WeatherStation
{

	public:
	//Public Functions
	WeatherStation(HardwareSerial* DefaultSerial);
	void init();
	int begin();
	int beginSensors();
	void clearValues();
	void getWeatherValues();
	void getWeatherAverage();
	void compareCurrents(float current);
	int task();
	float getBatteryVoltage();



	//Public Variables

	private:
	Adafruit_INA219 ina219;
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

	float temperatures[PERIOD];
	float humidities[PERIOD];
	float pressures[PERIOD];
	float voltages[PERIOD];
	float voltages2[PERIOD];

	float temperature;
	float humidity;
	float pressure;
	float batteryVoltage;
	float batteryVoltage2;
	float windDirection;
	float windGust;
	float windSpeed;
	float amountRain;

	float currentNow;
	float currentMin;
	float currentMax;

	HardwareSerial* DefaultSerial;
};

#endif
