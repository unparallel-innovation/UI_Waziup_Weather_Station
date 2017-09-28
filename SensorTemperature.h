#ifndef SENSOR_TEMPERATURE
#define SENSOR_TEMPERATURE


#include <Arduino.h>

#include <SPI.h>

#include <Wire.h>


// -------- Uncomment the sensor to be used --------
//#define USE_MPL3115A2
//#define USE_SI7021
#define USE_AM2315 // to use
//--------------------------------------------------

#ifdef USE_MPL3115A2
	#include "SparkFunMPL3115A2.h"
#endif

#ifdef USE_SI7021
	#include "SparkFun_Si7021_Breakout_Library.h"
#endif

#ifdef USE_AM2315
	#include "Adafruit_AM2315.h"
#endif


class SensorTemperature
{
	public:
	//Public Functions
	SensorTemperature();
	int begin();				// Begin the Temperature sensor selected (return 0 if ok, return 1 if erro)
	float getTemperature();		// Return the Temperature sensor measure (ºC)




	//Public Variables

	private:
		#ifdef USE_MPL3115A2
			MPL3115A2 mpl31;			// Define the sensor object
		#endif

		#ifdef USE_SI7021
			Weather si70;				// Define the sensor object
		#endif

		#ifdef USE_AM2315
			Adafruit_AM2315 am23;		// Define the sensor object
		#endif
	//Private Functions

	//Private Variables
};

#endif
