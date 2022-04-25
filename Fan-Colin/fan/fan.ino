//Colin Martires
//DC Motor and RTC Implementation

//libraries for RTC
#include <Wire.h>
#include "DS1307.h"

//define PORT E registers
volatile unsigned char* DDR_E = (unsigned char*) 0x2D;
volatile unsigned char* PORT_E = (unsigned char*) 0x2E;
volatile unsigned char* PIN_E = (unsigned char*) 0x2C;

//define variables
DS1307 clock;
int input;

void setup()
{
  //set pin 3 and 5 as OUTPUT
  *DDR_E |= B00111000;

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
    }
    else
    {
      setFan(0);
      printTime();
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
//state == 1 -> fan ON
//state == 0 -> fan OFF
void setFan(int state)
{
  if(state == 1)
  {
    //set pin 5 to HIGH to ENABLE fan
    *PORT_E = (1 << 5) | *PORT_E;

    //set pin 3 to HIGH to spin fan
    *PORT_E = (1 << 3) | *PORT_E;
    *PORT_E = ~(1 << 4) & *PORT_E;
    Serial.print("Fan was turned ON\n");
  }
  else if (state == 0)
  {
    //set pin 5 to LOW ro DISABLE fan
    *PORT_E = ~(1 << 5) & *PORT_E;
    Serial.print("Fan was turned OFF\n");
  }
}
