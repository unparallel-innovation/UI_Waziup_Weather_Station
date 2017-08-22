#ifndef WEATHER_RECORD
#define WEATHER_RECORD

#include "Arduino.h"


class WeatherRecord
{
	public:
	//Public Functions
	WeatherRecord();
	void setTemperature(float value);			// Set Weather Record Temperature (ºC)
	float getTemperature();						// Return Weather Record Temperature (ºC)
	void setHumidity(float value);				// Set Weather Record Humidity (%)
	float getHumidity();						// Return Weather Record Humidity (%)
	void setPressure(float value);				// Set Weather Record Pressure (kPa)
	float getPressure();						// Return Weather Record Pressure (kPa)
	void setWindSpeed(float value);				// Set Weather Record Wind Speed (kPh)
	float getWindSpeed();						// Return Weather Record Wind Speed (kPh)
	void setWindDirection(float value);			// Set Weather Record Wind Direction (º)
	float getWindDirection();					// Return Weather Record Wind Direction (º)
	void setWindGust(float value);				// Set Weather Record Wind Gust (kPh)
	float getWindGust();						// Return Weather Record Wind Gust (kPh)
	void setAmountRain(float value);			// Set Weather Record Amount of Rain (mm)
	float getAmountRain();						// Return Weather Record Amount of Rain (mm)
	void setBatteryVoltage(float value);		// Set Weather Record Battery Voltage (V)
	float getBatteryVoltage();					// Return Weather Record Battery Voltage (V)
	//
	void setBatteryINA(float value);		// Set Weather Record Battery INA (V)
	float getBatteryINA();					// Return Weather Record Battery INA (V)
	//
	void setMaxCurrent(float value);			// Set Weather Record Max Current (mA)
	float getMaxCurrent();						// Return Weather Record Max Current (mA)
	void setMinCurrent(float value);			// Set Weather Record Min Current (mA)
	float getMinCurrent();						// Return Weather Record Min Current (mA)
	void setRainClicks(unsigned long value);	// Set Weather Record Rain Clicks (nº of interrupts)
	unsigned long getRainClicks();				// Return Weather Record Rain Clicks (nº of interrupts)
	void setWindClicks(unsigned long value);	// Set Weather Record Wind Clicks (nº of interrupts)
	unsigned long getWindClicks();				// Return Weather Record Rain Clicks (nº of interrupts)

	//Public Variables

	private:
	//Private Functions

	//Private Variables
	float temperature;
	float humidity;
	float pressure;
	float windDirection;
	float windGust;
	float windSpeed;
	float amountRain;
	float battery;
	//
	float batteryINA;
	//
	float maxCurrent;
	float minCurrent;
	unsigned long rainClicks;
	unsigned long windClicks;
};

#endif
