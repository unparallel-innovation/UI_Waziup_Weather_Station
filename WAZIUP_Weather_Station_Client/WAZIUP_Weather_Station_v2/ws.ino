
#include <WeatherStation.h>
WeatherStation ws(&Serial1);

<<<<<<< HEAD
#define PRINT

#include <RTCInt.h>
RTCInt rtc;             // Create Instance for RTC

#include <RH_RF95.h>
RH_RF95 rf95(RFM95_CS, RFM95_INT);    // Singleton instance of the radio driver

#include "SparkFunMPL3115A2.h"    //Pressure sensor (use only the WAZIUP version)
MPL3115A2 MPL3115A2;    //Create Instance of HTU21D or SI7021 sensor and MPL3115A2 sensor

#include "SparkFun_Si7021_Breakout_Library.h"   //Humidity sensor (use only the WAZIUP version)
Weather SI7021;   //Create Instance of HTU21D or SI7021 sensor and MPL3115A2 sensor

//-------------- Global Variables -------------------------
volatile long rain = 0;
volatile long wind = 0;
volatile long lastRainIRQ = 0;
volatile long lastWindIRQ = 0;
//volatile unsigned long minutes = MINUTES;
volatile unsigned long minutes = 5;
unsigned long oneMinute = 3000;
volatile unsigned long getValues=0;

float temperatures[MINUTES];
float humidities[MINUTES];
float pressures[MINUTES];

unsigned long windPos[WPOS];
unsigned long pos=0;
unsigned long count=0;

unsigned long pos_aux=0;


float temperature = 0;
float humidity = 0;
float pressure = 0;
float windDirection = 0;
float windSpeed = 0;
float amountRain = 0;




 
void setup()
{
  Serial.begin(BAUDRATE);

  Wire.begin();        // Initiate the Wire library and join the I2C bus

//  configLoRa();   // Configure LoRa

  // for testing
  delay(1000);
  Serial.println("---inicio---");
  

  if( beginSensors() == 1)  // Initiate the sensors
  {
    #ifdef PRINT        
      Serial.println(F("Weather Station Sensors Online!"));
      Serial.println();
    #endif
  }
  else
    {
    #ifdef PRINT        
      Serial.println(F("Weather Station Sensors Erro!"));
      Serial.println();
    #endif
  }

//  lastRainIRQ = millis();
//  lastWindIRQ = millis();

    // Input from wind meters, windspeed sensor and rain gauge sensor
  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor

  attachInterrupt(digitalPinToInterrupt(WSPEED), ISR_WIND, FALLING);
  attachInterrupt(digitalPinToInterrupt(RAIN), ISR_RAIN, FALLING);

  interrupts(); // Turn on interrupts

  // for testing
  delay(1000);
  Serial.println("---inicio---");
=======
void setup()
{
  ws.init();
  //Serial1.println(F("(start)"));
>>>>>>> WithRTC
}

void loop()
{
<<<<<<< HEAD
  unsigned long before_time=0;
  unsigned long after_time=0;
  unsigned long current_time=0;
 
  if (minutes==5)
  {
      // Calcular todos
      getWeatherAverage();   
      
      // Enviar Valores
//      sendMensage();

      // Limpar Vectores e Contadores
      clearValues();
      
      // Para calcular valores para minuto zero
      getValues=1;
      minutes=0;
  }

  if (getValues==1)
  {
    getWeatherValues();
    
    // for testing
    Serial.print(" Minute: ");
    Serial.println(minutes);
//    Serial.print(" temperature: ");
//    Serial.print(temperatures[minutes]);
//    Serial.print(" humidity: ");
//    Serial.print(humidities[minutes]);
//    Serial.print(" pressure: ");
//    Serial.print(pressures[minutes]);
//    Serial.print(" Rain: ");
//    Serial.print(rain);
//    Serial.print(" windPOS: ");
//    Serial.print(get_wind_pos(analogRead(WDIR)));
//    Serial.print(" wind Direction: ");
//    Serial.print(get_wind_direction(get_wind_pos(analogRead(WDIR))));
//    Serial.print(" getWind: ");
//    Serial.println(wind);
  }

  // Get Time before cicle for count 1 minute.
  before_time = millis();

   Serial.print("1) Wind: ");
        Serial.print(wind);
        Serial.print(" Rain: ");
        Serial.print(rain);
  
  do
  {
      // Time for count 1 minute.
      after_time = millis();
      current_time=(after_time-before_time);

//      // for testing
//      if(control==1)
//      {
//        control=0;
//        
//        // for testing
//        Serial.println("interrup gerado");
//        Serial.print("Wind: ");
//        Serial.print(wind);
//        Serial.print(" Rain: ");
//        Serial.print(rain);
//        Serial.print(" getRain: ");
//        Serial.print(get_rain(rain));
//        Serial.print(" getWind: ");
//        Serial.print(get_wind_speed(wind, minutes));
//        Serial.print(" windPOS: ");
//        Serial.print(get_wind_pos(analogRead(WDIR)));
//        Serial.print(" wind Direction: ");
//        Serial.println(get_wind_direction(get_wind_pos(analogRead(WDIR))));
//      }


  }while((current_time) < (oneMinute));

  Serial.print(" 3) Wind: ");
        Serial.print(wind);
        Serial.print(" Rain: ");
        Serial.print(rain);
  
  // Increment minutes
  minutes++;

  Serial.print(" 4) Wind: ");
        Serial.print(wind);
        Serial.print(" Rain: ");
        Serial.print(rain);

  // To acquire the respective minute weather measurements
  getValues=1;

  Serial.print(" 5) Wind: ");
        Serial.print(wind);
        Serial.print(" Rain: ");
        Serial.println(rain);

  delay(1000);
}


//----------------------- To begin the mesurement sensors --------------------------
int beginSensors()
{

  int i;
  
  SPI.begin();

  if ( ((SI7021.begin())!= 0) && ((MPL3115A2.begin())!= 0) )
    i=1;  // Return 1 if connect with success
  else
    i=0;  // Return 0 if connect without success
  
  MPL3115A2.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  MPL3115A2.setOversampleRate(7); // Set Oversample to the recommended 128
  MPL3115A2.enableEventFlags(); // Enable all three pressure and temp event flags
  MPL3115A2.readTemp();
  MPL3115A2.readPressure();

  return i;    
}
//----------------------------------------------------------------------------------


//------------------------- To configure the LoRa module ---------------------------
void configLoRa()
{
  rf95.init();
  rf95.setFrequency(RF95_FREQ);
  delay(10);  
}
//----------------------------------------------------------------------------------


//------------------------ Amount of Rain Timer interrupt ---------------------------
void ISR_RAIN()          
{
  if (millis() - lastRainIRQ > 10) // Ignore switch-bounce glitches less than 10mS after initial edge
  {
    lastRainIRQ = millis(); // Set up for next event
    rain++; //Each dump is 0.011" of water  
  }
}
//----------------------------------------------------------------------------------


//-------------------------- Wind speed Timer interrupt ----------------------------
void ISR_WIND()          
{ 
  if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
  {
    lastWindIRQ = millis();   // Grab the current time
    wind++;   // There is 1.492MPH for each click per second.
  }
}
//----------------------------------------------------------------------------------


//----------------- Get the Weather measurements for each minute ------------------
void getWeatherValues()
{
  long wind_aux = wind;
  long rain_aux = rain;
  
//  detachInterrupt(digitalPinToInterrupt(WSPEED));
//  detachInterrupt(digitalPinToInterrupt(RAIN));
//  delay(100);
  
  temperatures[minutes] = SI7021.getTemp();   // or MPL3115A2.readTemp(); // Temperature (Cº)
  humidities[minutes] = SI7021.getRH();      // Humidity (%)
  pressures[minutes] = MPL3115A2.readPressure();   // Pressure (kPa)
  windPos[get_wind_pos(analogRead(WDIR))]++;  // Wind Direction (º)

  getValues=0;

//  attachInterrupt(digitalPinToInterrupt(WSPEED), ISR_WIND, FALLING);
//  attachInterrupt(digitalPinToInterrupt(RAIN), ISR_RAIN, FALLING);

  wind=wind_aux;
  rain=rain_aux;

}
//----------------------------------------------------------------------------------


//----------------- Get the Weather measurements for the Hour ------------------
void getWeatherAverage()
{  
  for (int i=0; i<minutes; i++)
  {
    temperature += temperatures[i];
    humidity += humidities[i];
    pressure += pressures[i];

    // for testing
    Serial.print(" temp: ");
    Serial.print(temperatures[i]);
    Serial.print(" hum: ");
    Serial.print(humidities[i]);
    Serial.print(" press: ");
    Serial.println(pressures[i]);
  }

  for (int i=0; i<WPOS; i++)
  {
    if (windPos[i] > count)
    {
      count = windPos[i];
      pos = i;
    }

    // for testing
    Serial.print(" pos: ");
    Serial.print(i);
    Serial.print(" count: ");
    Serial.print(count);
    Serial.print(" tendencia: ");
    Serial.println(windPos[i]);
  }

  
  temperature = (temperature/minutes);  // Temperature (Cº)
  humidity = (humidity/minutes);  // Humidity (%)
  pressure = (pressure/minutes);  // Pressure (kPa)
  windDirection = get_wind_direction(pos);  // Wind Direction (º)
  windSpeed = get_wind_speed(wind, minutes);   // Wind Speed (kPh) 
  amountRain = get_rain(rain);        // Rain (mm)

      //for testing
      Serial.print(" --- Last ");
      Serial.print(minutes);
      Serial.print(" minutes average ---  ");
      Serial.print(" temperature: ");
      Serial.print(temperature);
      Serial.print(" humidity: ");
      Serial.print(humidity);
      Serial.print(" pressure: ");
      Serial.print(pressure);
      Serial.print(" Wspeed: ");
      Serial.print(windSpeed);
      Serial.print(" Wpos: ");
      Serial.print(pos);
      Serial.print(" Wdirection: ");
      Serial.print(windDirection);
      Serial.print(" AMountRAIN: ");
      Serial.println(amountRain);
}
//----------------------------------------------------------------------------------


//----------------------- Clear interrupts, Couters & Values -----------------------
void clearValues()
{
  temperature = 0;
  humidity = 0;
  pressure = 0;
  windDirection = 0;
  windSpeed = 0;
  amountRain = 0;
  rain=0;
  wind=0;
  
  pos=0;
  pos_aux=0;
  count=0;
  
  minutes=0;

  for (int i=0; i<WPOS; i++)
    windPos[i]=0;

  for (int i=0; i<MINUTES; i++)
  {
    temperatures[i]=0;
    humidities[i]=0;
    pressures[i]=0;
  }
}
//----------------------------------------------------------------------------------


void sendMensage()
{
  uint8_t r_size;
  char auxBuf[20];
  uint8_t data[100]; 

      Serial.flush();
      
      r_size += sprintf((char*)data, "TP;%s;", dtostrf(temperature, 2, 2, auxBuf));
      r_size += sprintf((char*)data + r_size, "HD;%s;", dtostrf(humidity, 2, 2, auxBuf));
      r_size += sprintf((char*)data + r_size, "PA;%s;", dtostrf(pressure, 2, 4, auxBuf));
      r_size += sprintf((char*)data + r_size, "WD;%s;", dtostrf(windDirection, 2, 0, auxBuf));
      r_size += sprintf((char*)data + r_size, "WS;%s;", dtostrf(windSpeed, 3, 2, auxBuf));
//      r_size += sprintf((char*)data + r_size, "WG;%s;", dtostrf(windGust, 2, 2, auxBuf));
      r_size += sprintf((char*)data + r_size, "RA;%s;", dtostrf(amountRain, 2, 2, auxBuf));

      #ifdef PRINT 
        Serial.print("Sending: ");
        Serial.write(data, sizeof(data));
        Serial.println();
      #endif

      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();

      Serial.flush();     
}


=======
  if(ws.task()!=0);
}
>>>>>>> WithRTC
