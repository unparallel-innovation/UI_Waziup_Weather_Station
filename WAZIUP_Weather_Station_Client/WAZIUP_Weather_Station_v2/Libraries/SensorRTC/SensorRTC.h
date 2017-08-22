#ifndef SENSOR_RTC
#define SENSOR_RTC

#include <Arduino.h>

#include <SPI.h>

#include <Wire.h>

// -------- Uncomment the sensor to be used --------
#define USE_RCTInt
//--------------------------------------------------

#ifdef USE_RCTInt
	#include <RTCInt.h>
#endif




class SensorRTC
{
	public:
	//Public Functions
	SensorRTC();
	int begin();				// Begin the Pressure sensor selected (return 0 if ok, return 1 if erro)
	void configRTC();			// Config RTC

	void clearRTCAlarm();		// Clear RTC alarm state
	int getRTCAlarm();			// Return RTC alarm state (return 0 if alarm macth, return 1 if erro)


	//Public Variables

	private:
	#ifdef USE_RCTInt
		RTCInt rtc;				// Define RTC object
		void ISR_RTC();
		static void isr2();
		static SensorRTC * SensorRTC_instance;
		volatile unsigned int alarme;
	#endif

	//Private Functions

	//Private Variables
};

#endif