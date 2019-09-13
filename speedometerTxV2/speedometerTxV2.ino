
#include <SPI.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
SoftwareSerial BTserial(3, 4); // RX | TX


int revCount;    //half-rev count
volatile byte halfRev;       //  VOLATILE DATA TYPE TO STORE REVOLUTIONS
int rpm;  //  DEFINE RPM AND MAXIMUM RPM
unsigned long time;         //  DEFINE TIME TAKEN TO COVER ONE REVOLUTION
int mph = 0; 
long prevTime = 0;       //  STORE IDLE TIME TO TOGGLE MENU

void setup()
{
  
  BTserial.begin(9600);
  
  Serial.begin(115200);
  attachInterrupt(0, RPMcount, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
  halfRev = 0;     //half revolution
  rpm = 0;
  time = 0;
  //EEPROM.write(0,71);
  prevTime= 0;
  
  
}
void loop()
{ 
  calculateRpm();
  
  sendRpm();
  Serial.println(rpm); 
  
  
}
void RPMcount()//This function is called whenever a magnet/interrupt is detected by the arduino
{
  halfRev++;
  revCount++;
  
}

void sendRpm()
{
  BTserial.write(rpm);
    BTserial.flush();
}
void calculateRpm()
{
  if (halfRev >= 5)
  {
    rpm = 30 * 1000 / (millis() - prevTime) * halfRev; //calculations
    prevTime = millis();
    halfRev = 0;
    time = 0;
    
    //mph = (60 * 251.2 * (rpm * 6.21371e-7)*2)+1;
  }
}




