volatile unsigned char* my_ADCSRA = (unsigned char *) 0x7A;
volatile unsigned char* my_ADCSRB = (unsigned char *) 0x7B;
volatile unsigned char* my_ADMUX = (unsigned char *) 0x7c;
volatile unsigned int* ADC_DATA = (unsigned int *) 0x78;

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned int resval = 0; 
int respin = A5; 

void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  adc_init();

  
}

void loop() {
  // Print a message to the LCD.
  lcd.print("WATER LEVEL: ");
  // set the cursor to column 0, line 1
  lcd.setCursor(0, 1);

  resval = adc_read(5); //Read data from analog pin and store it to resval variable
  if (resval <= 3) {
    lcd.print("Empty ");
  } else if (resval > 3 && resval <= 350) {
    lcd.print("Low ");
  } else if (resval > 350 && resval <= 420) {
    lcd.print("Medium ");
  } else if (resval >= 420) {
    lcd.print("High ");
  }
  delay(1000);
  lcd.clear();
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
