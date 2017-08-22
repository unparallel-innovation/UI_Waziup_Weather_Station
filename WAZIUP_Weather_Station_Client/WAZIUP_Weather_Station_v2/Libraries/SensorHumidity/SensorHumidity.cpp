
#include <SensorHumidity.h>


//Initialize
SensorHumidity::SensorHumidity()
{	

}

	//------------------------------- Begin Humidity sensor -------------------------------------
	int SensorHumidity::begin()
	{
		SPI.begin();

		#ifdef USE_SI7021
			if(si70.begin()!=0)
				return 0;
			else
				return 1;
		#endif

		#ifdef USE_AM2315
  			if(am23.begin())
  				return 0;
  			else
  				return 1;
  		#endif
	}
	//----------------------------------------------------------------------------------------------

	//--------------------------------------	Get Humidity ---------------------------------------
	float SensorHumidity::getHumidity()
	{
		#ifdef USE_SI7021
			return si70.getRH();
		#endif

		#ifdef USE_AM2315
			return am23.readHumidity();
		#endif
	}
	//----------------------------------------------------------------------------------------------





	




