
#include "SensorPressure.h"


//Initialize
SensorPressure::SensorPressure()
{

}

	//-------------------------------- Begin Pressure sensor --------------------------------------
	void SensorPressure::begin()
	{
		SPI.begin();

		#ifdef USE_MPL3115A2
			mpl31.begin();
			mpl31.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  		//mpl31.setOversampleRate(7); // Set Oversample to the recommended 128
			mpl31.setOversampleRate(128); // Set Oversample to the recommended 128
  		mpl31.enableEventFlags(); // Enable all three pressure and temp event flags
  			mpl31.readTemp();
  			mpl31.readPressure();
  			//return 0;
		#endif
	}
	//----------------------------------------------------------------------------------------------

	//---------------------------------------- Get Pressure ----------------------------------------
	float SensorPressure::getPressure()
	{

		#ifdef USE_MPL3115A2
			mpl31.readTemp();
			mpl31.readPressure();
			return mpl31.readPressure();
	  #endif
	}
	//----------------------------------------------------------------------------------------------

	//----------------------------------- Get Pressure (Pascal) ------------------------------------
	float SensorPressure::getPressurePa()
	{
			return getPressure();
	}
	//----------------------------------------------------------------------------------------------

	//------------------------------- Get Pressure (kilo Pascal) -----------------------------------
	float SensorPressure::getPressureKPa()
	{
			return (getPressure()/1000);
	}
	//----------------------------------------------------------------------------------------------
