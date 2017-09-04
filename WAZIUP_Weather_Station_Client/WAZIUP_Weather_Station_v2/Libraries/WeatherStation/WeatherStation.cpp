
#include <WeatherStation.h>




//Initialize
WeatherStation::WeatherStation(HardwareSerial* Serial)
	:ina219(0x41),
	wc(RFM95_CS, RFM95_RST, RFM95_INT, RF95_FREQ),
	sr(RAIN),
	sw(WSPEED, WDIR_CALIBRATION, WDIR)
{
	DefaultSerial = Serial;
	minutes = 0;
	temperature = 0;
	humidity = 0;
	pressure = 0;
	windDirection = 0;
	windGust = 0;
	windSpeed = 0;
	amountRain = 0;
	batteryVoltage = 0;
	batteryVoltage2 = 0;
	currentNow=0;
	currentMin=9999;
	currentMax=-9999;
	time_before=0;
	time_after=0;
}

	//---------------------------------	Inicialize Weather Station --------------------------------------
	void WeatherStation::init()
	{
		#ifdef PRINT
		(*DefaultSerial).begin(BAUDRATE);
		#endif

		Wire.begin();        // Initiate the Wire library and join the I2C bus

		analogReference(AR_DEFAULT);

		// Initiate the sensors
		if( begin()!=0)
		{
		  #ifdef PRINT
		    (*DefaultSerial).println();
		    (*DefaultSerial).println(F("Weather Station Sensors Erro!"));
		  #endif
		}
		else
		{
		  #ifdef PRINT
		    (*DefaultSerial).println();
		    (*DefaultSerial).println(F("Weather Station Sensors Founded!"));
		  #endif
		}

		interrupts(); 				// Turn on interrupts
		sw.attachWindInterrupt();   // Attach Wind Interrupt
		sr.attachRainInterrupt();   // Attach Rain Interrupt

		clearValues();        // Clear variables, counters
	}
	//---------------------------------------------------------------------------------------------------


	//---------------------------------- To begin RTC & config LoRa -------------------------------------
	int WeatherStation::begin()
	{
		  #ifdef PRINT
	      (*DefaultSerial).println();
		  #endif

		  // RTC Sensor
		  if(rtc.begin()!=0)
		  {
		  	#ifdef PRINT
		    (*DefaultSerial).println(F("RTC Sensor Not Found!"));
		    #endif
		    return 1;
		  }
		  else
		  {
		  	#ifdef PRINT
		    (*DefaultSerial).println(F("RTC Sensor Found!"));
		    #endif
		    rtc.configRTC();    // Configure RTC
		  }

			// LoRa module
		  if( wc.configLoRa()!=0)
		  {
		    #ifdef PRINT
		      (*DefaultSerial).println(F("LoRa module config Erro!"));
		    #endif
		    return 1;
		  }
		  else
		  {
		    #ifdef PRINT
		      (*DefaultSerial).println(F("LoRa module config with Sucess!"));
		    #endif
		  }

			// To inicialize the sensors
			if( beginSensors()!=0)
				return 1;

		return 0;
	}
	//---------------------------------------------------------------------------------------------------


	//----------------------- To begin the Weather Station mesurement sensors ---------------------------
	int WeatherStation::beginSensors()
	{
			ina219.begin();   // Connect to INA219 module.

			// Pressure Sensor
			sp.begin();

			// Temperature Sensor
			if(st.begin()!=0)
			{
				#ifdef PRINT
				(*DefaultSerial).println(F("Temperature Sensor Not Found!"));
				#endif
				return 1;
			}
			else
			{
				#ifdef PRINT
				(*DefaultSerial).println(F("Temperature Sensor Found!"));
				#endif
			}

			// Humidity Sensor
			if(sh.begin()!=0)
			{
				#ifdef PRINT
				(*DefaultSerial).println(F("Humidity Sensor Not Found!"));
				#endif
				return 1;
			}
			else
			{
				#ifdef PRINT
				(*DefaultSerial).println(F("Humidity Sensor Found!"));
				#endif
			}

			return 0;
	}
	//---------------------------------------------------------------------------------------------------


	//----------------------- Clear Weather Station Interrupts, Couters & Values ------------------------
	void WeatherStation::clearValues()
	{
		  temperature = 0;
		  humidity = 0;
		  pressure = 0;
		  windDirection = 0;
		  windGust=0;
		  windSpeed = 0;
		  amountRain = 0;
		  batteryVoltage=0;
			batteryVoltage2=0;
		  currentNow=0;
		  currentMin=9999;
		  currentMax=-9999;

		  minutes=0;

		  for (int i=0; i<PERIOD; i++)
		  {
		    temperatures[i]=0;
		    humidities[i]=0;
		    pressures[i]=0;
		    voltages[i]=0;
				voltages2[i]=0;
		  }

		  sw.clearWind();
		  sr.clearRain();

		  rtc.clearRTCAlarm();
	}
	//---------------------------------------------------------------------------------------------------


	//---------------------- Get the Weather measurements for each minute -------------------------------
	void WeatherStation::getWeatherValues()
	{
	  voltages2[minutes-1] = ina219.getBusVoltage_V(); 	// Battery Voltage (V)
		voltages[minutes-1] = getBatteryVoltage(); 	// Battery Voltage (V)
	  currentNow=ina219.getCurrent_mA();				// Current (mA)
	  temperatures[minutes-1] = st.getTemperature(); 	// Temperature (Cº)
	  humidities[minutes-1] = sh.getHumidity();      	// Humidity (%)
		//(*DefaultSerial).print(F(" 1PA: "));                     (*DefaultSerial).println(sp.getPressure());
		//(*DefaultSerial).print(F(" 2PA: "));                     (*DefaultSerial).println(sp.getPressure());
		pressures[minutes-1] = sp.getPressureKPa();   	// Pressure (Pa)

	  int windIndice = sw.incrementWindIndice();		// Increment Wind Indice

	  compareCurrents(currentNow);		// Get the Max and Min current

	  rtc.clearRTCAlarm();    // Clear RTC Alarm control

	  #ifdef DEBUG
	      (*DefaultSerial).println(F(""));
	      (*DefaultSerial).print(F(" ==> Minute: "));             (*DefaultSerial).print(minutes);
	      (*DefaultSerial).print(F(" TP: "));                     (*DefaultSerial).print(temperatures[minutes-1]);
	      (*DefaultSerial).print(F(" HU: "));                     (*DefaultSerial).print(humidities[minutes-1]);
	      (*DefaultSerial).print(F(" PA: "));                     (*DefaultSerial).print(pressures[minutes-1]);
	      (*DefaultSerial).print(F(" Instant Rain: "));           (*DefaultSerial).print(sr.getRain());
  	    (*DefaultSerial).print(F(" Instant W_INDIce: "));       (*DefaultSerial).print(windIndice);
	      (*DefaultSerial).print(F(" Instant W_SPEED: "));        (*DefaultSerial).print(sw.getWindSpeed(minutes));
	      (*DefaultSerial).print(F(" B_fea: " ));       		 			(*DefaultSerial).print(voltages[minutes-1]);
				///
				(*DefaultSerial).print(F(" B_INA: " ));       		 			(*DefaultSerial).print(voltages2[minutes-1]);
				///
				(*DefaultSerial).print(F(" Current_INA:" ));       	 		(*DefaultSerial).print(currentNow);
	      (*DefaultSerial).println(F(""));
	  #endif

	}
	//---------------------------------------------------------------------------------------------------


	//------------------------- Get the Weather measurements for the Hour -------------------------------
	void WeatherStation::getWeatherAverage()
	{
		  // Get each minute weather measures --------------------------
		  for (int i=0; i<minutes; i++)
		  {
		    temperature += temperatures[i];
		    humidity += humidities[i];
		    pressure += pressures[i];
		    batteryVoltage += voltages[i];
				///
				batteryVoltage2 += voltages2[i];
				///

		    #ifdef DEBUG
		        // for testing
		        (*DefaultSerial).print(F(" i: "));     (*DefaultSerial).print(i);
		        (*DefaultSerial).print(F(" TP: "));    (*DefaultSerial).print(temperatures[i]);
		        (*DefaultSerial).print(F(" HU: "));    (*DefaultSerial).print(humidities[i]);
		        (*DefaultSerial).print(F(" PA: "));    (*DefaultSerial).print(pressures[i]);
		        (*DefaultSerial).print(F(" BV: "));    (*DefaultSerial).print(voltages[i]);
						///
						(*DefaultSerial).print(F(" BV2: "));    (*DefaultSerial).println(voltages2[i]);
						///
		    #endif
		  }

		  #ifdef DEBUG
		        // for testing
		        (*DefaultSerial).print(F(" MaxC: "));    (*DefaultSerial).print(currentMax);
		        (*DefaultSerial).print(F(" MinC: "));    (*DefaultSerial).println(currentMin);
		  #endif

		  windDirection = sw.getWindDirection(&Serial1);  // Wind Direction (º)
		  windSpeed = sw.getWindSpeed(minutes);   // Wind Speed (kPh)
		  windGust = sw.getWindGust();   // Wind Gust (kPh)
		  amountRain = sr.getRain();        // Rain (mm)

		  // --------------------------------------------------------



		  // Set measures to Weather Record object
		  wr.setTemperature(temperature/minutes); // Temperature (Cº)
		  wr.setHumidity(humidity/minutes);       // Humidity (%)
		  wr.setPressure(pressure/minutes);       // Pressure (Pa)
		  wr.setWindDirection(windDirection);     // Wind Direction (º)
		  wr.setWindSpeed(windSpeed);             // Wind Speed (kPh)
		  wr.setWindClicks(sw.getWindClicks());                 // Wind Clicks (interrupts)
		  wr.setAmountRain(amountRain);           // Amount of Rain (mm)
		  wr.setRainClicks(sr.getRainClicks());                  // Rain Clicks (interrupts)
		  wr.setWindGust(windGust);               // Wind Gust (kPh)
		  wr.setBatteryVoltage(batteryVoltage/minutes);   // Battery Voltage (V)
			wr.setBatteryINA(batteryVoltage2/minutes);   // Battery Voltage INA (V)
		  wr.setMaxCurrent(currentMax);			  // Min Current (mA)
		  wr.setMinCurrent(currentMin);			  // Max Current (mA)
	}
	//---------------------------------------------------------------------------------------------------


	//-------------------------------- Get the Max and Min current --------------------------------------
	void WeatherStation::compareCurrents(float current)
	{
		if (current>currentMax)
			currentMax=current;

		if (current<currentMin)
			currentMin=current;
	}
	//---------------------------------------------------------------------------------------------------

	//------------------------------ Weather Station Operation Task -------------------------------------
	int WeatherStation::task()
	{
		// For each RTC Alarm
		  if (rtc.getRTCAlarm()==1)
		  {

		    minutes++;

		    getWeatherValues();   // For each minute get the respective minute weather measures

		      // For each hour: get weather averages, send mensage and clear the values
		      if (minutes==PERIOD)
		      {
		          #ifdef DEBUG
		            (*DefaultSerial).println(F(""));
		            (*DefaultSerial).println(F(" |||||| Weather Average |||||| "));
		          #endif

		          getWeatherAverage();   // Get weather measure averages

		          //wc.sendMessageSerial(wr, DefaultSerial);         // Send message
		          wc.sendMessageLoRa(wr);

		          clearValues();         // Clear variables, counters
		      }
		      return 0;		// Send period reached
		  }

			// Wind Direction Tendency
			time_after=millis();
			if ((time_after-time_before) > 500)
			{
				time_before=millis();
				int windIndice = sw.incrementWindIndice();
			}




		  		#ifdef DEBUG_INTERRUPTS
				  // for testing
				  if(sw.getWindControl()==1 || sr.getRainControl()==1)
				  {
				    // for testing WIND
				    (*DefaultSerial).println(F(""));
				    (*DefaultSerial).print(F("WindClicks: "));     (*DefaultSerial).print(sw.getWindClicks());
				    (*DefaultSerial).print(F(" RainClicks: "));    (*DefaultSerial).print(sr.getRainClicks());
				    (*DefaultSerial).print(F(" Instant Rain: "));  (*DefaultSerial).print(sr.getRain());
				    (*DefaultSerial).print(F(" W_indice: "));    	(*DefaultSerial).print(sw.getWindIndice());

				    // for testing
				    (*DefaultSerial).print(F(" NEW(t_min): "));    (*DefaultSerial).print(sw.getGustInterval());
				    windGust=sw.getWindGust();
				    (*DefaultSerial).print(F(" NEW_Gust: "));     	(*DefaultSerial).print(windGust);
				    (*DefaultSerial).print(F(" Instant Power: "));     (*DefaultSerial).print(ina219.getCurrent_mA());

						(*DefaultSerial).print(F(" B_INA: "));     (*DefaultSerial).print(ina219.getBusVoltage_V());
						(*DefaultSerial).print(F(" B_fea: "));     (*DefaultSerial).println(getBatteryVoltage());


				    sw.clearWindControl();
				    sr.clearRainControl();
				    }
				#endif

		return 1; /// Para continuar no ciclo
	}
	//---------------------------------------------------------------------------------------------------


	//------------------------------------ Get Battery Voltage ------------------------------------------
	float WeatherStation::getBatteryVoltage()
	{
		float measuredvbat = analogRead(VBATPIN);

		measuredvbat *= 2;    // we divided by 2, so multiply back
		measuredvbat *= 3.2;  // Multiply by 3.3V, our reference voltage
		//measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
		measuredvbat /= 1024; // convert to voltage

		return measuredvbat;
	}
	//---------------------------------------------------------------------------------------------------
