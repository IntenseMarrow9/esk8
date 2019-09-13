#include <EEPROM.h>
#include <TFT.h> // Arduino LCD library
#include <SPI.h>
#include <SoftwareSerial.h>
SoftwareSerial BTserial(3, 4); // RX | TX
// pin defines
#define cs   10
#define dc   8
#define rst  9

TFT tft = TFT(cs, dc, rst);
volatile byte halfRev,revCount;       //  VOLATILE DATA TYPE TO STORE REVOLUTIONS
unsigned long int rpm;  //  DEFINE RPM AND MAXIMUM RPM
unsigned long time;         //  DEFINE TIME TAKEN TO COVER ONE REVOLUTION
int mph = 0;             //  A VARIABLE TO DETERMINE WHETHER THE LCD NEEDS TO BE CLEARED OR NOT
long prevTime = 0;       //  STORE IDLE TIME TO TOGGLE MENU
char mphPrintout[4];
char distPrintout[4];
String mphChar = "";
String distChar = "";
volatile int distance,prevDist=71,finalDist;      //recorded distance
int cruise;
String prevMph = "";
int voltPercent,prevPercent;
long voltage=69;

long readVcc(long voltage) 
{ long result; // Read 1.1V reference against AVcc 
ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); 
delay(2); // Wait for Vref to settle 
ADCSRA |= _BV(ADSC); // Convert 
while (bit_is_set(ADCSRA,ADSC)); 
result = ADCL; 
result |= ADCH<<8; 
result = 1126400L / result; // Back-calculate AVcc in mV return result;
voltage = result;
return voltage; 
}
void setup()
{
  pinMode(A2, INPUT);
  BTserial.begin(9600);
 
  Serial.begin(115200);
  tft.begin();
  tft.background(0, 0, 0); // clear the screen with a black background
  tft.setRotation(4);
  tft.setTextSize(2);
  tft.setCursor(48, 115);
  tft.stroke(255, 255, 0);
  tft.print("mph");
  tft.stroke(0, 255, 0);
  tft.line(105,2,125,2);        //battery icon
  tft.line(105,12,125,12);
  tft.line(125,2,125,13);
  tft.line(104,2,104,4);
  tft.line(104,13,104,11);
  tft.line(102,4,105,4);
  tft.line(102,10,105,10);
  tft.line(102,4,102,10);
  tft.setCursor(119,4);
  tft.setTextSize(1);
  tft.stroke(255,255,255);
  tft.print("%");                 //battery icon
  //prevDistance = EEPROM.read(0);
  
  
  halfRev = 0;
  rpm = 0;
  time = 0;
  prevTime = 0;

 // EEPROM.write(0,71);
  
}
void loop()
{
  //Serial.println(digitalRead(A2));
  while(BTserial.available())
  BTserial.read();
  clearScreen();
  calculateMph();
  //calculateDist();
  setCursor(); 
  tft.stroke(255, 255, 255);
  tft.setTextSize(8);    //use to be 8
  mphChar=String(mph);
  mphChar.toCharArray(mphPrintout, 4);
  printBars();
  tft.print(mphPrintout);

  //readVcc(voltage);
  //displayVoltage();
  saveData();
  Serial.println(voltage);
  delay(500); 
}
void clearScreen()
{
  tft.stroke(0, 0, 0);
  tft.setTextSize(8);
  if (mphChar.length()==2)
  {
    tft.text(mphPrintout, 16, 50);
  }
  else
  {
    tft.text(mphPrintout, 45, 50);
  }
  tft.fill(0,0,0);        //clear battery percentage
  tft.rect(105,3,13,9); 
}
void setCursor()
{
  if ( mphChar.length()==2)
  { 
    tft.setCursor(16, 50);
  }  
  else 
  {
    tft.setCursor(45, 50);
  }
}
void displayVoltage()
{
  float anotherFloat = 0.10;                  // arduino measurement is about 90% accurate
  voltage = voltage - anotherFloat;          // and it's 10% more than multimeter reading
  float voltDiff = voltage - 3.00;      // Current voltage - minimum voltage
    if ( voltDiff<1.2)
  {
    voltPercent=99;
    if (voltDiff>=0.94&&voltDiff<=0.96)
    {
      voltPercent=95;
    }
  }
   if ( voltDiff<0.91)
  {
    voltPercent=90;
    if (voltDiff>=0.84&&voltDiff<=0.86)
    {
      voltPercent=85;
    }
  }
   if ( voltDiff<0.81)
  {
    voltPercent=80;
    if (voltDiff>=0.74&&voltDiff<=0.76)
    {
      voltPercent=75;
    }
  }
    if ( voltDiff<0.71)
  {
    voltPercent=70;
     if (voltDiff>=0.64&&voltDiff<=0.66)
    {
      voltPercent=65;
    }
  }
   if ( voltDiff<0.61)
  {
    voltPercent=60;
     if (voltDiff>=0.54&&voltDiff<=0.56)
    {
      voltPercent=55;
    }
  }
  if ( voltDiff<0.51)
  {
    voltPercent=50;
     if (voltDiff>=0.44&&voltDiff<=0.46)
    {
      voltPercent=45;
    }
  }
  if ( voltDiff<0.41)
  {
    voltPercent=40;
     if (voltDiff>=0.34&&voltDiff<=0.36)
    {
      voltPercent=35;
    }
  }
   if ( voltDiff<0.31)
  {
    voltPercent=30;
     if (voltDiff>=0.24&&voltDiff<=0.26)
    {
      voltPercent=25;
    }
  }
  if ( voltDiff<0.21)
  {
    voltPercent=20;
     if (voltDiff>=0.14&&voltDiff<=0.16)
    {
      voltPercent=15;
    }
  }
  if ( voltDiff<0.11)
  {
    voltPercent=10;
     if (voltDiff>=0.04&&voltDiff<=0.06)
    {
      voltPercent=5;
    }
  }
  if ( voltDiff<=0.00)
  {
    voltPercent=0;
  }
  tft.setCursor(106,4);
  tft.setTextSize(1);
  tft.stroke(255,255,255);
  tft.print(voltPercent);
  //Serial.println(voltage);
}
void printBars()
{
  tft.stroke(255,255,255);
  tft.fill(0,255,0);
  tft.rect(11,20,8,20);
  tft.rect(22,20,8,20);
  tft.rect(33,20,8,20);
  tft.rect(44,20,8,20);
  
  tft.fill(0,170,230);
  tft.rect(55,20,8,20);
  tft.rect(66,20,8,20);
  tft.rect(77,20,8,20);
  
  tft.fill(0,90,255);
  tft.rect(88,20,8,20);
  tft.rect(99,20,8,20);
  
  tft.fill(0,0,255);
  tft.rect(110,20,8,20);
}
void coverBars()
{
  
}
void saveData()  //saves data when bluetooth loses connection
{
  
  while (digitalRead(A2)==0)     //if bluetooth disconnected, save to EEPROM
  {
   for (int i = 1; i<=158;i++)
   {
    int j = i-1;
    int lim = 127;
    if (distance<=lim*i)
    {
      EEPROM.update(j, distance-(lim*j)); 
      finalDist = distance-(lim*j)+lim*j;
      prevDist = finalDist;
      Serial.println(finalDist);
      delay(1000);
      break;  
    }
   
   }
  }
}
void calculateDist()
{
  distance = prevDist+(revCount*7.8106359e-5);    //1 halfRev = 125.7 mm or 7.8106359e-5 miles
  tft.stroke(255,255,255);
  tft.setTextSize(2);
  tft.setCursor(20, 130);
  tft.print(finalDist);
}
void calculateMph()
{
  
  rpm = BTserial.read();
   
  mph = (60 * 251.2 * (rpm * 6.21371e-7)*2)+1;
 
}


