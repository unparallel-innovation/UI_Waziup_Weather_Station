
#include <SensorRain.h>


//Initialize
SensorRain::SensorRain(int interrupt_pin)
{	
	r_int_pin=interrupt_pin;
	rain=0;
	lastRainIRQ = millis();
	control=0;
}

	//----------------------------------------- Get Rain -------------------------------------------
	float SensorRain::getRain()
	{
		return (float)rain*0.2794;	// 1 interrup represent 0.011" iches of rain 1" --> 0.2794 mm
	}
	//----------------------------------------------------------------------------------------------

	//--------------------------------------- Get Rain Clicks --------------------------------------
	long SensorRain::getRainClicks()
	{
		return rain;
	}
	//----------------------------------------------------------------------------------------------

	//-------------------------------- Rain Interrupt Routine --------------------------------------
	void SensorRain::ISR_RAIN()
	{
		  // Ignore switch-bounce glitches less than 10mS after initial edge
		  if (millis() - lastRainIRQ > 10) 
		  {
		    rain++; 					//Each dump is 0.011" of water
		    lastRainIRQ = millis(); 	// Set up for next event  
		  }
		  control=1;			// (FINAL VERSION: TO BE DELETED)		
	}
	//----------------------------------------------------------------------------------------------

	
	//--------------------------------- Attach Rain Interrupt --------------------------------------
	void SensorRain::attachRainInterrupt()
	{
		
		pinMode(r_int_pin, INPUT_PULLUP); // input from wind meters windspeed sensor
		
		attachInterrupt(digitalPinToInterrupt(r_int_pin), isr1, FALLING);    //FALLING
		SensorRain_instance = this;
	}
	//----------------------------------------------------------------------------------------------

	//----------------------------- Auxiliar Rain Interrupt ISR ------------------------------------
	void SensorRain::isr1()
	{
		SensorRain_instance->ISR_RAIN();
	}
	//----------------------------------------------------------------------------------------------


	
	SensorRain * SensorRain::SensorRain_instance;		// Auxiliar instance to Rain Interrupt ISR 



	//----------------------------------Clear the Rain values -------------------------------------- 
	void SensorRain::clearRain()
	{
  		rain=0;
  		control=0;			// (FINAL VERSION: TO BE DELETED)
	}
	//----------------------------------------------------------------------------------------------


	// (FINAL VERSION: TO BE DELETED)
	//-------------------------------------- Clear Rain Control ------------------------------------
	void SensorRain::clearRainControl()
	{
		control=0;
	}
	//----------------------------------------------------------------------------------------------

	//--------------------------------------- Get Rain Control -------------------------------------
	int SensorRain::getRainControl()
	{
		return control;
	}
	//----------------------------------------------------------------------------------------------
