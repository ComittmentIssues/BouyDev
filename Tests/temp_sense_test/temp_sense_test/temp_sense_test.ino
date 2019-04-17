

// DallasTemperature - Version: Latest 
#include <DallasTemperature.h>
// OneWire - Version: 2.3.4
#include <OneWire.h>

#define OWBUS 7
int numsense;
OneWire oneWire(OWBUS);
DallasTemperature sensors(&oneWire);
 unsigned long Tstart;
 unsigned long Tstop;
 unsigned long Tsample;
void setup()
{
  Serial.begin(115200);
  Serial.println("Waiting for Sensors...");
  sensors.begin();
  numsense = sensors.getDeviceCount();
  char buff [50];
  sprintf(buff,"Found %d sensor/s on bus %d\n",numsense,OWBUS);
  Serial.print(buff);
  Serial.println("Starting Temp Conversion");
  
}

double getTemp()
{
  double temp = 0;
  sensors.requestTemperatures();

  temp = sensors.getTempCByIndex(0);
 
  return temp;
}
void loop()
{
  //sample at 1/750 ms
  //sample continuous for Tsample Seconds
  Tsample = 20*60000UL;
  Tstart = millis();
  unsigned long Telapsed = millis() - Tstart;
  while(Telapsed < Tsample)
  {
    double tempC = getTemp();
    char buff[50];
    char strtemp[6];
    dtostrf(tempC, 4, 2, strtemp);
    sprintf(buff,"The Temperature is %s °C\n",strtemp);
    Serial.print(buff);    
    Telapsed = millis() - Tstart;
  }
  Serial.println("Test Complete");
  Serial.print("Duration:");Serial.print(Telapsed);Serial.print("ms");
  waitloop();
}

void waitloop()
{
  while(1);
  
}
