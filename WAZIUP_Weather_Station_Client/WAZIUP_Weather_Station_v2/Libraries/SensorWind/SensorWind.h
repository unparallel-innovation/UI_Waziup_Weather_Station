#ifndef SENSOR_WIND
#define SENSOR_WIND

#include <Arduino.h>


class SensorWind
{
	public:
	//Public Functions
	SensorWind(int interrupt_pin, double calibration, const uint8_t direction_pin);
	float getWindSpeed(long minutes);													// Return the Wind Speed (kPh)
	int incrementWindIndice();															// Incremente vector wind indice
	int getWindIndice();																// Return the Wind indice
	float getWindDirection(HardwareSerial* DefaultSerial);								// Return Wind Direction (º)
	float getWindGust();																// Return Wind Gust (kPh)
	long getWindClicks();																// Return the nº of Wind interrupts
	long getGustInterval();																// Return the minimum time between 2 wind interrutps (ms)
	void attachWindInterrupt();															// Attach the Wind interrupt
	void clearWind();										// Clear the Wind variables (FINAL VERSION: REMOVE SERIAL PRINTS)

	void clearWindControl();															// Reset WInd control variable  (FINAL VERSION: TO BE DELETED)
	int getWindControl();																// Return Wind control variable  (FINAL VERSION: TO BE DELETED)	


	



	//Public Variables

	private:
	//Private Functions
	void ISR_WIND();								// Wind Interrupt Routine
	static void isr0();								// Auxiliar Wind Interrupt ISR
	static SensorWind * SensorWind_instance;		// Auxiliar instance to Wind Interrupt ISR 

    //Private Variables
    double w_calibration; 					// Wind Direction calibration value
	uint8_t w_dir_pin;						// Wind Direction pin
	int w_int_pin;							// Wind interrupt pin
	unsigned long windPos[16];				// Wind indice vector
	volatile unsigned long wind;			// Wind interrupt counter
	volatile unsigned long lastWindIRQ;		// Last Wind check time (ms)
	volatile unsigned long t_min;			// Minimum time between 2 wind interrutps (ms)
	volatile unsigned long t_previous;		// Auxiliar var to t_min 
	volatile unsigned long t_interval;		// Auxiliar var to t_min 
	
	volatile unsigned int control;			// (FINAL VERSION: TO BE DELETED)
};

#endif