/*---------------------------------------------------
CPE301 Semester Project
Swamp Cooler
Group 35: Rameen Feda, Peyton Thomas, Colin Martires
Date: 5/2/2022
---------------------------------------------------*/

// TODO:
// - create function to analyze parameters and return value
//   corresponding to state of swamp cooler
// - remember to add printTime!!!

//libraries for RTC(Wire.h, DS1307.h), LCD(LiquidCrystal.h), Temp and Humidity Sensor(DHT.h)
#include <Wire.h>
#include "DS1307.h"
#include <LiquidCrystal.h>
#include "DHT.h"

//define macros for DHT sensor
#define DHTPIN 10     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   

//define PORT registers
volatile unsigned char* DDR_A = (unsigned char*) 0x21;
volatile unsigned char* PORT_A = (unsigned char*) 0x22;
volatile unsigned char* PIN_A = (unsigned char*) 0x20;

volatile unsigned char* DDR_L = (unsigned char*) 0x10A;
volatile unsigned char* PORT_L = (unsigned char*) 0x10B;
volatile unsigned char* PIN_L = (unsigned char*) 0x109;

volatile unsigned char* my_ADCSRA = (unsigned char *) 0x7A;
volatile unsigned char* my_ADCSRB = (unsigned char *) 0x7B;
volatile unsigned char* my_ADMUX = (unsigned char *) 0x7c;
volatile unsigned int* ADC_DATA = (unsigned int *) 0x78;

//define variables
DS1307 clock;
int input;
unsigned int resval = 0; 
int respin = A5; 

//create DHT and LiquidCrystal objects
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 13);

void setup()
{
  //set pin PL0, PL1, and PL2 as OUTPUT for DC Motor
  *DDR_L |= B00000111;

  //set pin PA0, PA1, PA2, and PA2 as OUTPUT for LEDs
  *DDR_A |= B00001111;

  //initialize date and time on RTC
  setClockTime();

  //set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  adc_init();
  delay(500);
  dht.begin();

  Serial.begin(9600);
}

void loop()
{
  //read humidity, temperature, and resevoir water level
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  resval = adc_read(5);

  //ERROR STATE - water level low
  if (resval <= 10)
  {
    lcd.print("Error: ");
    lcd.setCursor(0, 1);
    lcd.print("No water ");
    setFan(0);
    setLED(3);
  }
  else if (resval > 10 && resval <= 320)
  {
    lcd.print("Error: ");
    lcd.setCursor(0, 1);
    lcd.print("Water level: Low ");
    setFan(0);
    setLED(3);
  }
  else if (resval > 320)
  {
    lcd.print("Humidity: ");
    lcd.print(h);
    lcd.print("% ");
    lcd.setCursor(0,1);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print(" C   ");
    setFan(1);
    setLED(0);
  }

  delay(1000);
  lcd.clear();


  // if(Serial.available())
  // {
  //   input = Serial.read();
  //   Serial.println(input);

  //   if(input > 50)
  //   {
  //     setFan(1);
  //     printTime();
  //     setLED(2);
  //   }
  //   else
  //   {
  //     setFan(0);
  //     printTime();
  //     setLED(1);
  //   }
  // }
}

//print time and date from RTC
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

//set clock time for RTC
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

//Turn on LED [0 = Green, 1 = Yellow, 2 = Blue, 3 = Red]
void setLED(int color)
{
  //Clear LEDs
  *PORT_A = ~(1 << 0) & *PORT_A;
  *PORT_A = ~(1 << 1) & *PORT_A;
  *PORT_A = ~(1 << 2) & *PORT_A;
  *PORT_A = ~(1 << 3) & *PORT_A;
  
  *PORT_A = (1 << color) | *PORT_A;
}

// 320-420 medium, 420 high
void adc_init()
{
  *my_ADCSRA |= 0b10000000;
  *my_ADCSRA &= 0b11011111;
  *my_ADCSRA &= 0b11110111;
  *my_ADCSRA &= 0b11111000;

  *my_ADCSRB &= 0b11110111;
  *my_ADCSRB &= 0b11111000;

  *my_ADMUX &= 0b01111111;
  *my_ADMUX |= 0b01000000;
  *my_ADMUX &= 0b11011111;
  *my_ADMUX &= 0b11100000;
}

unsigned int adc_read(unsigned char adc_channel)
{
  *my_ADMUX &= 0b11100000;
  *my_ADCSRB &= 0b11110111;
  if (adc_channel > 7)
  {
    adc_channel -= 8;
    *my_ADCSRB |= 0b00001000;
  }
  *my_ADMUX += adc_channel;
  *my_ADCSRA |= 0x40;
  while((*my_ADCSRA & 0x40) != 0);
  return *ADC_DATA;
}
