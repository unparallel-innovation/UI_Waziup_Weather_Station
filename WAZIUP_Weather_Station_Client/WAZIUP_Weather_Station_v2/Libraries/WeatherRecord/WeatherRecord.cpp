#include "WeatherRecord.h"


WeatherRecord::WeatherRecord(){
}
	//----------------------------------------	Temperature ----------------------------------------
	void WeatherRecord::setTemperature(float value){
		temperature=value;
	}

	float WeatherRecord::getTemperature()
	{
		return temperature;
	}
	//----------------------------------------------------------------------------------------------

	//-----------------------------------------	Humidity -------------------------------------------
	void WeatherRecord::setHumidity(float value){
		humidity=value;
	}

	float WeatherRecord::getHumidity(){
		return humidity;
	}
	//----------------------------------------------------------------------------------------------

	//-----------------------------------------	Pressure -------------------------------------------
	void WeatherRecord::setPressure(float value){
		pressure=value;
	}

	float WeatherRecord::getPressure(){
		return pressure;
	}
	//----------------------------------------------------------------------------------------------

	//------------------------------------------- Wind Speed  --------------------------------------
	void WeatherRecord::setWindSpeed(float value){
		windSpeed=value;
	}

	float WeatherRecord::getWindSpeed(){
		return windSpeed;
	}
	//----------------------------------------------------------------------------------------------

	// ------------------------------------------- Wind Direction  ---------------------------------
	void WeatherRecord::setWindDirection(float value){
		windDirection=value;
	}

	float WeatherRecord::getWindDirection(){
		return windDirection;
	}
	//----------------------------------------------------------------------------------------------

	// ------------------------------------------- Wind Gust  --------------------------------------
	void WeatherRecord::setWindGust(float value){
		windGust=value;
	}

	float WeatherRecord::getWindGust(){
		return windGust;
	}
	//----------------------------------------------------------------------------------------------

	// ------------------------------------------- Amount Rain  ------------------------------------
	void WeatherRecord::setAmountRain(float value){
		amountRain=value;
	}

	float WeatherRecord::getAmountRain(){
		return amountRain;
	}
	//----------------------------------------------------------------------------------------------

	// ------------------------------------------- Battery Voltage  --------------------------------
	void WeatherRecord::setBatteryVoltage(float value){
		battery=value;
	}

	float WeatherRecord::getBatteryVoltage(){
		return battery;
	}

	//
	void WeatherRecord::setBatteryINA(float value){
		batteryINA=value;
	}

	float WeatherRecord::getBatteryINA(){
		return batteryINA;
	}
	//
	//----------------------------------------------------------------------------------------------

	// ------------------------------------------- Max Current  ------------------------------------
	void WeatherRecord::setMaxCurrent(float value){
		maxCurrent=value;
	}

	float WeatherRecord::getMaxCurrent(){
		return maxCurrent;
	}
	//----------------------------------------------------------------------------------------------

	// ------------------------------------------- Min Current  ------------------------------------
	void WeatherRecord::setMinCurrent(float value){
		minCurrent=value;
	}
	float WeatherRecord::getMinCurrent(){
		return minCurrent;
	}

	// ------------------------------------------- Rain Clicks  ------------------------------------
	void WeatherRecord::setRainClicks(unsigned long value){
		rainClicks=value;
	}

	unsigned long WeatherRecord::getRainClicks(){
		return rainClicks;
	}
	//----------------------------------------------------------------------------------------------

	// ------------------------------------------- Wind Clicks  ------------------------------------
	void WeatherRecord::setWindClicks(unsigned long value){
		windClicks=value;
	}

	unsigned long WeatherRecord::getWindClicks(){
		return windClicks;
	}
	//----------------------------------------------------------------------------------------------
