#include <stdbool.h> 

volatile bool currentStatus = false;
volatile bool previousStatus = true;

volatile unsigned char* PORT_E = (unsigned char*) 0x28;
volatile unsigned char* DDR_E = (unsigned char*) 0x27;
volatile unsigned char* PIN_E = (unsigned char*) 0x26;

volatile unsigned char* myEIMSK = (unsigned char*) 0x1D; 
volatile unsigned char* myEIFR = (unsigned char*) 0x1C;  
volatile unsigned char* mySREG = (unsigned char*) 0x3F; 
volatile unsigned char* myEICRB = (unsigned char*) 0x6A;

void setup() {
  DDRE &= ~(1 << 4);
  PORTE |= (1 << 4);

  DDRE |= (1 << 1);
  
  EICRB |= (1 << ISC41);
  EICRB &= ~(1 << ISC40);

  EIMSK |= (1 << INT4);

  sei();
}

void loop() {
  // put your main code here, to run repeatedly:
while(!previousStatus & currentStatus){
  PORTE |= (1 << 1);
}
while(previousStatus & !currentStatus){
  PORTE &= ~(1 << 1);
}

}

ISR (INT4_vect){
  previousStatus = !previousStatus;
  currentStatus = !currentStatus;
}
