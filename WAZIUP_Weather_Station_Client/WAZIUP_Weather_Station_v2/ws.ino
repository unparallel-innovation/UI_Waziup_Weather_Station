
#include <WeatherStation.h>
WeatherStation ws(&Serial1);


void setup()
{
  ws.init();
}

void loop()
{
  if(ws.task()!=0);
}
