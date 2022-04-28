#include <Stepper.h>
#define STEPS 64
Stepper stebber = Stepper(STEPS, 6, 8, 7, 9);  //switching 7 and 8 will make the motor work properly
int OldVal = 0;    //initialize oldval to be 0
signed int Back = -256;   //move the motor -256 steps (-45 degrees)
signed int Forward = 256;  //move the motor 256 steps (45 degrees)
int angleLimit = 90;   //start in the "middle"

volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADCH_DATA = (unsigned int*) 0x79;
volatile unsigned int* ADC_DATA = (unsigned int *) 0x78;


void setup() {
  stebber.setSpeed(256);
  adc_init();
  OldVal = adc_read(0);  //initialize OldVal to be the same as NewVal so that nothing occurs until a change happens
  
}

void loop() {
  int NewVal = adc_read(0);
/*  Serial.print(NewVal);
  Serial.print(" New");    //for testing
  Serial.print("\n"); */
  
  if((NewVal > OldVal + 15) && (angleLimit < 136)){    //adding 15 allows for pot corrections
    stebber.step(Back);  
    angleLimit += 45;  //this will increment the angleLimit so that the motor cannot turn past 0 0r 180 degrees
    delay(500);
/*    Serial.print("NEW >");
    Serial.print("\n");      //for testing */
  }
  else if((NewVal + 15 < OldVal) && (angleLimit > 44)){   //adding 15 allows for pot corrections
    stebber.step(Forward);
    angleLimit -= 45;  //this will increment the angleLimit so that the motor cannot turn past 0 0r 180 degrees
    delay(500);
/*    Serial.print("OLD > ");
    Serial.print("\n");  //for testing */
  }
  else{
    
  }
  OldVal = NewVal;
/* Serial.print(OldVal);
  Serial.print(" Old");
  Serial.print("\n");  */
  
} 

void adc_init(){
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
unsigned int adc_read(unsigned char adc_channel){
  *my_ADMUX &= 0b11100000;
  *my_ADCSRB &= 0b11110111;
  if (adc_channel > 7){
    adc_channel -= 8;
    *my_ADCSRB |= 0b00001000;

  }
  *my_ADMUX += adc_channel;
  *my_ADCSRA |= 0x40;
  while((*my_ADCSRA & 0x40) != 0);
  return *ADC_DATA;
}
