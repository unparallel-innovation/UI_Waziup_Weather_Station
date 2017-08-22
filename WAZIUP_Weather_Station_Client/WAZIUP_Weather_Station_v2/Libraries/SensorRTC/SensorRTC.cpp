
#include <SensorRTC.h>


//Initialize
SensorRTC::SensorRTC()
{	
	alarme=0;
}

	//----------------------------------------- Begin RTC -------------------------------------------
	int SensorRTC::begin()
	{
		#ifdef USE_RCTInt
			rtc.begin(TIME_H24);      //RTC initializing with 24 hour representation mode
			return 0;
		#else
			return 1;
		#endif
	}
	//----------------------------------------------------------------------------------------------

	//---------------------------------------- Config RTC ------------------------------------------
	void SensorRTC::configRTC()
	{
		#ifdef USE_RCTInt
			rtc.setTime(0,0,0,0);     //setting time (hour minute and second)
  
  			rtc.setDate(13,8,15);     //setting date
  
  			rtc.enableAlarm(SEC, ALARM_INTERRUPT, isr2); //enabling alarm and match on second
  			SensorRTC_instance = this;

  			rtc.local_time.second=59;  //setting second to match

  			rtc.setAlarm();  //write second in alarm register
		#endif
	}
	//----------------------------------------------------------------------------------------------


	//------------------------------ RTC Alarm Interrupt Routine -----------------------------------
	void SensorRTC::ISR_RTC()
	{
		  alarme=1;
	}
	//----------------------------------------------------------------------------------------------

	
	//----------------------------- Auxiliar RTC Interrupt ISR -------------------------------------
	void SensorRTC::isr2()
	{
		SensorRTC_instance->ISR_RTC();
	}
	//----------------------------------------------------------------------------------------------


	SensorRTC * SensorRTC::SensorRTC_instance;		// Auxiliar instance to RTC Interrupt ISR 

	
	//---------------------------------- Clear RTC alarme state ------------------------------------
	void SensorRTC::clearRTCAlarm()
	{
		alarme=0;
	}
	//----------------------------------------------------------------------------------------------
	
	//------------------------------------ Get RTC alarme state ------------------------------------
	int SensorRTC::getRTCAlarm()
	{
		return alarme;	
	}
	//----------------------------------------------------------------------------------------------





	




