/*
 *  temperature sensor on analog 8 to test the LoRa gateway
 *
 *  Copyright (C) 2016 Congduc Pham, University of Pau, France
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************
 * last update: Nov. 16th by C. Pham
 */
//library for Lora
#include <SPI.h> 
#include <featherLora.h>

//library for Weather Station
#include <Wire.h> //I2C needed for sensors
#include "WeatherConfig.h"

WeatherConfig myConfigWeather;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// please uncomment only 1 choice
#define ETSI_EUROPE_REGULATION
//#define FCC_US_REGULATION
//#define SENEGAL_REGULATION

// uncomment if your radio is an HopeRF RFM92W, HopeRF RFM95W, Modtronix inAir9B, NiceRF1276
// or you known from the circuit diagram that output use the PABOOST line instead of the RFO line
#define PABOOST

// please uncomment only 1 choice
#define BAND868
//#define BAND900
//#define BAND433

// uncomment to prints via serial
#define PRINT
#define BAUDRATE 38400

#ifdef ETSI_EUROPE_REGULATION
  #define MAX_DBM 14
#elif defined SENEGAL_REGULATION
  #define MAX_DBM 10
#endif

#ifdef BAND868
  #ifdef SENEGAL_REGULATION
     const uint32_t DEFAULT_CHANNEL=CH_04_868;
  #else
     const uint32_t DEFAULT_CHANNEL=CH_10_868;
  #endif
#elif defined BAND900
  const uint32_t DEFAULT_CHANNEL=CH_05_900;
#elif defined BAND433
  const uint32_t DEFAULT_CHANNEL=CH_00_433;
#endif

// COMMENT OR UNCOMMENT TO CHANGE FEATURES. 
// ONLY IF YOU KNOW WHAT YOU ARE DOING!!! OTHERWISE LEAVE AS IT IS
#define WITH_EEPROM
#define WITH_APPKEY
#define FLOAT_SENSOR
#define NEW_DATA_FIELD
#define LOW_POWER
#define LOW_POWER_HIBERNATE
#define WITH_ACK
//////////////////////////////////////////////////////////////////

// CHANGE HERE THE LORA MODE, NODE ADDRESS 
#define LORAMODE  1
#define DEFAULT_DEST_ADDR 1
#define node_addr 8

// CHANGE HERE THE READ PIN AND THE POWER PIN FOR THE SENSOR's
// digital I/O pins
const byte WSPEED = 1;
const byte RAIN = 0;
const byte STAT_BLUE = 12;
const byte STAT_GREEN = 13;

// analog I/O pins
const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;
const byte WDIR = A0;

// CHANGE HERE THE TIME IN MINUTES BETWEEN 2 READING & TRANSMISSION
unsigned int idlePeriodInMin = 0.3;

#ifdef WITH_APPKEY
  // CHANGE HERE THE APPKEY, BUT IF GW CHECKS FOR APPKEY, MUST BE
  // IN THE APPKEY LIST MAINTAINED BY GW.
  uint8_t my_appKey[4]={5, 6, 7, 8};
#endif

#define PRINTLN                   Serial.println("")
#define PRINT_CSTSTR(fmt,param)   Serial.print(F(param))
#define PRINT_STR(fmt,param)      Serial.print(param)
#define PRINT_VALUE(fmt,param)    Serial.print(param)
#define FLUSHOUTPUT               Serial.flush();

#ifdef WITH_ACK
   #define NB_RETRIES 2
#endif

#ifdef LOW_POWER
  #define LOW_POWER_PERIOD 8
  // you need the LowPower library from RocketScream
  // https://github.com/rocketscream/Low-Power
  #include "LowPower.h"
  unsigned int nCycle = idlePeriodInMin*60/LOW_POWER_PERIOD;
#endif

unsigned long lastTransmissionTime=0;
unsigned long delayBeforeTransmit=0;
uint8_t message[100];
int loraMode=LORAMODE;

char tempc_str[20];
char humidity_str[20];
char light_lvl_str[20];
char pressure_str[20];
char batt_lvl_str[20];
char windDirection_str[20];
char getWindSpeed_str[20];
char getRain_str[10];
char getGust_str[10];
byte minutes; //Keeps track of where we are in various arrays of data
volatile float rainHour[60]; //60 floating numbers to keep track of 60 minutes of rain
volatile float rainin = 0; // [rain inches over the past hour)] -- the accumulated rainfall in the past 60 min
volatile float windIn = 0;
volatile float lastGust = 0;

volatile float dailyrainin = 0; // [rain inches so far today in local time]
volatile long lastWindIRQ = 0;
volatile byte windClicks = 0;
volatile byte rainClicks = 0;

float windSpeed = 0;
// volatiles are subject to modification by IRQs
volatile unsigned long raintime, rainlast, raininterval, rain, rTime, gustTime;


long lastWindCheck = 0;
long lastRainCheck = 0;
long lastGustTcheck = 0;
#ifdef WITH_EEPROM
  #include <EEPROM.h>
  struct sx1272config {
    uint8_t flag1;
    uint8_t flag2;
    uint8_t seq;
    // can add other fields such as LoRa mode,...
  };sx1272config my_sx1272config;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  int e;

  pinMode(STAT_BLUE, OUTPUT); //Status LED Blue
  pinMode(STAT_GREEN, OUTPUT); //Status LED Green
  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);
  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor  
  
  #ifdef LOW_POWER
      digitalWrite(STAT_GREEN,HIGH);
  #endif

  Serial.begin(BAUDRATE); 
  
  // Power ON the Lora module 
  sx1272.ON();

  #ifdef WITH_EEPROM
    // get config from EEPROM
    EEPROM.get(0, my_sx1272config);
      // found a valid config?
      if (my_sx1272config.flag1==0x12 && my_sx1272config.flag2==0x34) {
        PRINT_CSTSTR("%s","Get back previous sx1272 config\n");
    
        // set sequence number for SX1272 library
        sx1272._packetNumber=my_sx1272config.seq;
        PRINT_CSTSTR("%s","Using packet sequence number of ");
        PRINT_VALUE("%d", sx1272._packetNumber);
        PRINTLN;
      }
      else {
        // otherwise, write config and start over
        my_sx1272config.flag1=0x12;
        my_sx1272config.flag2=0x34;
        my_sx1272config.seq=sx1272._packetNumber;
      }
  #endif
  
  // Set transmission mode and print the result
  int statusMode = sx1272.setMode(loraMode);
  
  // enable carrier sense
  sx1272._enableCarrierSense=true;
  
  #ifdef LOW_POWER
    // TODO: with low power, when setting the radio module in sleep mode
    // there seem to be some issue with RSSI reading
    sx1272._RSSIonSend=false;
  #endif   
    
  
  // Select amplifier line; PABOOST or RFO
  #ifdef PABOOST
    sx1272._needPABOOST=true;
  #endif  
  
  int statusChannel = sx1272.setChannel(DEFAULT_CHANNEL);
  int statusPower = sx1272.setPowerDBM((uint8_t)MAX_DBM);
  int statusNodeAdd = sx1272.setNodeAddress(node_addr);
  
  #ifdef PRINT
  PRINT_CSTSTR("%s","Setting Mode: state ");
  PRINT_VALUE("%d", statusMode);
  PRINTLN;

  PRINT_CSTSTR("%s","Setting Channel: state ");
  PRINT_VALUE("%d", statusChannel);
  PRINTLN;
  
  PRINT_CSTSTR("%s","Setting Power: state ");
  PRINT_VALUE("%d", statusPower);
  PRINTLN;
  
  // Set the node address and print the result
  PRINT_CSTSTR("%s","Setting node addr: state ");
  PRINT_VALUE("%d", statusNodeAdd);
  PRINTLN;
  
  // Print a success message
  PRINT_CSTSTR("%s","SX1272 successfully configured\n");
  
  Serial.println("Weather Shield Config");
  #endif
  
  myConfigWeather.calibration();

  //attach external interrupt pins to IRQ functions
  attachInterrupt(2, rainIRQ, FALLING);
  attachInterrupt(3, wspeedIRQ, FALLING);

  // turn on interrupts
  interrupts();

  #ifdef PRINT
    Serial.println("Weather Shield online!");
  #endif
 
  delay(1000);
  
  #ifdef LOW_POWER
    digitalWrite(STAT_GREEN,LOW);
  #endif  
}


//Interrupt routines (these are called by the hardware interrupts, not by the main code)
void rainIRQ()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
{
    raintime = millis(); // grab current time
    raininterval = raintime - rainlast; // calculate interval between this and last event
    
    if (raininterval > 10) // ignore switch-bounce glitches less than 10mS after initial edge
    {
        dailyrainin += 0.011; //Each dump is 0.011" of water
        //rainHour[minutes] += 0.011; //Increase this minute's amount of rain

        rainlast = raintime; // set up for next event
        
        rainClicks++;
    }
}
void wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
    if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
    {
        lastWindIRQ = millis(); //Grab the current time
        windClicks++; //There is 1.492MPH for each click per second.
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(void)
{
  long startSend;
  long endSend;
  uint8_t app_key_offset=0;
  int e;
  digitalWrite(STAT_BLUE,HIGH);

  //humidade
  float humidity = myConfigWeather.get_humidity();
  //temperatura em Cº
  float tempc = myConfigWeather.get_temperature();
  //Check Pressure Sensor
  double pressure = myConfigWeather.get_pressure();
  //Check light sensor
  float light_lvl = myConfigWeather.get_light_level(analogRead(REFERENCE_3V3), analogRead(LIGHT));
  //Check batt level
  float batt_lvl = myConfigWeather.get_battery_level(analogRead(REFERENCE_3V3), analogRead(BATT));
  //Check wind direction
  float windDirection = myConfigWeather.get_wind_direction(WDIR);
  //Check wind speed 5min
  float getGust = get_gust();
  //Check wind speed 5min
  float getWindSpeed = get_wind_speed();
  //check rain 5min
  float getRain = get_rain();
  
  #ifdef PRINT
  PRINTLN;
  Serial.print("Temperature = ");
  Serial.print(tempc, 2);
  Serial.print("C,");
  
  Serial.print(" Humidity = ");
  Serial.print(humidity,0);
  Serial.print("%,");

  Serial.print(" Pressure = ");
  Serial.print(pressure,0);
  Serial.print("Pa,");

  Serial.print(" Light_lvl = ");
  Serial.print(light_lvl);
  Serial.print("V,");

  Serial.print(" VinPin = ");
  Serial.print(batt_lvl);
  Serial.print("V");
  
  Serial.print(" Wind Direction = ");
  Serial.print(windDirection);
  Serial.print("º");
  
  Serial.print(" Gust = ");
  Serial.print(getGust);
  
  Serial.print(" Wind Speed = ");
  Serial.print(getWindSpeed);
  
  Serial.print(" Rain = ");
  Serial.print(getRain);

  #endif
      
  #ifndef LOW_POWER
    // 600000+random(15,60)*1000
    if (millis()-lastTransmissionTime > delayBeforeTransmit) {
      #endif
    
      #ifdef WITH_APPKEY
            app_key_offset = sizeof(my_appKey);
            // set the app key in the payload
            memcpy(message,my_appKey,app_key_offset);
      #endif
      
        uint8_t r_size;
  
        // then use app_key_offset to skip the app key
          
      #ifdef FLOAT_SENSOR

        pressure = pressure/100;   
        sx1272.ftoa(tempc_str,tempc,2);
        sx1272.ftoa(humidity_str,humidity,0);
        sx1272.ftoa(light_lvl_str,light_lvl,2);
        sx1272.ftoa(pressure_str,pressure,2);
        sx1272.ftoa(batt_lvl_str,batt_lvl,2);
        sx1272.ftoa(windDirection_str,windDirection,0);
        sx1272.ftoa(getGust_str,getGust,0);
        sx1272.ftoa(getWindSpeed_str,getWindSpeed,0);
        sx1272.ftoa(getRain_str,getRain,3);

           #ifdef NEW_DATA_FIELD      
              r_size=sprintf((char*)message+app_key_offset, "\\!TC/%s/HU/%s/LU/%s/DO/%s/AZO/%s/WD/%s/WC/%s/RC/%s", tempc_str, humidity_str, light_lvl_str, pressure_str ,batt_lvl_str, windDirection_str, getWindSpeed_str, getRain_str );
           #else
              r_size=sprintf((char*)message+app_key_offset, "\\!#%d#%d", field_index, (int)temp);
           #endif            
      #endif
      
      #ifdef PRINT
          PRINTLN;
          PRINT_CSTSTR("%s","Sending ");
          PRINT_STR("%s",(char*)(message+app_key_offset));
          PRINTLN;
          
          PRINT_CSTSTR("%s","Real payload size is ");
          PRINT_VALUE("%d", r_size);
          PRINTLN; 
      #endif
      
      int pl=r_size+app_key_offset;
      sx1272.CarrierSense();
      startSend=millis();
      
      #ifdef WITH_APPKEY
            // indicate that we have an appkey
            sx1272.setPacketType(PKT_TYPE_DATA | PKT_FLAG_DATA_WAPPKEY);
      #else
            // just a simple data packet
            sx1272.setPacketType(PKT_TYPE_DATA);
      #endif
            
            // Send message to the gateway and print the result
            // with the app key if this feature is enabled
      #ifdef WITH_ACK
            int n_retry=NB_RETRIES;
            
            do {
              e = sx1272.sendPacketTimeoutACK(DEFAULT_DEST_ADDR, message, pl);
      
              if (e==3)
                PRINT_CSTSTR("%s","No ACK");
              
              n_retry--;
              
              if (n_retry)
                PRINT_CSTSTR("%s","Retry");
              else
                PRINT_CSTSTR("%s","Abort"); 
                
            } while (e && n_retry);   
            if(e==0){
               digitalWrite(STAT_BLUE, LOW);
               digitalWrite(STAT_GREEN,HIGH);
               delay(3000);               
            }       
      #else      
            e = sx1272.sendPacketTimeout(DEFAULT_DEST_ADDR, message, pl);
      #endif  
            endSend=millis();
          
      #ifdef WITH_EEPROM
            // save packet number for next packet in case of reboot
            my_sx1272config.seq=sx1272._packetNumber;
            EEPROM.put(0, my_sx1272config);
      #endif

      #ifdef PRINT
            PRINT_CSTSTR("%s","LoRa pkt size ");
            PRINT_VALUE("%d", pl);
            PRINTLN;
            
            PRINT_CSTSTR("%s","LoRa pkt seq ");
            PRINT_VALUE("%d", sx1272.packet_sent.packnum);
            PRINTLN;
          
            PRINT_CSTSTR("%s","LoRa Sent in ");
            PRINT_VALUE("%ld", endSend-startSend);
            PRINTLN;
                
            PRINT_CSTSTR("%s","LoRa Sent w/CAD in ");
            PRINT_VALUE("%ld", endSend-sx1272._startDoCad);
            PRINTLN;
      
            PRINT_CSTSTR("%s","Packet sent, state ");
            PRINT_VALUE("%d", e);
            PRINTLN;
      #endif
              
      #ifdef LOW_POWER
            PRINT_CSTSTR("%s","Switch to power saving mode\n");
      
            e = sx1272.setSleepMode();
      
            if (!e){
              PRINT_CSTSTR("%s","Successfully switch LoRa module in sleep mode\n");
                digitalWrite(STAT_BLUE,LOW);
                digitalWrite(STAT_GREEN,LOW);
          }
            else  
              PRINT_CSTSTR("%s","Could not switch LoRa module in sleep mode\n");
              
            FLUSHOUTPUT
            delay(50);
      
      #ifdef __SAMD21G18A__
            
      #else
            nCycle = idlePeriodInMin*60/LOW_POWER_PERIOD + random(2,4);
      
      #ifdef __MK20DX256__ 
            
      #endif          
      for (int i=0; i<nCycle; i++) {  
      
          #if defined ARDUINO_AVR_PRO || defined ARDUINO_AVR_NANO || ARDUINO_AVR_UNO || ARDUINO_AVR_MINI  
                    
          #ifdef LOW_POWER_HIBERNATE
                    Serial.println(test1);
                    Snooze.hibernate(sleep_config);
          #else
                    Serial.println(test2);            
                    Snooze.deepSleep(sleep_config);
          #endif
          
          #else
                    // use the delay function
                    delay(LOW_POWER_PERIOD*1000);
          #endif 
                                 
      Serial.println(i);
      PRINT_CSTSTR("%s",".");
      FLUSHOUTPUT; 
      delay(10);                        
      }
              
            delay(50);
      #endif      
            
      #else
            // use a random part also to avoid collision
            PRINT_VALUE("%ld", lastTransmissionTime);
            PRINTLN;
            PRINT_CSTSTR("%s","Will send next value in\n");
            lastTransmissionTime=millis();
            delayBeforeTransmit=idlePeriodInMin*60*1000+random(15,60)*1000;
            PRINT_VALUE("%ld", delayBeforeTransmit);
            PRINTLN;
      }
  #endif

  delay(50);
}

//Returns the wind speed 5 min
float get_wind_speed()
{
   
    float deltaTime = millis() - lastWindCheck; //750ms
    
    deltaTime /= 1000.0; //Covert to seconds

    if(deltaTime > 300){ //300s==5 minutos
      windSpeed = (float)windClicks / 60; //3 / 0.750s = 4
      windClicks = 0; //Reset and start watching for new wind
      windSpeed *= 1.492*2.4; //4 * 1.492 = 5.968MPH    *2.4 = KPH
      lastWindCheck = millis();
      }
     
    return(windSpeed);
}

//returns the rain 5 mins
float get_rain(){

  float rTime = millis() - lastRainCheck;
  rTime /=1000.0;
  if( rTime>300){

    rainin = dailyrainin;
    dailyrainin = 0;
    lastGust = 0;
    lastRainCheck = millis();
  }
  
  return(rainin);
}

//returns the rain 5 mins
float get_gust(){
  gustTime = millis() - lastGustTcheck;
  gustTime /= 1000.0;
    if(gustTime > 10){
      windIn = (float)windClicks / gustTime;
      if (windIn>lastGust ){
        lastGust = windIn;
        }
      gustTime = 0;
      lastGustTcheck = millis();
      }
  return (lastGust);  
}


//      Serial.println(nCycle);
//            for (int i=0; i<nCycle; i++) {  
//
//                LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
//                Serial.println(i);
//      
//                PRINT_CSTSTR("%s",".");
//                FLUSHOUTPUT; 
//           }
//            
//            delay(50);
