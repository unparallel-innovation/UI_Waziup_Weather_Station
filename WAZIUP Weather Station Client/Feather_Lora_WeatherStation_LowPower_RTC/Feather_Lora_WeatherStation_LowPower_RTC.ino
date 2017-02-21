/*
    temperature sensor on analog 8 to test the LoRa gateway

    Copyright (C) 2016 Congduc Pham, University of Pau, France

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the program.  If not, see <http://www.gnu.org/licenses/>.

 *****************************************************************************
   last update: Nov. 16th by C. Pham
*/

// ----------------------------------------------------------------------------
// LIBRARYS FOR RTC
// ----------------------------------------------------------------------------
#include <RTClib.h>  // a basic DS1307 library that returns time as a time_t

RTC_DS3231 rtc;
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// LIBRARYS FOR WEATHER STATION
// ----------------------------------------------------------------------------
#include <Wire.h>               // I2C needed for sensors
#include "WeatherConfig.h"      // Includes the "SparkFunMPL3115A2.h" & 
                                // "SparkFunHTU21D.h" libraries

WeatherConfig myConfigWeather;  // For Pressure sensor & Humidity sensor 
                                // respectively
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// CONFIGURING LoRa DEVICE
// ----------------------------------------------------------------------------
// Library for LoRa
#include <SPI.h>
#include <featherLora.h>
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Radio Regulation: (Please uncomment only 1 choice)
// ----------------------------------------------------------------------------
#define ETSI_EUROPE_REGULATION
//#define FCC_US_REGULATION
//#define SENEGAL_REGULATION
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Uncomment if your radio is an HopeRF RFM92W, HopeRF RFM95W, 
// Modtronix inAir9B, NiceRF1276 or you known from the circuit diagram that 
// output use the PABOOST line instead of the RFO line
// ----------------------------------------------------------------------------
#define PABOOST
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// OPERATING BAUDRATE
// ----------------------------------------------------------------------------
#define BAUDRATE 38400
//#define SLEEP_TIME 900   
#define SLEEP_TIME 30 
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Frequency Band: (Please uncomment only 1 choice)
// ----------------------------------------------------------------------------
#define BAND868
//#define BAND900
//#define BAND433
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Select Radio Regulation
// ----------------------------------------------------------------------------
#ifdef ETSI_EUROPE_REGULATION
#define MAX_DBM 14
#elif defined SENEGAL_REGULATION
#define MAX_DBM 10
#endif

#ifdef BAND868
#ifdef SENEGAL_REGULATION
//const uint32_t DEFAULT_CHANNEL = CH_04_868;
#define DEFAULT_CHANNEL CH_04_868
#else
//const uint32_t DEFAULT_CHANNEL = CH_10_868;
#define DEFAULT_CHANNEL CH_10_868
#endif
#elif defined BAND900
//const uint32_t DEFAULT_CHANNEL = CH_05_900;
#define DEFAULT_CHANNEL CH_05_900
#elif defined BAND433
//const uint32_t DEFAULT_CHANNEL = CH_00_433;
#define DEFAULT_CHANNEL CH_00_433
#endif
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// COMMENT OR UNCOMMENT TO CHANGE FEATURES.
// ONLY IF YOU KNOW WHAT YOU ARE DOING!!! OTHERWISE LEAVE AS IT IS
// ----------------------------------------------------------------------------
#define WITH_EEPROM
#define WITH_APPKEY
#define NEW_DATA_FIELD
#define LOW_POWER
#define WITH_ACK
//#define LEDS
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// LoRa Mode (0 to Broadcast, 1 to Gateway) & Node Address (Between 2 and 255)
// ----------------------------------------------------------------------------
#define LORAMODE  1
#define DEFAULT_DEST_ADDR 1
#define node_addr 8
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// HARDWARE PIN DEFENITION:
// CHANGE HERE THE READ PIN AND THE POWER PIN FOR THE SENSOR's
// ----------------------------------------------------------------------------
// Digital I/O pins
#define WSPEED 1
#define RAIN 0
#define STAT_BLUE 12
#define STAT_GREEN 13
#define WS_STATE A4   //WS_STATE previously was pin digital 10 
// Analog I/O pins
#define REFERENCE_3V3 A3
#define LIGHT A1
#define BATT A2
#define WDIR A0
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Change here the AppKey, 
// But if GW checks for AppKey, must be in the AppKey list maintained by GW.
// ----------------------------------------------------------------------------
#ifdef WITH_APPKEY
uint8_t my_appKey[4] = {5, 6, 7, 8};
#endif
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// UNCOMMENT TO PRINTS VIA SERIAL MONITOR
// ----------------------------------------------------------------------------
//#define PRINT
//#define DELAY
//#define PRINT_DATA
//#define PRINTLN                   Serial.println(F(""))
//#define PRINT_STR(fmt,param)      Serial.print(param)
//#define PRINT_CSTSTR(fmt,param)   Serial.print(F(param))
//#define PRINT_VALUE(fmt,param)    Serial.print(param)
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// WITH_ACK
// ----------------------------------------------------------------------------
#ifdef WITH_ACK
#define NB_RETRIES 2
#endif
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// LOW_POWER
// You need the LowPower library from RocketScream:
// https://github.com/rocketscream/Low-Power
// ----------------------------------------------------------------------------
#ifdef LOW_POWER
#include "LowPower.h"
#endif
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// WITH_EEPROM
// ----------------------------------------------------------------------------
#ifdef WITH_EEPROM
#include <EEPROM.h>
struct sx1272config {
  uint8_t flag1;
  uint8_t flag2;
  uint8_t seq;
  // can add other fields such as LoRa mode,...
}; sx1272config my_sx1272config;
#endif
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// GLOBAL VARIABLES
// ----------------------------------------------------------------------------
// volatiles are subject to modification by IRQs
float lastGust = 0;
volatile byte windClicks = 0;
volatile byte rainClicks = 0;
volatile float windSpeed = 0;
volatile unsigned int lastWindCheck = 0;  

uint8_t message[100];
char auxBuf[20];
// ----------------------------------------------------------------------------



////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// SETUP(); //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(STAT_BLUE, OUTPUT);     // Status LED Blue
  pinMode(STAT_GREEN, OUTPUT);    // Status LED Green
  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);
  pinMode(WSPEED, INPUT_PULLUP);  // Input from wind meters windspeed sensor
  pinMode(RAIN, INPUT_PULLUP);    // Input from wind meters rain gauge sensor
  pinMode(WS_STATE, OUTPUT);

  #ifdef LOW_POWER
    #ifdef LEDS
      digitalWrite(STAT_GREEN, HIGH);
    #endif
    digitalWrite(WS_STATE, HIGH); 
  #endif

  Serial.begin(BAUDRATE);

  Wire.begin();
  
  // ----- Get Date & Time from system and send to RTC -------
  rtc.begin();    //begin the RTC
  
  // Uncomment to adjust the date & time ONLY in the first compiler run
  //rtc.adjust(DateTime(__DATE__, __TIME__)); // Get the Date and Time the compiler was run

  lastWindCheck = rtc.now().unixtime();

  
  // Power ON the Lora module
  sx1272.ON();
  //LowPower.idle(SLEEP_FOREVER, ADC_OFF, TIMER4_OFF, TIMER3_OFF, TIMER1_OFF, TIMER0_ON, SPI_OFF, USART1_OFF, TWI_OFF, USB_ON);

  #ifdef WITH_EEPROM
    // Get config from EEPROM
    EEPROM.get(0, my_sx1272config);
    
    // Found a valid config?
    if (my_sx1272config.flag1 == 0x12 && my_sx1272config.flag2 == 0x34)
    {
      #ifdef PRINT;
        Serial.println(F("Get back previous sx1272 config"));
        PRINTLN;
      #endif
  
      // Set sequence number for SX1272 library
      sx1272._packetNumber = my_sx1272config.seq;
      
      #ifdef PRINT;
        Serial.print(F("Using packet sequence number of "));
        Serial.print(sx1272._packetNumber);
        PRINTLN;
      #endif
    }
    else
    {
      // Otherwise, write config and start over
      my_sx1272config.flag1 = 0x12;
      my_sx1272config.flag2 = 0x34;
      my_sx1272config.seq = sx1272._packetNumber;
    }
  #endif

  // Set transmission mode
  int statusMode = sx1272.setMode(LORAMODE);

  // Enable carrier sense
  sx1272._enableCarrierSense = true;

  #ifdef LOW_POWER
    // TODO: with low power, when setting the radio module in sleep mode
    // there seem to be some issue with RSSI reading
    sx1272._RSSIonSend = false;
  #endif


  // Select amplifier line; PABOOST or RFO
  #ifdef PABOOST
    sx1272._needPABOOST = true;
  #endif

  // Select frequency channel
  int statusChannel = sx1272.setChannel(DEFAULT_CHANNEL);
  // Select setting output power
  int statusPower = sx1272.setPowerDBM((uint8_t)MAX_DBM);
  // Set the node address
  int statusNodeAdd = sx1272.setNodeAddress(node_addr);


  // Print the results
  #ifdef PRINT
    Serial.print(F("Setting Mode: state "));
    Serial.print(statusMode);
    PRINTLN;
  
    Serial.print(F("Setting Channel: state "));
    Serial.print(statusChannel);
    PRINTLN;
  
    Serial.print(F("Setting Power: state "));
    Serial.print(statusPower);
    PRINTLN;
  
    // Set the node address and print the result
    Serial.print(F("Setting node addr: state "));
    Serial.print(statusNodeAdd);
    PRINTLN;
  
    // Print a success message
    Serial.println(F("SX1272 successfully configured"));
  
    Serial.println(F("Weather Shield Config"));
  #endif


  // Run Weather Shield calibration
  myConfigWeather.calibration();

  // Attach external interrupt pins to IRQ functions
  attachInterrupt(2, rainIRQ, FALLING);              // FALLING: when the pin goes from high to low
  attachInterrupt(3, wspeedIRQ, FALLING);

  // Turn on interrupts
  interrupts();

  // Weather Shield it's ONLINE
  #ifdef PRINT
    Serial.println(F("Weather Shield online!"));
  #endif

  #ifdef LOW_POWER
    #ifdef LEDS
      digitalWrite(STAT_GREEN, LOW);
    #endif
    digitalWrite(WS_STATE, LOW);
  #endif
}



//----------------------------------------------------------------------------------------------------------------
// INTERRUP ROUTINES:
// Called by the hardware interrupts, not by the main code
//----------------------------------------------------------------------------------------------------------------

// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
void rainIRQ()
{
  // To ignore switch-bounce glitches less than 10mS after initial edge (added filter in hardware)
    rainClicks++;
}


// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
void wspeedIRQ()
{
  // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes (added filter in hardware)
    windClicks++; //There is 1.492MPH for each click per second.
}


//----------------------------------------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// get_wind_speed(); get_gust(); & get_rain(); //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns the wind speed
float get_wind_speed()
{
  float deltaTime = rtc.now().unixtime() - lastWindCheck; //750ms

  //deltaTime /= 1000.0; //Covert to seconds
 
  windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

  //windSpeed *= 1.492 * 1.61; //4 * 1.492 = 5.968MPH    1 milha = 1.61 KPH
  windSpeed *= 2.4; 

  //deltaTime = 0;
  
  lastWindCheck = rtc.now().unixtime();

  return (windSpeed);
}


// Returns the last gust
float get_gust(float wind_speed)
{
    if (wind_speed > lastGust)
      return wind_speed;
    
    return (lastGust);
}


// Returns the rain
float get_rain()
{
  return (rainClicks*0.2794); // Each interrup represent 0.011" iches of rain 1" to mm => 25.4mm
}


// LowPower fuction. Return the sleep mode status.
int lowPower(int e)
{
  #ifdef DELAY
    delay(10);
  #endif
           
  e = sx1272.setSleepMode();
  
  if (!e) 
  {
    #ifdef PRINT;
      Serial.println(F("Successfully switch LoRa module in sleep mode"));
    #endif
    #ifdef LEDS;
      digitalWrite(STAT_BLUE, LOW);
      digitalWrite(STAT_GREEN, LOW);
    #endif
    digitalWrite(WS_STATE, LOW);
  }
  else
  {
    #ifdef PRINT;
      Serial.println(F("Could not switch LoRa module in sleep mode"));
    #endif
  }
  
  Serial.flush();
  
  // Enter power down state for 8 s with ADC and BOD module disabled
  #ifdef LOW_POWER
    //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    LowPower.idle(SLEEP_8S, ADC_OFF, TIMER4_OFF, TIMER3_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART1_OFF, TWI_OFF, USB_OFF);
    
  #endif

  Serial.flush();
 
  #ifdef DELAY
    delay(50);
  #endif
  
  return e;
}


// Fuction to send the data to gateway
void sendInfo(int pl, int e)
{
      #ifdef WITH_APPKEY
        // Indicate that we have an appkey
        sx1272.setPacketType(PKT_TYPE_DATA | PKT_FLAG_DATA_WAPPKEY);
      #else
        // Just a simple data packet
        sx1272.setPacketType(PKT_TYPE_DATA);
      #endif
                  
        
      // Send message to the gateway and print the result with the app key if this feature is enabled
      #ifdef WITH_ACK
          int n_retry = NB_RETRIES;
      
          do
          {
                  e = sx1272.sendPacketTimeoutACK(DEFAULT_DEST_ADDR, message, pl);
                  
              #ifdef PRINT;
                  if (e == 3)
                  {
                    PRINTLN;
                    Serial.print(F("No ACK"));
                  }
              #endif
            
                  n_retry--;

              #ifdef PRINT;
                  if (n_retry)
                  {
                    PRINTLN;
                    Serial.print(F("Retry"));
                  }
                  else
                  {
                    PRINTLN;
                    Serial.println(F("Abort"));
                  }
              #endif
          } while (e && n_retry);
                  
                  
          if (e == 0)
          {
                  #ifdef LEDS
                    digitalWrite(STAT_BLUE, LOW);
                    digitalWrite(STAT_GREEN, HIGH);
                  #endif
                  
                  #ifdef DELAY
                    delay(3000);
                  #endif
          }
       #else
            e = sx1272.sendPacketTimeout(DEFAULT_DEST_ADDR, message, pl);
       #endif
}




////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////   LOOP();   /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop(void)
{
      unsigned long startSend;
      unsigned long endSend;
      uint8_t app_key_offset = 0;
      int e;
      unsigned long before_sleep=0;
      unsigned long after_sleep=0;
    
      
      DateTime now = rtc.now();
      
      
      digitalWrite(WS_STATE, HIGH);

      #ifdef LEDS;
        digitalWrite(STAT_BLUE, HIGH);
      #endif

      // GET VALUES
        //Humidity
        float humidity = myConfigWeather.get_humidity();
        //Temperature (Cº)
        float tempc = myConfigWeather.get_temperature();
        //Check Pressure Sensor
        float pressure = myConfigWeather.get_pressure();
        //Check light sensor
        float light_lvl = myConfigWeather.get_light_level(analogRead(REFERENCE_3V3), analogRead(LIGHT));
        //Check batt level
        float batt_lvl = myConfigWeather.get_battery_level(analogRead(REFERENCE_3V3), analogRead(BATT));
        //Check wind direction
        float windDirection = myConfigWeather.get_wind_direction(WDIR);   // WDIR: is analog pin "A0"
        //Check wind speed
        float getWindSpeed = get_wind_speed();
        //Get Rain
        float getRain = get_rain();


        // WITH_APPKEY
        #ifdef WITH_APPKEY
            app_key_offset = sizeof(my_appKey);
        
            // Set the app key in the payload
            memcpy(message, my_appKey, app_key_offset);
        #endif

        uint8_t r_size;

        // then use app_key_offset to skip the app key


        // Print the Results
        #ifdef PRINT_DATA
          PRINTLN;
          Serial.print(F("Temperature = "));
          Serial.print(tempc, 2);
          Serial.print(F("C,"));
        
          Serial.print(F(" Humidity = "));
          Serial.print(humidity, 0);
          Serial.print(F("%,"));
        
          Serial.print(F(" Pressure = "));
          Serial.print(pressure, 0);
          Serial.print(F("Pa,"));
        
          Serial.print(F(" Light_lvl = "));
          Serial.print(light_lvl);
          Serial.print(F("V,"));
        
          Serial.print(F(" VinPin = "));
          Serial.print(batt_lvl);
          Serial.print(F("V"));
        
          Serial.print(F(" Wind Direction = "));
          Serial.print(windDirection);
          Serial.print("º");
        
          Serial.print(F(" Gust = "));
          Serial.print(lastGust);
        
          Serial.print(F(" Wind Speed = "));
          Serial.print(getWindSpeed);
        
          Serial.print(F(" Rain = "));
          Serial.print(getRain);
          PRINTLN;
        #endif
      

        // NEW_DATA_FIELD
        #ifdef NEW_DATA_FIELD
           
           r_size = sprintf((char*)message + app_key_offset, "\\!TC/%s", dtostrf(tempc,2,2,auxBuf));
           r_size += sprintf((char*)message + app_key_offset + r_size, "/HU/%s", dtostrf(humidity,2,0,auxBuf));
           r_size += sprintf((char*)message + app_key_offset + r_size, "/LU/%s", dtostrf(light_lvl,2,2,auxBuf));
           r_size += sprintf((char*)message + app_key_offset + r_size, "/DO/%s", dtostrf(pressure,2,2,auxBuf));
           r_size += sprintf((char*)message + app_key_offset + r_size, "/AZO/%s", dtostrf(batt_lvl,2,2,auxBuf));
           r_size += sprintf((char*)message + app_key_offset + r_size, "/WD/%s", dtostrf(windDirection,2,0,auxBuf));
           r_size += sprintf((char*)message + app_key_offset + r_size, "/WC/%s", dtostrf(getWindSpeed,1,0,auxBuf));
           r_size += sprintf((char*)message + app_key_offset + r_size, "/RC/%s", dtostrf(getRain,2,2,auxBuf));
           
           //r_size = sprintf((char*)message + app_key_offset, "\\!TC/%s/HU/%s/LU/%s/DO/%s/AZO/%s/WD/%s/WC/%s/RC/%s", dtostrf(tempc,3,2,auxBuf), dtostrf(humidity,3,0,auxBuf), dtostrf(light_lvl,3,2,auxBuf), dtostrf(pressure,3,2,auxBuf) , dtostrf(batt_lvl,3,2,auxBuf), dtostrf(windDirection,3,0,auxBuf), dtostrf(getWindSpeed,3,0,auxBuf), dtostrf(lastRain,3,3,auxBuf) );

          #else
           r_size = sprintf((char*)message + app_key_offset, "\\!#%d#%d", field_index, (int)temp);
        #endif


       //r_size = sprintf((char*)message + app_key_offset, "\\!TC/%s/HU/%s/LU/%s/DO/%s/AZO/%s/WD/%s/WC/%s/RC/%s", char(tempc), char(humidity), char(light_lvl), char(pressure) , char(batt_lvl), char(windDirection), char(getWindSpeed), char(lastRain) );
       //r_size = sprintf((char*)message + app_key_offset, "\\!TC/%s/HU/%s/LU/%s/DO/%s/AZO/%s/WD/%s/WC/%s/RC/%s", char(tempc), char(humidity), char(light_lvl), char(pressure) , char(batt_lvl), char(windDirection), char(getWindSpeed), char(lastRain) );

        /*
          //  "\\!TC/%s/HU/%s/LU/%s/DO/%s/AZO/%s/WD/%s/WC/%s/RC/%s""
          String valores;
        
          valores += "\\!TC/"+String(tempc,2)+"/HU/"+String(humidity,0)+"/LU/"+String(light_lvl,2)+"/DO/"+String((pressure),2)+"/AZO/"+String(batt_lvl,2)+"/WD/"+String(windDirection,1)+"/WC/"+String(getWindSpeed,1)+"/RC/"+String(getRain,1);
        
          Serial.print(valores);
          Serial.println("");
        */

                             
        #ifdef PRINT
            #ifdef DELAY
              delay(10);
            #endif
            PRINTLN;
            Serial.print(F("Sending "));
            PRINT_STR("%s",(char*)(message+app_key_offset));   
            PRINTLN;
            Serial.print(F("Real payload size is "));
            Serial.print(r_size);
            PRINTLN;
            #ifdef DELAY
              delay(10);
            #endif
        #endif
        
                  
                  
        int pl = r_size + app_key_offset;
        sx1272.CarrierSense();


        //--- STAR TO SEND -------------------
        #ifdef PRINT;
            Serial.println(F(" SENDING! "));
        #endif

        now = rtc.now();
        startSend = now.unixtime();
        sendInfo(pl, e);
        now = rtc.now();
        endSend = now.unixtime();
        //--- STOP TO SEND -------------------
        

        #ifdef WITH_EEPROM
            // Save packet number for next packet in case of reboot
            my_sx1272config.seq = sx1272._packetNumber;
            EEPROM.put(0, my_sx1272config);
        #endif
        

       #ifdef PRINT
          PRINTLN;
          Serial.print(F("LoRa pkt size "));
          Serial.print(pl);
          PRINTLN;
      
          Serial.print(F("LoRa pkt seq "));
          Serial.print(sx1272.packet_sent.packnum);
          PRINTLN;
      
          Serial.print(F("LoRa Sent in "));
          Serial.print((endSend - startSend));
          PRINTLN;
      
          Serial.print(F("LoRa Sent w/CAD in "));
          Serial.print((endSend - sx1272._startDoCad));
          PRINTLN;
      
          Serial.print(F("Packet sent, state "));
          Serial.print(e);
          PRINTLN;
      #endif

        lastGust=0;
        windClicks = 0;
        windSpeed = 0;
        rainClicks = 0;
        lastWindCheck = 0;



      // LowPower Mode. Put in Sleep mode cicle (8s) utill the "SLEEP_TIME"
      now = rtc.now();
      before_sleep = now.unixtime();     // now.unixtime() return the seconds since 1970 till now

      // APAGARRRRRRRRRR ---------------------------
      #ifdef PRINT_DATA
        Serial.println();
        Serial.print("before_sleep ");
        Serial.println(before_sleep);
      #endif
    
      #ifdef PRINT_DATA
        Serial.println(F("Switch to LowPower Mode!"));
      #endif

      do
      {
        e = lowPower(e);

        lastGust = get_gust(get_wind_speed());
        
        now = rtc.now();
        after_sleep = now.unixtime();

        
        #ifdef PRINT_DATA
          Serial.print(F("."));
          Serial.println(after_sleep-before_sleep);
        #endif

                     
      }while ((after_sleep-before_sleep) < (SLEEP_TIME));

      // APAGARRRRRRRRRR ---------------------------
      #ifdef PRINT_DATA
        delay(10);
        Serial.println(F(""));
        Serial.print(F("Slept Time: "));
        Serial.print(after_sleep-before_sleep);
        Serial.println(F(" seconds."));
      #endif
        
      before_sleep=0;
      after_sleep=0;
        
  #ifdef DELAY
    delay(50);
  #endif
  }


