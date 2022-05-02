/*---------------------------------------------------
CPE301 Semester Project
Swamp Cooler
Group 35: Rameen Feda, Peyton Thomas, Colin Martires
Date: 5/2/2022
---------------------------------------------------*/

//libraries for RTC(Wire.h, DS1307.h), LCD(LiquidCrystal.h), Temp and Humidity Sensor(DHT.h), and Stepper Motor(Stepper.h)
#include <Wire.h>
#include "DS1307.h"
#include <LiquidCrystal.h>
#include "DHT.h"
#include <Stepper.h>
#include <stdbool.h>

//define macros
#define DHTPIN 10         //Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   
#define STEPS 64
#define tempThresh 26     //temperature threshold

//---------------PORT registers---------------------------//
//LEDs
volatile unsigned char* DDR_A = (unsigned char*) 0x21;
volatile unsigned char* PORT_A = (unsigned char*) 0x22;
volatile unsigned char* PIN_A = (unsigned char*) 0x20;

//DC Motor (Fan)
volatile unsigned char* DDR_L = (unsigned char*) 0x10A;
volatile unsigned char* PORT_L = (unsigned char*) 0x10B;
volatile unsigned char* PIN_L = (unsigned char*) 0x109;

//ADC
volatile unsigned char* my_ADCSRA = (unsigned char *) 0x7A;
volatile unsigned char* my_ADCSRB = (unsigned char *) 0x7B;
volatile unsigned char* my_ADMUX = (unsigned char *) 0x7c;
volatile unsigned int* ADC_DATA = (unsigned int *) 0x78;

//button
volatile unsigned char* PORT_E = (unsigned char*) 0x28;
volatile unsigned char* DDR_E = (unsigned char*) 0x27;
volatile unsigned char* PIN_E = (unsigned char*) 0x26;

//ISR
volatile unsigned char* myEIMSK = (unsigned char*) 0x1D; 
volatile unsigned char* myEIFR = (unsigned char*) 0x1C;  
volatile unsigned char* mySREG = (unsigned char*) 0x3F; 
volatile unsigned char* myEICRB = (unsigned char*) 0x6A;
//--------------------------------------------------------//

//define variables
DS1307 clock;
unsigned int resval = 0; 
int respin = A5;
int OldVal = 0;           //initialize oldval to be 0
signed int Back = -256;   //move the motor -256 steps (-45 degrees)
signed int Forward = 256; //move the motor 256 steps (45 degrees)
int angleLimit = 90;      //start in the "middle"
bool inErrorState;        //enables/disables stepper motor
volatile bool currentStatus = false;
volatile bool previousStatus = true;

//create DHT, LiquidCrystal, and Stepper objects
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 13);
Stepper stebber = Stepper(STEPS, 6, 8, 7, 9);

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

  stebber.setSpeed(256);
  adc_init();
  OldVal = adc_read(8);  //initialize OldVal to be the same as NewVal so that nothing occurs until a change happens

  DDRE &= ~(1 << 4);
  PORTE |= (1 << 4);
  DDRE |= (1 << 1);
  
  EICRB |= (1 << ISC41);
  EICRB &= ~(1 << ISC40);
  EIMSK |= (1 << INT4);

  sei();

  Serial.begin(9600);
}

void loop()
{
  while(!previousStatus & currentStatus)
  {
    //read humidity, temperature, and resevoir water level
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    resval = adc_read(5);

    //States
    if(resval <= 320)                                        //ERROR STATE - low water
    {
      lcd.clear();
      printTime("ERROR");
      lcd.setCursor(0,0);    
      lcd.print("Error: ");
      lcd.setCursor(0, 1);
      lcd.print("Water level: Low ");
      setLED(3);
      setFan(0);
      inErrorState = true;
    }
    else if(resval > 320 && temperature < tempThresh)        //IDLE STATE - monitors temp and humidity
    {
      lcd.clear();
      printTime("IDLE");  
      lcd.print("Humidity: ");
      lcd.print(humidity);
      lcd.print("% ");
      lcd.setCursor(0,1);
      lcd.print("Temp: ");
      lcd.print(temperature);
      lcd.print(" C   ");
      setLED(0);
      setFan(0);
      inErrorState = false;
    }
    else if(resval > 320 && temperature >= tempThresh)       //RUNNITNG STATE - fan on
    {
      lcd.clear();
      printTime("RUNNING");
      lcd.setCursor(0,0);
      lcd.print("Humidity: ");
      lcd.print(humidity);
      lcd.print("% ");
      lcd.setCursor(0,1);
      lcd.print("Temp: ");
      lcd.print(temperature);
      lcd.print(" C   ");
      setLED(2);
      setFan(1);
      inErrorState = false;
    }

    if(!inErrorState)                                        //disable stepper motor in ERROR state
    {
      int NewVal = adc_read(8);
      
      if((NewVal > OldVal + 15) && (angleLimit < 136)){      //adding 15 allows for pot corrections
        stebber.step(Back);  
        angleLimit += 45;                                    //increment the angleLimit so that the motor cannot turn past 0 or 180 degrees
        delay(500);
        printTime("Vent angle changed");
      }
      else if((NewVal + 15 < OldVal) && (angleLimit > 44)){  //adding 15 allows for pot corrections
        stebber.step(Forward);
        angleLimit -= 45;                                    //increment the angleLimit so that the motor cannot turn past 0 or 180 degrees
        delay(500);
        printTime("Vent angle changed");
      }
      OldVal = NewVal;
    }
    delay(1000);
  }

  while(previousStatus & !currentStatus)                     //DISABLED STATE
  {
    lcd.clear();
    lcd.setCursor(0,0);
    printTime("Disabled");
    lcd.print("System");
    lcd.setCursor(0,1);
    lcd.print("Disabled");
    setLED(1);
    setFan(0);
    inErrorState = false;
    delay(1000);
  }

  delay(1000);
}

//print time and date from RTC
void printTime(String message)
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
    Serial.print(message);
    Serial.print("\n");
}

//set clock time for RTC
void setClockTime()
{
  clock.begin();
  clock.fillByYMD(2022, 5, 2);
  clock.fillByHMS(20, 34, 0);
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
  }
  else if (state == 0)
  {
    //set PL1 to LOW to DISABLE fan
    *PORT_L = ~(1 << 1) & *PORT_L;
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

ISR(INT4_vect)
{
  previousStatus = !previousStatus;
  currentStatus = !currentStatus;
}

//add speaker to play Nickleback