#ifndef SENSOR_RAIN
#define SENSOR_RAIN

#include <Arduino.h>


class SensorRain
{
	public:
	//Public Functions
	SensorRain(int interrupt_pin);
	float getRain();					// Return the Amount of Rain (mm)
	long getRainClicks();				// Return the nº of Rain interrupts
	void attachRainInterrupt();			// Attach the Rain interrupt
	void clearRain();					// Clear the Rain variables

	void clearRainControl();			// Reset Rain control variable  (FINAL VERSION: TO BE DELETED)
	int getRainControl();				// Return Rain control variable  (FINAL VERSION: TO BE DELETED)				
	


	//Public Variables

	private:
	//Private Functions
	void ISR_RAIN();							// Rain Interrupt Routine
	static void isr1();							// Auxiliar Rain Interrupt ISR
	static SensorRain * SensorRain_instance;	// Auxiliar instance to Rain Interrupt ISR 

	//Private Variables
	int r_int_pin;							// Rain interrupt pin
	volatile unsigned long rain;			// Rain interrupt counter
	volatile unsigned long lastRainIRQ;		// Last Rain check time (ms)	
	
	volatile unsigned int control;			// (FINAL VERSION: TO BE DELETED)	
};

#endif