LoRa Gateway Modifications
==========

This document describes the modifications made to the [WAZIUP Low-cost LoRa gateway](https://github.com/CongducPham/LowCostLoRaGw) , so that the gateway can push the weather data to two Cloud repositories, such as the actual WAZIUP Platform, and a ThingsSpeak channel created for this Weather data. The original code was obtained at: [https://github.com/CongducPham/LowCostLoRaGw](https://github.com/CongducPham/LowCostLoRaGw), then the lora gateway was installed with support for newCloudDesign by following these tutorials.

[https://github.com/CongducPham/LowCostLoRaGw/blob/master/README.md](https://github.com/CongducPham/LowCostLoRaGw/blob/master/README.md)
[https://github.com/CongducPham/LowCostLoRaGw/blob/master/gw_advanced/README.md](https://github.com/CongducPham/LowCostLoRaGw/blob/master/gw_advanced/README.md)
[https://github.com/CongducPham/LowCostLoRaGw/blob/master/gw_advanced/new_cloud_design/README-NewCloud.md](https://github.com/CongducPham/LowCostLoRaGw/blob/master/gw_advanced/new_cloud_design/README-NewCloud.md)

Now the folder “WAZIUP Gateway Modifications” contains files in the format “Cloud(…).py” and each one together with the cloud configuration file (“clouds.json”, from the original LoRa Gateway) tells the gateway how to communicate with a specific cloud.

The data structure defined in the context of WAZIUP, and shown here as an example of the Weather Station data: “\!TC/18.6/HU/85/LU/56/DO/7.7/AZO/87,…” is used to represent the data sent from the sensor nodes to the gateway. This represents weather parameters like Atmospheric temperature, Humidity, Barometric pressure, etc. After parsing this information the gateway reads the cloud configuration file (“clouds.json”), and pushes the data the ThingSpeak channel and to the WAZIUP broker. In this case the following cloud configuration files were used:



* [**CloudThingSpeakWeatherStation.py:**](https://github.com/unparallel-innovation/UI_Waziup_Weather_Station/blob/master/WAZIUP%20Gateway%20Modifications/CloudThingSpeakWeatherStation.py) Is based on the file [CloudThingSpeak.py](https://github.com/CongducPham/LowCostLoRaGw/blob/master/gw_advanced/new_cloud_design/CloudThingSpeak.py) and is responsible for sending each weather parameter into a different field on a specified [thingspeak channel](https://thingspeak.com/channels/184796).

    ```python
    (...)
    #Key defining the thingspeak channel
    _def_thingspeak_channel_key=’XXXXXXXXXXXXXXXX’
    (...)
    #Each weather parameter is attributed to a different field (temperature ->     field1, humidity -> field2, etc).
    str = ''
    try:
        for i in range(len(data)):
            if data[i] == 'TC':
                str = str+'field1='+data[i+1]+'&'
            elif data[i] == 'HU':
                str = str+'field2='+data[i+1]+'&'
            elif data[i] == 'LU':
                str = str+'field3='+data[i+1]+'&'
            elif data[i] == 'DO':
                str = str+'field4='+data[i+1]+'&'
            elif data[i] == 'AZO':
                str = str+'field5='+data[i+1]+'&'
            elif data[i] == 'WD':
                str = str+'field6='+data[i+1]+'&'
            elif data[i] == 'WC':
                str = str+'field7='+data[i+1]+'&'
            elif data[i] == 'RC':
                str = str+'field8='+data[i+1]+'&'
    except:
        print "Error"
    cmd = 'curl -s -k -X POST --data '+str+ ' https://api.thingspeak.com/update?key='+data[0]
    (...)
    ```
* [**CloudWAZIUP.py:**](https://github.com/unparallel-innovation/UI_Waziup_Weather_Station/blob/master/WAZIUP%20Gateway%20Modifications/CloudWAZIUP.py) Is used to send weather parameters to the WAZIUP broker which is built on the top of the FIWARE Orion context broker, a customized [freeboard page](http://freeboard.waziup.io/index.html#source=http://thingproxy.freeboard.io/fetch/https://www.dropbox.com/s/qcoam8z8epblrzt/UI_WEATHER.json?dl=1) was created with the purpose of displaying through several graphical widgets information about the stored weather parameters, this script uses the following configurations.
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

After adding those files to the folder lora_gateway, is necessary to reference them on the configuration of the gateway by adding the following entries of the configuration file clouds.json:

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



