#ifndef SENSOR_HUMIDITY
#define SENSOR_HUMIDITY

#include <Arduino.h>

#include <SPI.h>

#include <Wire.h>


// -------- Uncomment the sensor to be used --------
//#define USE_SI7021
#define USE_AM2315 // to use
//--------------------------------------------------


#ifdef USE_SI7021
	#include "SparkFun_Si7021_Breakout_Library.h"
#endif

#ifdef USE_AM2315
	#include "Adafruit_AM2315.h"
#endif



class SensorHumidity
{
	public:
	//Public Functions
	SensorHumidity();
	int begin();			// Begin the Humidity sensor selected (return 0 if ok, return 1 if erro)
	float getHumidity();	// Return the Humidity sensor measure (%)


	//Public Variables

	private:
		#ifdef USE_SI7021
			Weather si70;			// Define the sensor object
		#endif

		#ifdef USE_AM2315
			Adafruit_AM2315 am23;	// Define the sensor object
		#endif

	//Private Functions

	//Private Variables
};

#endif
