
#include <Adafruit_FXOS8700.h>
#include <Adafruit_FXAS21002C.h>
#include<Wire.h>

/*
 * Program to sample 3 axis data from 9dof board
 * Data returned: Accelerometer: (x,y,z)m.s^-2
 *                Gyroscope:     (x,y,z) rad/s
 * Data sampled @ 10Hz for 20 mins continuous
 * Data transmitted to Host via USB
 *
 */

 //acc and gyro objects
 Adafruit_FXOS8700 acc = Adafruit_FXOS8700(0x8700A,0x8700B);
 unsigned long startTime;
 unsigned long stopTime;
 unsigned long sampleTime;
 
void setup() 
{
  //begin serial communication @ 115200 bits/s
  Serial.begin(115200);
  Serial.print("Waiting for Serial...");
  while(!Serial);
  Serial.println("Done");
  // init sensors
  sensor_t accel,mag;

 
 //check for presence
 bool acc_online = acc.begin(ACCEL_RANGE_4G);
 if(!acc_online)
  {
    /* There was a problem detecting the FXOS8700 ... check your connections */
    Serial.println("Ooops, no FXOS8700 detected ... Check your wiring!");
    while(1);
  } 
  

 
Serial.println("Starting Test:");
Serial.println("==============");
}

void loop() 
{
  //sample for 20mins continuous
  int count = 0;
  sampleTime = 1*60*1000UL; //convert to ms
  //Sample @10Hz (0.1 s)
  
  startTime = millis();  
  unsigned long elapsedTime = millis() -startTime;
  while(elapsedTime < sampleTime)
  {
    Serial.println(get_AcceleroMagnet_data());
    ++count;
    delay(100);
    elapsedTime = millis() - startTime;
  }
  Serial.println("Test Complete");
  Serial.print("Number of samples:"); Serial.println(count);
  Serial.print("Elapsed Time:"); Serial.println(elapsedTime);
  waitloop();
}
void waitloop()
{
  while(1);
}

String get_AcceleroMagnet_data()
{
  //sample Accelerometer data
  sensors_event_t aevent,mevent;
  acc.getEvent(&aevent,&mevent);

  //format data
  char tempx[2][6];
  dtostrf(aevent.acceleration.x,4,2,tempx[0]);
  dtostrf(mevent.magnetic.x,4,2,tempx[1]);
  char tempy[2][6];
  dtostrf(aevent.acceleration.y,4,2,tempy[0]);
  dtostrf(mevent.magnetic.y,4,2,tempy[1]);
  char tempz[2][6];
  dtostrf(aevent.acceleration.z,4,2,tempz[0]);
  dtostrf(mevent.magnetic.z,4,2,tempz[1]);
  
  
  //print to serial in form (x,y,z)
  char accdata[100];
  sprintf(accdata,"Accelerameter: X: %s Y: %s Z: %s m/s^2",tempx[0],tempy[0],tempz[0]);
  char magdata[100];
  sprintf(magdata,"Magnetometer: X: %s Y: %s Z: %s uT",tempx[1],tempy[1],tempz[1]);
  char data[200];
  sprintf(data,"%s\t%s",accdata,magdata);
  return data;

}
