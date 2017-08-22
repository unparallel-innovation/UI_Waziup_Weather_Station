
#include <SensorWind.h>


//Initialize
SensorWind::SensorWind(int interrupt_pin, double calibration, const uint8_t direction_pin)
{
	w_calibration=calibration;
	w_dir_pin=direction_pin;
	w_int_pin=interrupt_pin;
	wind=0;
	lastWindIRQ = millis();
	t_min=36000000;
	t_previous=0;
	t_interval=0;
	control=0;
}

	//---------------------------------------- Get Wind Speed --------------------------------------
	float SensorWind::getWindSpeed(long minutes)
	{
  		float windSpeed=0;

  		float period=0;

  		float deltaTime = (float)minutes*60;    // Convert minutes to seconds

	  	if (wind == 0)
	    	return 0;   // no interrupts return 0 kmh

		  if (deltaTime > 0)
	 	 {

	  	  period = deltaTime/(float)wind;

	  	  windSpeed = 2.4 / period;
	 	 }
	  	return (windSpeed);    // 1 interrupt = 2.4 kPh
	}
	//----------------------------------------------------------------------------------------------


	//---------------------------------- Increment Wind Direction Indice ---------------------------
	int SensorWind::incrementWindIndice()
	{
	  unsigned int indice=getWindIndice();


	  if (indice!=-1)
    		windPos[indice]++;  // Wind Direction (º)

      return indice;
	}
	//----------------------------------------------------------------------------------------------

	//-------------------------------- Returns the Wind Direction Indice ---------------------------
	int SensorWind::getWindIndice()
	{
	  // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
	  // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
	  // Note that these are not in compass degree order! See Weather Meters datasheet for more information.

	  unsigned int adc = w_calibration*analogRead(w_dir_pin);

	  if (adc < 380) return (5);
	  if (adc < 393) return (3);
	  if (adc < 414) return (4);
	  if (adc < 456) return (7);
	  if (adc < 508) return (6);
	  if (adc < 551) return (9);
	  if (adc < 615) return (8);
	  if (adc < 680) return (1);
	  if (adc < 746) return (2);
	  if (adc < 801) return (11);
	  if (adc < 833) return (10);
	  if (adc < 878) return (15);
	  if (adc < 913) return (0);
	  if (adc < 940) return (13);
	  if (adc < 967) return (14);
	  if (adc < 990) return (12);
	  return (-1);
	}
	//----------------------------------------------------------------------------------------------


	//-------------------------------- Returns the Wind Direction Indice ---------------------------
	float SensorWind::getWindDirection(HardwareSerial* DefaultSerial)
	{
	  #ifdef PRINT
	  // For testing
	  (*DefaultSerial).println("");  (*DefaultSerial).println(" |||||| WIND POSICION |||||| ");
	  #endif

	  int count=0;
	  int indice=0;

	  // Get each minute wind direction
	  for (int i=0; i<16; i++)
	  {
	    if (windPos[i] > count)
	    {
	      count = windPos[i];
	      indice = i;
	    }

	    #ifdef PRINT
	    // For testing
        (*DefaultSerial).print(" i: ");    (*DefaultSerial).print(i);    (*DefaultSerial).print(" - ");
        (*DefaultSerial).print(windPos[i]); (*DefaultSerial).println(" x");
        #endif
   	  }

   	  #ifdef PRINT
	  // For testing
		  (*DefaultSerial).print("-- Angle occurencies in the last hour--: ");
		  (*DefaultSerial).print(" i: ");  (*DefaultSerial).print(indice);  (*DefaultSerial).print(" - ");
		  (*DefaultSerial).print(count); (*DefaultSerial).println("x");
	  #endif


    if (indice==5) return (112.5);
	  if (indice==3) return (67.5);
	  if (indice==4) return (90);
	  if (indice==7) return (157.5);
	  if (indice==6) return (135);
	  if (indice==9) return (202.5);
	  if (indice==8) return (180);
	  if (indice==1) return (22.5);
	  if (indice==2) return (45);
	  if (indice==11) return (247.5);
	  if (indice==10) return (225);
	  if (indice==15) return (337.5);
	  if (indice==0) return (0);
	  if (indice==13) return (292.5);
	  if (indice==14) return (315);
	  if (indice==12) return (270);
	  return (-1);    // error
	}
	//----------------------------------------------------------------------------------------------


	//------------------------------------ Returns the Wind Gust -----------------------------------
	float SensorWind::getWindGust()
	{
	  float windGust = 0;

	  if (t_min > 0)
	  {
	      float period = ((float)t_min/1000);    // Convert milliseconds to seconds

	      windGust=2.4/period;   // (1 interrupt/1 second) = 2.4kPh --> in 0.2 seconds --> (2.4/0.2)
	  }
	  return (windGust);
	}
	//----------------------------------------------------------------------------------------------

	//-------------------------------------	Get Wind Clicks ----------------------------------------
	long SensorWind::getWindClicks()
	{
		return wind;
	}
	//----------------------------------------------------------------------------------------------


	//-------------------------------	Get t_min between interrupts -------------------------------
	long SensorWind::getGustInterval()
	{
		return t_min;
	}
	//----------------------------------------------------------------------------------------------

	//-------------------------------- Wind Interrupt Routine --------------------------------------
	void SensorWind::ISR_WIND()
	{
		  // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
		  if (millis() - lastWindIRQ > 10)
		  {
		    wind++;   // There is 1.492MPH for each click per second.
		    lastWindIRQ = millis();   // Grab the current time


		    // For each wind interrupt: get minimum time (in ms) between interrupts
		    t_interval=lastWindIRQ-t_previous;

		     // if is a new minimum time between interrups
		    if(t_interval<t_min)
		      t_min=t_interval;

		    t_previous=lastWindIRQ;
		  }
		  control=1;
	}
	//----------------------------------------------------------------------------------------------


	//--------------------------------- Attach Rain Interrupt --------------------------------------
	void SensorWind::attachWindInterrupt()
	{
		pinMode(w_int_pin, INPUT_PULLUP); // input from wind meters windspeed sensor
		attachInterrupt(digitalPinToInterrupt(w_int_pin), isr0, FALLING);    //FALLING
		SensorWind_instance = this;

	}
	//----------------------------------------------------------------------------------------------

	//----------------------------- Auxiliar Wind Interrupt ISR ------------------------------------
	void SensorWind::isr0()
	{
		SensorWind_instance->ISR_WIND();
	}
	//----------------------------------------------------------------------------------------------



	SensorWind * SensorWind::SensorWind_instance;		// Auxiliar instance to Rain Interrupt ISR




	//--------------------------- Clear the Wind Direction Indice vector ---------------------------
	void SensorWind::clearWind()
	{

	  	// Clear the Wind Direction Indice vector
		for (int i=0; i<16; i++)
  		{
      		windPos[i]=0;
 		}


		t_min=36000000;
  		t_previous=millis();
  		wind=0;
  		control=0;					// (FINAL VERSION: TO BE DELETED)
		t_interval=0;
	}
	//----------------------------------------------------------------------------------------------


	// (FINAL VERSION: TO BE DELETED)
	//-------------------------------------- Set Wind Control --------------------------------------
	void SensorWind::clearWindControl()
	{
		control=0;
	}
	//----------------------------------------------------------------------------------------------

	//-------------------------------------	Get Wind Control ---------------------------------------
	int SensorWind::getWindControl()
	{
		return control;
	}
	//----------------------------------------------------------------------------------------------
