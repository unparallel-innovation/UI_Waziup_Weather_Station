LoRa Gateway Modifications
==========

This document describes the modifications made to the [WAZIUP Low-cost LoRa gateway](https://github.com/CongducPham/LowCostLoRaGw) , so that the gateway can push the weather data to two Cloud repositories, such as the actual WAZIUP Platform, and a ThingsSpeak channel created for this Weather data. The original code was obtained at: [https://github.com/CongducPham/LowCostLoRaGw](https://github.com/CongducPham/LowCostLoRaGw), then the lora gateway was installed by following this tutorial.

[https://github.com/CongducPham/LowCostLoRaGw/blob/master/README.md](https://github.com/CongducPham/LowCostLoRaGw/blob/master/README.md)

The data structure defined in the context of WAZIUP, and shown here as an example of the Weather Station data: “\!TO/18.6/TI/19/PA/1023/HU/40/WS/30,…” is used to represent the data sent from the sensor nodes to the gateway. This represents weather parameters like Atmospheric temperature, Humidity, Barometric pressure, etc. After parsing this information the gateway reads the cloud configuration file (“clouds.json”), and pushes the data the ThingSpeak channel and to the WAZIUP broker. In this case the following cloud configuration files were used:

* [**CloudThingSpeakWeatherStation.py:**](https://github.com/unparallel-innovation/UI_Waziup_Weather_Station/blob/master/WAZIUP_Gateway_Modifications/CloudThingSpeakWeatherStation.py) Is based on the file [CloudThingSpeak.py](https://github.com/CongducPham/LowCostLoRaGw/blob/master/gw_full_latest/CloudThingSpeak.py) and is responsible for sending each weather parameter into a [thingspeak channel](https://thingspeak.com/channels/252734) after specifying the channel key.

    ```python
    (...)
    #Key defining the thingspeak channel
    _def_thingspeak_channel_key=’XXXXXXXXXXXXXXXX’
    (...)
    #Each weather parameter is attributed to a different field (temperature ->     field1, humidity -> field2, etc).
    str = ''
    try:
        for i in range(len(data)):
            if data[i] == 'TO':
                str = str+'field1='+data[i+1]+'&'
            elif data[i] == 'TI':
                str = str+'field2='+data[i+1]+'&'
            elif data[i] == 'HU':
                str = str+'field3='+data[i+1]+'&'
            elif data[i] == 'PA':
                str = str+'field4='+data[i+1]+'&'
            elif data[i] == 'WD':
                str = str+'field5='+data[i+1]+'&'
            elif data[i] == 'WS':
                str = str+'field6='+data[i+1]+'&'
            elif data[i] == 'RA':
                str = str+'field7='+data[i+1]+'&'
            elif data[i] == 'UT':
                str = str+'field8='+data[i+1]+'&'
    except:
        print "Error"
    cmd = 'curl -s -k -X POST --data '+str+ ' https://api.thingspeak.com/update?key='+data[0]
    (...)
    ```
* [**CloudWAZIUP.py:**](https://github.com/unparallel-innovation/UI_Waziup_Weather_Station/blob/master/WAZIUP_Gateway_Modifications/CloudWAZIUP.py) Is used to send weather parameters to the WAZIUP broker which is built on the top of the FIWARE Orion context broker, a customized [freeboard page](http://freeboard.waziup.io/index.html#source=http://thingproxy.freeboard.io/fetch/https://www.dropbox.com/s/qksb30nq0fv0thi/WAZIUP_WEATHER_STATION_V2.json?dl=1) was created with the purpose of displaying through several graphical widgets information about the stored weather parameters, this script uses the following configurations.
    ```python
    (...)
    #server: CAUTION must exist
    waziup_server="http://broker.waziup.io/v2"
    
    #project name
    project_name="waziup"
    
    #your organization: CHANGE HERE
    organization_name="UI"
    
    #service tree: CHANGE HERE at your convenience
    service_tree='/WEATHER'
    
    #sensor name: CHANGE HERE but maybe better to leave it as Sensor
    sensor_name="Sensor"
    
    #Fiware service path: DO NOT CHANGE HERE
    service_path='/'+organization_name+service_tree
    (...)
    ```

Those files needs to be added to the folder lora_gateway, and then referenced on the configuration of the gateway by adding the following entries on the configuration file clouds.json:

```
(...)
{
    "name":"WAZIUP Orion cloud",
    "script":"python CloudWAZIUP.py",
    "type":"iotcloud",
    "write_key":"",
    "enabled":true
},
{
    "name":"ThingSpeak cloud",
    "script":"python CloudThingSpeakWeatherStation.py",
    "type":"iotcloud",
    "write_key":"",
    “enabled":true
},
(...)
```
When the WAZIUP Gateway receives weather data from the sensor nodes, it will execute the scripts defined on this configuration file, and this ensures that the weather data is sent simultaneously to both Thingspeak channel and WAZIUP Platform.



