/*
 * Low-Cost Weather Station v2.0 - WAZIUP
 * 
 * Unparalell Inovations Lda
 * 
 * Last update: 
 * 
 * WeatherStation_v2_Simple: version with no Sleep
 */


#include <SPI.h>
#include <Wire.h>
#include "SX1272.h"   //Include the SX1272 LoRa module

#include <RTClib.h>  // Include the DS3231 library 
RTC_DS3231 rtc;

#include <Adafruit_AM2315.h>      // Include the AM2315 Temperature and Humidity sensor
Adafruit_AM2315 am2315;

#include <Adafruit_MPL115A2.h>
Adafruit_MPL115A2 mpl115a2;       // Include the MPL115A2 Barometric Pressure and Temperature sensor


//-------- Uncomment only 1 choice ------- 
#define ETSI_EUROPE_REGULATION
//#define FCC_US_REGULATION
//#define SENEGAL_REGULATION
//----------------------------------------


//-------- Uncomment only 1 choice ------- 
#define BAND868
//#define BAND900
//#define BAND433
//----------------------------------------


//-------- Do not change anything --------
#ifdef ETSI_EUROPE_REGULATION
#define MAX_DBM 14
#elif defined SENEGAL_REGULATION
#define MAX_DBM 10
#elif defined FCC_US_REGULATION
#define MAX_DBM 14
#endif
//----------------------------------------


//----------- Do not change anything -----------
#ifdef BAND868
#ifdef SENEGAL_REGULATION
#define DEFAULT_CHANNEL CH_04_868
#else
#define DEFAULT_CHANNEL CH_10_868
#endif
#elif defined BAND900
#define DEFAULT_CHANNEL CH_05_900
#elif defined BAND433
#define DEFAULT_CHANNEL CH_00_433
#endif
//-----------------------------------------------


#define PABOOST   // uncomment if your radio is an HopeRF RFM92W, HopeRF RFM95W, Modtronix inAir9B, NiceRF1276
                  // or you known from the circuit diagram that output use the PABOOST line instead of the RFO line

#define BAUDRATE 38400  // Operating Baudrate


//--------------- Comment or Uncomment to change features ---------------
#if not defined _VARIANT_ARDUINO_DUE_X_ && not defined __SAMD21G18A__
#define WITH_EEPROM  
#endif
#define WITH_APPKEY
//#define WITH_ACK
//------------------------------------------------------------------------

#define LORAMODE  1   // The LoRa mode
#define node_addr 8   // Node Address

// Change here the time in miliseconds between 2 reading & transmission
unsigned int sleepTime = 120000;   


//--------------- Comment or Uncomment to change features ---------------
#ifdef WITH_APPKEY
// Change the Appkey, but if GW checks for AppKey, must be in the AppKey
// list maintained by GW
uint8_t my_appKey[4] = {5, 6, 7, 8};
#endif
//------------------------------------------------------------------------


//----------- Uncomment to print via serial monitor -----------
#define PRINT
#define PRINT_DATA
#define PRINTLN                   Serial.println(F(""))
#define PRINT_STR(fmt,param)      Serial.print(param)
#define PRINT_CSTSTR(fmt,param)   Serial.print(F(param))
#define PRINT_VALUE(fmt,param)    Serial.print(param)
#define DELAY
//-------------------------------------------------------------


//----------- Do not change anything -----------
#ifdef WITH_EEPROM
#include <EEPROM.h>
#endif
//----------------------------------------------


#define DEFAULT_DEST_ADDR 1     // Change here the destination address


//----------- Do not change anything -----------
#ifdef WITH_ACK
#define NB_RETRIES 2
#endif
//----------------------------------------------


//----------- Do not change anything -----------
#ifdef WITH_EEPROM
struct sx1272config
{
  uint8_t flag1;
  uint8_t flag2;
  uint8_t seq;
  // can add other fields such as LoRa mode,...
};

sx1272config my_sx1272config;
#endif
//----------------------------------------------


//-------------- Global Variables --------------
char auxBuf[20];
uint8_t message[100];
unsigned long lastWindCheck = 0;   
volatile unsigned int rain = 0;
volatile unsigned int wind = 0;
#define WDIR A6
//----------------------------------------------




//---------------------------------------- Setup --------------------------------------
void setup() 
{
  int e;

  Serial.begin(BAUDRATE);     // Open serial communications

  pinMode(21, INPUT_PULLUP);    // Select Rain interrupt
  pinMode(22, INPUT_PULLUP);    // Select Wind interrupt
  attachInterrupt(21, ISR_RAIN, RISING);
  attachInterrupt(22, ISR_WIND, RISING);
  interrupts();


  Wire.begin();

  rtc.begin();    // Begin the RTC. Get Date & Time from system
  
  // Uncomment to adjust the date & time ONLY in the first compiler run
  // rtc.adjust(DateTime( F(__DATE__), F(__TIME__) ) - TimeSpan(0,1,0,0));
  if (rtc.lostPower())
  {
    rtc.adjust(DateTime( F(__DATE__), F(__TIME__) ) - TimeSpan(0,1,0,0));   // Get the Date and Time the compiler was run
  }

  
  am2315.begin();     // Connect the AM2315 sensor
  mpl115a2.begin();   // Connect the MPL115A2 sensor


  sx1272.ON();    // Power ON the LoRa module

  #ifdef WITH_EEPROM
    EEPROM.get(0, my_sx1272config);   // get config from EEPROM
  
    if (my_sx1272config.flag1 == 0x12 && my_sx1272config.flag2 == 0x34)   // found a valid config?
    {
        #ifdef PRINT
            Serial.println(F("Get back previous sx1272 config"));
            PRINTLN;
        #endif
  
        sx1272._packetNumber = my_sx1272config.seq;   // set sequence number for SX1272 library
  
        #ifdef PRINT
            Serial.print(F("Using packet sequence number of "));
            Serial.print(sx1272._packetNumber);
            PRINTLN;
        #endif
    }
    else 
    {
        // otherwise, write config and start over
        my_sx1272config.flag1 = 0x12;
        my_sx1272config.flag2 = 0x34;
        my_sx1272config.seq = sx1272._packetNumber;
    }
  #endif

  e = sx1272.setMode(LORAMODE);   // Set transmission mode and print the result
  
  #ifdef PRINT
    Serial.print(F("Setting Mode: state "));
    Serial.print(e);
    PRINTLN;
  #endif

  sx1272._enableCarrierSense = true;    // enable carrier sense
  
  #ifdef LOW_POWER
    // TODO: with low power, when setting the radio module in sleep mode there seem to be some issue with RSSI reading
    sx1272._RSSIonSend = false;
  #endif

  e = sx1272.setChannel(DEFAULT_CHANNEL);   // Select frequency channel

  #ifdef PRINT
    Serial.print(F("Setting Channel: state "));
    Serial.print(e);
    PRINTLN;
  #endif

  // Select amplifier line; PABOOST or RFO
  #ifdef PABOOST
    sx1272._needPABOOST = true;   // previous way for setting output power. powerLevel='x';
  #else
    // previous way for setting output power. powerLevel='M';
  #endif

  // e = sx1272.setPower(powerLevel);   // previous way for setting output power

  e = sx1272.setPowerDBM((uint8_t)MAX_DBM);
  
  #ifdef PRINT
    Serial.print(F("Setting Power: state "));
    Serial.print(e);
    PRINTLN;
  #endif
  
  e = sx1272.setNodeAddress(node_addr);   // Set the node address and print the result
  
  #ifdef PRINT
    Serial.print(F("Setting node addr: state "));
    Serial.print(e);
    PRINTLN;
  #endif

  #ifdef PRINT        // Print success messageS
    Serial.println(F("SX1272 successfully configured!"));
    Serial.println(F("Weather Station Config!"));
    Serial.println(F("Weather Station online!"));    // Weather Station it's ONLINE
  #endif

  #ifdef PRINT
    delay(1);
  #endif
}
//--------------------------------------------------------------------------------------



void ISR_RAIN()          // timer compare interrupt service routine
{
  rain++;
}

void ISR_WIND()          // timer compare interrupt service routine
{
  wind++;
}



// Returns the Wind Speed
float get_wind_speed(int wind)
{
  float windSpeed =0;
  float deltaTime =0;
  
  deltaTime = rtc.now().unixtime() - lastWindCheck; 

  if (deltaTime > 0)
  {
      windSpeed =  ((float)wind) / deltaTime;

      windSpeed *= 2.4;   //1 milha = 1.61 KPH
  }
  return (windSpeed);
}


// Returns the rain
float get_rain(int rain)
{
  long rainClicks=0;

  rainClicks= rain;
    
  return (rainClicks*0.2794); // Each interrup represent 0.011" iches of rain 1" to mm => 25.4mm
} 


// Return the Wind Direction
int get_wind_direction()
{
  unsigned int adc;

  adc = analogRead(WDIR); // get the current reading from the sensor

  // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
  // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
  // Note that these are not in compass degree order! See Weather Meters datasheet for more information.

 if (adc < 365) return (113); // 380
  if (adc < 382) return (68); //  393
  if (adc < 390) return (90); //  414
  if (adc < 423) return (158); // 456
  if (adc < 476) return (135); //508 
  if (adc < 529) return (203); //  551
  if (adc < 563) return (180); // 615
  if (adc < 657) return (23); //  680
  if (adc < 697) return (45); //  746
  if (adc < 790) return (248); //  801
  if (adc < 809) return (225); //  833
  if (adc < 856) return (338); // 878 
  if (adc < 905) return (0); //  913
  if (adc < 927) return (293); // 940
  if (adc < 960) return (315); //  967
  if (adc < 991) return (270); // 990
  return (-1); // error, disconnected?
}











//----------------------------------------- Loop ---------------------------------------
void loop() 
{
  uint8_t app_key_offset = 0;
  int e;

  DateTime now = rtc.now();

  //------------------- Get measures from the sensors----------------------
  long timeStamp = now.unixtime();    // Unixtime
  float pressure = mpl115a2.getPressure();   // Pressure in kPa
  float insideTemp = (mpl115a2.getTemperature() +3);    // Temperature (Cº) inside the box
  float humidity = am2315.readHumidity();   // Humidity
  float tempc = am2315.readTemperature();   // Temperature (Cº)
  float windDirection = get_wind_direction();  // Wind Direction
  float getWindSpeed = get_wind_speed(wind);   // Wind Speed
  float getRain = get_rain(rain);        // Rain

  #ifdef WITH_APPKEY
    app_key_offset = sizeof(my_appKey);
    memcpy(message, my_appKey, app_key_offset);   // set the app key in the payload
  #endif

  uint8_t r_size;
  
  // then use app_key_offset to skip the app key


  #ifdef PRINT
    delay(1);
  #endif



  #ifdef PRINT
    PRINTLN;
    Serial.print(F("Timestamp = "));
    Serial.print(timeStamp);
    Serial.print(F("seconds,"));
    
    Serial.print(F("Temperature = "));
    Serial.print(tempc);
    Serial.print(F("C,"));
    
    Serial.print(F(" Humidity = "));
    Serial.print(humidity);
    Serial.print(F("%,"));
    
    Serial.print(F(" Pressure = "));
    Serial.print(pressure);
    Serial.print(F("kPa,"));
    
    Serial.print(F(" Box Temperature = "));
    Serial.print(insideTemp);
    Serial.print(F("C,"));
    
    Serial.print(F(" Wind Direction = "));
    Serial.print(windDirection);
    Serial.print("º");
    
    Serial.print(F(" Wind = "));
    Serial.print(wind);        //getWindSpeed
    
    Serial.print(F(" Wind Speed = "));
    Serial.print(getWindSpeed);        //getWindSpeed
    Serial.print("kPh");
    
    Serial.print(F(" Rain Clicks= "));
    Serial.print(rain);
    
    Serial.print(F(" Rain = "));
    Serial.print(getRain);
    Serial.print("mm");
    PRINTLN;
  #endif


  //-------------------------- Message to be sent -------------------------------
  
    r_size = sprintf((char*)message + app_key_offset, "\\!UT/%s", dtostrf(timeStamp, 10, 0, auxBuf)); // !1HMA3TVV1TLUHU8B## (THINGSPEAK_KEY##MENSAGEM)
    r_size += sprintf((char*)message + app_key_offset + r_size, "/TO/%s", dtostrf(tempc, 2, 2, auxBuf)); 
    r_size += sprintf((char*)message + app_key_offset + r_size, "/HU/%s", dtostrf(humidity, 2, 0, auxBuf));
    r_size += sprintf((char*)message + app_key_offset + r_size, "/PA/%s", dtostrf(pressure, 2, 4, auxBuf));
    r_size += sprintf((char*)message + app_key_offset + r_size, "/TI/%s", dtostrf(insideTemp, 2, 1, auxBuf));
    r_size += sprintf((char*)message + app_key_offset + r_size, "/WD/%s", dtostrf(windDirection, 2, 0, auxBuf));
    r_size += sprintf((char*)message + app_key_offset + r_size, "/WS/%s", dtostrf(getWindSpeed, 3, 2, auxBuf));     //getWindSpeed
    r_size += sprintf((char*)message + app_key_offset + r_size, "/RA/%s", dtostrf(getRain, 2, 2, auxBuf));


  //----------------------- Start to send the message ----------------------------
  #ifdef PRINT
    PRINTLN;
    Serial.print(F("Sending "));
    PRINT_STR("%s", (char*)(message + app_key_offset));
    PRINTLN;
    Serial.print(F("Real payload size is "));
    Serial.print(r_size);
    PRINTLN;
  #endif

  int pl = r_size + app_key_offset;

  sx1272.CarrierSense();

  #ifdef WITH_APPKEY
    sx1272.setPacketType(PKT_TYPE_DATA | PKT_FLAG_DATA_WAPPKEY);    // indicate that we have an appkey
  #else
    sx1272.setPacketType(PKT_TYPE_DATA);    // just a simple data packet
  #endif

  // Send message to the gateway and print the result with the app key if this feature is enabled
  #ifdef WITH_ACK
      int n_retry = NB_RETRIES;
      
      do
      {
        e = sx1272.sendPacketTimeoutACK(DEFAULT_DEST_ADDR, message, pl);
  
        #ifdef PRINT
          if (e == 3)
          {
            PRINTLN;
            Serial.print(F("No ACK"));
          }
        #endif
          
        n_retry--;
          
        #ifdef PRINT
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
  #else
      e = sx1272.sendPacketTimeout(DEFAULT_DEST_ADDR, message, pl);
  #endif
  //------------------------ Stop to send the message ----------------------------

  
  #ifdef WITH_EEPROM
    my_sx1272config.seq = sx1272._packetNumber;   // save packet number for next packet in case of reboot
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
    //Serial.print((endSend - startSend));
    PRINTLN;
    Serial.print(F("LoRa Sent w/CAD in "));
    //Serial.print((endSend - sx1272._startDoCad));
    PRINTLN;
    Serial.print(F("Packet sent, state "));
    Serial.print(e);
    PRINTLN;
  #endif

  //--------------------------------------------------------------------------

  rain=0;
  wind=0;

  unsigned long before_time=0;
  unsigned long after_time=0;
  unsigned long current_time=0;


  now = rtc.now();
  before_time = now.unixtime();     // now.unixtime() return the seconds since 1970 till now

  lastWindCheck = rtc.now().unixtime();
  
  do
  {
    // Time for count the wind and rain interrupts.
      now = rtc.now();
      after_time = now.unixtime();
      current_time=(after_time-before_time)*1000;
      
  }while((current_time) < (sleepTime));


  am2315.begin();
  mpl115a2.begin();
  //--------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------
