
#include <WeatherComunication.h>

//Initialize
WeatherComunication::WeatherComunication(int cs, int rst, int interrupt, float freq)
//	:rf95(cs)	// Singleton instance of the radio driver object
	:rf95(cs, interrupt)	// Singleton instance of the radio driver object
{
	lora_cs = cs;			// Get LoRa module chip select pin
	lora_rst = rst;			// Get LoRa module reset pin
	lora_int = interrupt;	// Get LoRa imodule interrupt pin
	lora_freq = freq;		// Get LoRa module frequency
	cont=0;
}
	//----------------------------------------- Config LoRa ----------------------------------------
	int WeatherComunication::configLoRa()
	{
		if(rf95.init()==1)
		{
			rf95.setFrequency(lora_freq);
			delay(50);
			rf95.sleep();
			return 0;
		}
		return 1;

 		// Probably need to put LoRa mode
	}
	//----------------------------------------------------------------------------------------------

	//---------------------------------	Send Message via Serial ------------------------------------
	void WeatherComunication::sendMessageSerial(WeatherRecord wr, HardwareSerial* DefaultSerial)
	{
	  (*DefaultSerial).println(F(" Sending:"));
	  (*DefaultSerial).print(F(" TP: "));
	  (*DefaultSerial).print(wr.getTemperature());
	  (*DefaultSerial).print(F(" HU: "));
	  (*DefaultSerial).print(wr.getHumidity());
	  (*DefaultSerial).print(F(" PA: "));
	  (*DefaultSerial).print(wr.getPressure());
	  (*DefaultSerial).print(F(" RC: "));
  	  (*DefaultSerial).print(wr.getRainClicks());
	  (*DefaultSerial).print(F(" WC: "));
	  (*DefaultSerial).print(wr.getWindClicks());
	  (*DefaultSerial).print(F(" WS: "));
	  (*DefaultSerial).print(wr.getWindSpeed());
	  (*DefaultSerial).print(F(" WD: "));
	  (*DefaultSerial).print(wr.getWindDirection());
	  (*DefaultSerial).print(F(" WG: "));
	  (*DefaultSerial).print(wr.getWindGust());
	  (*DefaultSerial).print(F(" AR: "));
	  (*DefaultSerial).print(wr.getAmountRain());
	  (*DefaultSerial).print(F(" BV: "));
	  (*DefaultSerial).print(wr.getBatteryVoltage());
		//
		(*DefaultSerial).print(F(" BV_ina: "));
	  (*DefaultSerial).print(wr.getBatteryINA());
		//
	  (*DefaultSerial).print(F(" minC: "));
	  (*DefaultSerial).print(wr.getMinCurrent());
	  (*DefaultSerial).print(F(" maxC: "));
	  (*DefaultSerial).println(wr.getMaxCurrent());
	}
	//----------------------------------------------------------------------------------------------

	//---------------------------------	Send Message via LoRa --------------------------------------
	void WeatherComunication::sendMessageLoRa(WeatherRecord wr)
	{
	  	// Aux variables to define the message
		uint8_t r_size;
	  	char auxBuf[20];
	  	uint8_t data[150];

	  	cont++;

	  	r_size = sprintf((char*)data, "WT123 - %d;", cont);
	  	r_size += sprintf((char*)data + r_size, "TP;%s;", dtostrf(wr.getTemperature(), 2, 2, auxBuf));
	  	r_size += sprintf((char*)data + r_size, "HD;%s;", dtostrf(wr.getHumidity(), 2, 2, auxBuf));
	  	r_size += sprintf((char*)data + r_size, "PA;%s;", dtostrf(wr.getPressure(), 6, 2, auxBuf));
	  	r_size += sprintf((char*)data + r_size, "WD;%s;", dtostrf(wr.getWindDirection(), 3, 2, auxBuf));
	  	r_size += sprintf((char*)data + r_size, "WS;%s;", dtostrf(wr.getWindSpeed(), 3, 2, auxBuf));
	  	r_size += sprintf((char*)data + r_size, "WG;%s;", dtostrf(wr.getWindGust(), 3, 2, auxBuf));
	  	r_size += sprintf((char*)data + r_size, "RA;%s;", dtostrf(wr.getAmountRain(), 2, 2, auxBuf));
	  	r_size += sprintf((char*)data + r_size, "BV;%s;", dtostrf(wr.getBatteryVoltage(), 2, 2, auxBuf));
			//
			r_size += sprintf((char*)data + r_size, "BINA;%s;", dtostrf(wr.getBatteryINA(), 2, 2, auxBuf));
			//
	  	r_size += sprintf((char*)data + r_size, "minC;%s;", dtostrf(wr.getMinCurrent(), 2, 2, auxBuf));
	  	r_size += sprintf((char*)data + r_size, "maxC;%s;", dtostrf(wr.getMaxCurrent(), 2, 2, auxBuf));


		// Send message via LoRa
		rf95.send(data, r_size);
		rf95.waitPacketSent();

		delay(50);
		rf95.sleep();
	}
	//----------------------------------------------------------------------------------------------
