#include "WeatherStation.h"





//Initialize Weather Station
WeatherStation::WeatherStation(float rf_lora_freq, int wind_speed, int rain, int wind_dir, int period_time)
:
#ifdef ENABLE_INA
ina219(0x41),
#endif
wc(RFM95_CS, RFM95_RST, RFM95_INT, rf_lora_freq),
sr(rain),
sw(wind_speed, WDIR_CALIBRATION, wind_dir)
{
	period=period_time;
	minutes = 0;
	temperature = 0;
	humidity = 0;
	pressure = 0;
	windDirection = 0;
	windGust = 0;
	windSpeed = 0;
	amountRain = 0;
	batteryVoltage = 0;
	time_before=0;
	time_after=0;

	temperatures = new float[period_time];
	humidities = new float[period_time];
	pressures = new float[period_time];
	voltages = new float[period_time];

	#ifdef ENABLE_INA
	batteryVoltage2 = 0;
	currentNow=0;
	currentMin=9999;
	currentMax=-9999;
	voltages2= new float[period_time];
	#endif
}

//Initialize Weather Station
WeatherStation::WeatherStation(float rf_lora_freq, int wind_speed, int rain, int wind_dir, int period_time, const char* channel)
:
#ifdef ENABLE_INA
ina219(0x41),
#endif
wc(RFM95_CS, RFM95_RST, RFM95_INT, rf_lora_freq, channel),
sr(rain),
sw(wind_speed, WDIR_CALIBRATION, wind_dir)
{
	period=period_time;
	minutes = 0;
	temperature = 0;
	humidity = 0;
	pressure = 0;
	windDirection = 0;
	windGust = 0;
	windSpeed = 0;
	amountRain = 0;
	batteryVoltage = 0;
	time_before=0;
	time_after=0;

	temperatures = new float[period_time];
	humidities = new float[period_time];
	pressures = new float[period_time];
	voltages = new float[period_time];

	#ifdef ENABLE_INA
	batteryVoltage2 = 0;
	currentNow=0;
	currentMin=9999;
	currentMax=-9999;
	voltages2= new float[period_time];
	#endif
}

	//---------------------------------	Inicialize Weather Station --------------------------------------
	void WeatherStation::init()
	{
		#ifdef ENABLE_DEBUG
		Serial.begin(BAUDRATE);

		unsigned long before = millis();

		while (!Serial)
		{
			unsigned long after = millis();

			if( (after-before)>5000 )
				break;
  	}

		Serial.println(F("------------------------------------------------------------"));
		Serial.println(F("-                  LoRa Weather Station                    -"));
		Serial.println(F("-                                                          -"));
		Serial.println(F("- Copyright (C) UNPARALLEL Innovation, Lda                 -"));
		Serial.println(F("- http://www.unparallel.pt                                 -"));
		Serial.println(F("------------------------------------------------------------"));
		#endif


		Wire.begin();        // Initiate the Wire library and join the I2C bus

		analogReference(AR_DEFAULT);

		// Initiate the sensors
		if( begin()!=0)
		{
		  #ifdef ENABLE_DEBUG
		    Serial.println();
		    Serial.println(F("Weather Station Sensors Erro!"));
				Serial.println(F("------------------------------------------------------------"));
		  #endif
		}
		else
		{
		  #ifdef ENABLE_DEBUG
		    Serial.println();
		    Serial.println(F("Weather Station Sensors Founded!"));
				Serial.println(F("------------------------------------------------------------"));
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
		  #ifdef ENABLE_DEBUG
				Serial.println(F("------------------------------------------------------------"));
	      Serial.println(F("Inicialize Weather Station Sensors:"));
				Serial.println();
		  #endif

		  // RTC Sensor
		  if(rtc.begin()!=0)
		  {
		  	#ifdef ENABLE_DEBUG
		    Serial.println(F("RTC Sensor Not Found!"));
		    #endif
		    return 1;
		  }
		  else
		  {
		  	#ifdef ENABLE_DEBUG
		    Serial.println(F("RTC Sensor Found!"));
		    #endif
		    rtc.configRTC();    // Configure RTC
		  }

			// LoRa module
		  if( wc.configLoRa()!=0)
		  {
		    #ifdef ENABLE_DEBUG
		      Serial.println(F("LoRa module config Erro!"));
		    #endif
		    return 1;
		  }
		  else
		  {
		    #ifdef ENABLE_DEBUG
		      Serial.println(F("LoRa module config with Sucess!"));
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
			#ifdef ENABLE_INA
			ina219.begin();   // Connect to INA219 module.
			#endif

			// Pressure Sensor
			sp.begin();

			// Temperature Sensor
			if(st.begin()!=0)
			{
				#ifdef ENABLE_DEBUG
				Serial.println(F("Temperature Sensor Not Found!"));
				#endif
				return 1;
			}
			else
			{
				#ifdef ENABLE_DEBUG
				Serial.println(F("Temperature Sensor Found!"));
				#endif
			}


			// Humidity Sensor
			if(sh.begin()!=0)
			{
				#ifdef ENABLE_DEBUG
				Serial.println(F("Humidity Sensor Not Found!"));
				#endif
				return 1;
			}
			else
			{
				#ifdef ENABLE_DEBUG
				Serial.println(F("Humidity Sensor Found!"));
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
			minutes=0;

			#ifdef ENABLE_INA
			batteryVoltage2=0;
		  currentNow=0;
		  currentMin=9999;
		  currentMax=-9999;
			#endif


		  for (int i=0; i<period; i++)			// Not necessary clear. Change to next version!
		  {
		    temperatures[i]=0;
		    humidities[i]=0;
		    pressures[i]=0;
		    voltages[i]=0;

				#ifdef DEBUG_UI
				Serial.print("Temperature"); Serial.print(i); Serial.print(": "); Serial.print(temperatures[i]);
				Serial.print(" Humidity"); Serial.print(i); Serial.print(": "); Serial.print(humidities[i]);
				Serial.print(" Pressure"); Serial.print(i); Serial.print(": "); Serial.print(pressures[i]);
				Serial.print(" Voltage"); Serial.print(i); Serial.print(": "); Serial.println(voltages[i]);
				#endif

				#ifdef ENABLE_INA
				voltages2[i]=0;
				#endif
		  }

		  sw.clearWind();
		  sr.clearRain();

		  rtc.clearRTCAlarm();
	}
	//---------------------------------------------------------------------------------------------------


	//---------------------- Get the Weather measurements for each minute -------------------------------
	void WeatherStation::getWeatherValues()
	{
	  temperatures[minutes-1] = st.getTemperature(); 	// Temperature (Cº)
	  humidities[minutes-1] = sh.getHumidity();      	// Humidity (%)
		pressures[minutes-1] = sp.getPressureKPa();   	// Pressure (Pa)
		voltages[minutes-1] = getBatteryVoltage(); 	// Battery Voltage (V)
	  int windIndice = sw.incrementWindIndice();		// Increment Wind Indice

		#ifdef ENABLE_INA
		voltages2[minutes-1] = ina219.getBusVoltage_V(); 	// Battery Voltage (V)
		currentNow=ina219.getCurrent_mA();				// Current (mA)

	  compareCurrents(currentNow);		// Get the Max and Min current
		#endif

	  rtc.clearRTCAlarm();    // Clear RTC Alarm control

	  #ifdef ENABLE_DEBUG
				Serial.println();
				Serial.println(F("-----------------------------------------------------------------------------------------------------------"));
	      Serial.print(F(" ==> Weather values for minute "));     Serial.print(minutes);	Serial.println(F(" <=="));
				Serial.println();
				Serial.print(F(" Temperature: "));                     Serial.print(temperatures[minutes-1]);		Serial.print(F("ºC "));
	      Serial.print(F(" Humidity: "));                     Serial.print(humidities[minutes-1]);	Serial.print(F("% "));
	      Serial.print(F(" Pressure: "));                     Serial.print(pressures[minutes-1]);	Serial.println(F("KPa "));
	      Serial.print(F(" Rain: "));           Serial.print(sr.getRain());	Serial.println(F("mm "));
	      Serial.print(F(" Wind Speed: "));        Serial.print(sw.getWindSpeed(minutes));	Serial.print(F("km/h "));
				Serial.print(F(" Wind Gust: "));        Serial.print(sw.getWindGust());	Serial.print(F("km/h "));
				Serial.print(F(" Wind Direction: "));        Serial.print(sw.getWindDirection());	Serial.println(F("º "));
	      Serial.print(F(" Battery Voltage: " ));       		 			Serial.print(voltages[minutes-1]);	Serial.print(F("V "));
				#ifdef ENABLE_INA
				Serial.print(F(" Battery Voltages (from INA): " ));       		 			Serial.print(voltages2[minutes-1]);	Serial.print(F("V "));
				Serial.print(F(" Current (from INA):" ));       	 		Serial.print(currentNow);		Serial.print(F("mA "));
				#endif
				Serial.println();
	      Serial.println(F("-----------------------------------------------------------------------------------------------------------"));
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
				#ifdef ENABLE_INA
				batteryVoltage2 += voltages2[i];
				#endif

		    #ifdef DEBUG_UI
				        // for testing
		        Serial.print(F(" i: "));     Serial.print(i);
		        Serial.print(F(" TP: "));    Serial.print(temperatures[i]);
		        Serial.print(F(" HU: "));    Serial.print(humidities[i]);
		        Serial.print(F(" PA: "));    Serial.print(pressures[i]);
		        Serial.print(F(" BV: "));    Serial.print(voltages[i]);
						#ifdef ENABLE_INA
						Serial.print(F(" BV2: "));    Serial.println(voltages2[i]);
						#endif
		    #endif
		  }

		  #ifdef ENABLE_INA
		        #ifdef DEBUG_UI
		        Serial.print(F(" MaxC: "));    Serial.print(currentMax);
		        Serial.print(F(" MinC: "));    Serial.println(currentMin);
						#endif
		  #endif

		  windDirection = sw.getWindDirection();  // Wind Direction (º)
		  windSpeed = sw.getWindSpeed(minutes);   // Wind Speed (km/h)
		  windGust = sw.getWindGust();   // Wind Gust (km/hh)
		  amountRain = sr.getRain();        // Rain (mm)


			#ifdef ENABLE_UI
			Serial.println();
			Serial.println(F("-----------------------------------------------------------------------------------------------------------"));
			Serial.print(F(" ==> Send Period Reached! Weather values average for the last "));     Serial.print(minutes);	Serial.println(F(" minutes <=="));
			Serial.println();
			Serial.print(F(" Temperature: "));                     Serial.print(temperature/minutes);		Serial.print(F("ºC "));
			Serial.print(F(" Humidity: "));                     Serial.print(humidity/minutes);	Serial.print(F("% "));
			Serial.print(F(" Pressure: "));                     Serial.print(pressure/minutes);	Serial.println(F("KPa "));
			Serial.print(F(" Rain: "));           Serial.print(amountRain);	Serial.println(F("mm "));
			Serial.print(F(" Wind Speed: "));        Serial.print(windSpeed);	Serial.print(F("km/h "));
			Serial.print(F(" Wind Gust: "));        Serial.print(windGust);	Serial.print(F("km/h "));
			Serial.print(F(" Wind Direction: "));        Serial.print(windDirection);	Serial.println(F("º "));
			Serial.print(F(" Battery Voltage: " ));       		 			Serial.print(batteryVoltage/minutes);	Serial.print(F("V "));
				#ifdef ENABLE_INA
				Serial.print(F(" Battery Voltages (from INA): " ));       		 			Serial.print(batteryVoltage2/minutes);	Serial.print(F("V "));
				Serial.print(F(" Maximum Current  (from INA):" ));       	 		Serial.print(currentMax);		Serial.print(F("mA "));
				Serial.print(F(" Minimum Current  (from INA):" ));       	 		Serial.print(currentMin);		Serial.print(F("mA "));
				#endif
			Serial.println();
			Serial.println(F("-----------------------------------------------------------------------------------------------------------"));
			#endif



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
			#ifdef ENABLE_INA
			wr.setBatteryINA(batteryVoltage2/minutes);   // Battery Voltage INA (V)
		  wr.setMaxCurrent(currentMax);			  // Min Current (mA)
		  wr.setMinCurrent(currentMin);			  // Max Current (mA)
			#endif
	}
	//---------------------------------------------------------------------------------------------------


	//-------------------------------- Get the Max and Min current --------------------------------------
	#ifdef ENABLE_INA
	void WeatherStation::compareCurrents(float current)
	{
		if (current>currentMax)
			currentMax=current;

		if (current<currentMin)
			currentMin=current;
	}
	#endif
	//---------------------------------------------------------------------------------------------------

	//------------------------------ Weather Station Operation Task -------------------------------------
	void WeatherStation::task()
	{
		// For each RTC Alarm
		  if (rtc.getRTCAlarm()==1)
		  {

		    minutes++;

		    getWeatherValues();   // For each minute get the respective minute weather measures

		      // For each hour: get weather averages, send mensage and clear the values
		      if (minutes==period)
		      {
		          getWeatherAverage();   // Get weather measure averages

		          wc.sendMessageSerial(wr);         // Send message
		          wc.sendMessageLoRa(wr);

		          clearValues();         // Clear variables, counters
		      }
		  }

			// Wind Direction Tendency
			time_after=millis();
			if ((time_after-time_before) > 500)
			{
				time_before=millis();
				int windIndice = sw.incrementWindIndice();
			}


  		#ifdef ENABLE_DEBUG
		  // for testing
			  if(sw.getWindControl()==1)
			  {
						Serial.println();
						Serial.println(F("++++++++++++++++++++++++++++++ Wind Interrupt ++++++++++++++++++++++++++++++"));
				    Serial.print(F(" WindClicks (in period): "));     Serial.print(sw.getWindClicks());	Serial.println();
						Serial.print(F(" WindDirection: "));    Serial.print(sw.getWindDirection());	Serial.print(F("º "));
						Serial.print(F(" Last WindGust: "));    Serial.print(sw.getGustInterval());	Serial.print(F("ms "));
				    windGust=sw.getWindGust();
				    Serial.print(F(" Actual WindGust: "));     	Serial.print(windGust);	Serial.println(F("ms "));
						Serial.println(F("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"));

				    sw.clearWindControl();
			   }

				 if(sr.getRainControl()==1)
			   {
	 						Serial.println();
	 						Serial.println(F("########## Rain Interrupt ##########"));
	 				    Serial.print(F(" RainClicks (in period): "));    Serial.print(sr.getRainClicks());  Serial.println();
							Serial.print(F(" Rain: "));  Serial.print(sr.getRain()); Serial.println(F("mm "));
							Serial.println(F("####################################"));

				   sr.clearRainControl();
			   }
		  #endif
	}
	//---------------------------------------------------------------------------------------------------


	//------------------------------------ Get Battery Voltage ------------------------------------------
	float WeatherStation::getBatteryVoltage()
	{
		float measuredvbat = analogRead(VBATPIN);

		measuredvbat *= 2;    // we divided by 2, so multiply back
		measuredvbat *= 3.2;  // Multiply by 3.3V, our reference voltage
		measuredvbat /= 1024; // convert to voltage

		return measuredvbat;
	}
	//---------------------------------------------------------------------------------------------------
