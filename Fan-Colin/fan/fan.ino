//Colin Martires
//DC Motor and RTC Implementation

//libraries for RTC
#include <Wire.h>
#include "DS1307.h"

//define PORT registers
volatile unsigned char* DDR_A = (unsigned char*) 0x21;
volatile unsigned char* PORT_A = (unsigned char*) 0x22;
volatile unsigned char* PIN_A = (unsigned char*) 0x20;

volatile unsigned char* DDR_L = (unsigned char*) 0x10A;
volatile unsigned char* PORT_L = (unsigned char*) 0x10B;
volatile unsigned char* PIN_L = (unsigned char*) 0x109;


//define variables
DS1307 clock;
int input;

void setup()
{
  //set pin PL0, PL1, and PL2 as OUTPUT for DC Motor
  *DDR_L |= B00000111;

  //set pin PA0, PA1, PA2, and PA2 as OUTPUT for LEDs
  *DDR_A |= B00001111;

  setClockTime();

  Serial.begin(9600);
}

void loop()
{
  if(Serial.available())
  {
    input = Serial.read();
    Serial.println(input);

    if(input > 50)
    {
      setFan(1);
      printTime();
      setLED(2);
    }
    else
    {
      setFan(0);
      printTime();
      setLED(1);
    }
  }
}

void printTime()
{
    clock.getTime();
    Serial.print(clock.hour, DEC);
    Serial.print(":");
    Serial.print(clock.minute, DEC);
    Serial.print(":");
    Serial.print(clock.second, DEC);
    Serial.print("  ");
    Serial.print(clock.month, DEC);
    Serial.print("/");
    Serial.print(clock.dayOfMonth, DEC);
    Serial.print("/");
    Serial.print(clock.year + 2000, DEC);
    Serial.print("\n");
}

//set clock time
void setClockTime()
{
  clock.begin();
  clock.fillByYMD(2022, 4, 13);
  clock.fillByHMS(20, 34, 0);
  clock.fillDayOfWeek(WED);
  clock.setTime();
}

//set fan state
//state == 1 -> fan ON1
//state == 0 -> fan OFF
void setFan(int state)
{
  if(state == 1)
  {
    //set PL1 to HIGH to ENABLE fan
    *PORT_L = (1 << 1) | *PORT_L;
  
    //set PL2 to HIGH and PL0 to LOW to spin fan
    *PORT_L = (1 << 2) | *PORT_L;
    *PORT_L = ~(1 << 0) & *PORT_L;
    Serial.print("Fan was turned ON\n");
  }
  else if (state == 0)
  {
    //set PL1 to LOW to DISABLE fan
    *PORT_L = ~(1 << 1) & *PORT_L;
    Serial.print("Fan was turned OFF\n");
  }
}

//Turn on LED | 0 = Green, 1 = Yellow, 2 = Blue, 3 = Red
void setLED(int color)
{
  //Clear LEDs
  *PORT_A = ~(1 << 0) & *PORT_A;
  *PORT_A = ~(1 << 1) & *PORT_A;
  *PORT_A = ~(1 << 2) & *PORT_A;
  *PORT_A = ~(1 << 3) & *PORT_A;
  
  *PORT_A = (1 << color) | *PORT_A;
}
