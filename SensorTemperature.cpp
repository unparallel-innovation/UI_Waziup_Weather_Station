
#include "SensorTemperature.h"


//Initialize
SensorTemperature::SensorTemperature()
{

}

	//------------------------------- Begin Temperature sensor -------------------------------------
	int SensorTemperature::begin()
	{
		SPI.begin();

		#ifdef USE_SI7021
			if(si70.begin()!=0)
				return 0;
			else
				return 1;
		#endif

		#ifdef USE_MPL3115A2
			mpl31.begin();
			mpl31.setModeBarometer(); 	// Measure pressure in Pascals from 20 to 110 kPa
  			mpl31.setOversampleRate(7); // Set Oversample to the recommended 128
  			mpl31.enableEventFlags(); 	// Enable all three pressure and temp event flags
  			mpl31.readTemp();
  			mpl31.readPressure();
  			return 0;
		#endif

  		#ifdef USE_AM2315
  			if(am23.begin())
				{
					delay(100);
					return 0;
				}
  			else
					return 1;
  		#endif
	}
	//----------------------------------------------------------------------------------------------

	//------------------------------------	Get Temperature ----------------------------------------
	float SensorTemperature::getTemperature()
	{
		#ifdef USE_SI7021
			return si70.getTemp();
		#endif

		#ifdef USE_MPL3115A2
			return mpl31.readTemp();
	  	#endif

		#ifdef USE_AM2315
			float temperature = am23.readTemperature();
			delay(100);
			return temperature;
	  #endif
	}
	//----------------------------------------------------------------------------------------------
