#include <stdbool.h> 

volatile bool currentStatus = false;   //start current status as 0 (off)
volatile bool previousStatus = true;  //start previous status as 1 (off)

volatile unsigned char* PORT_E = (unsigned char*) 0x28;
volatile unsigned char* DDR_E = (unsigned char*) 0x27;
volatile unsigned char* PIN_E = (unsigned char*) 0x26;

volatile unsigned char* myEIMSK = (unsigned char*) 0x1D; 
volatile unsigned char* myEIFR = (unsigned char*) 0x1C;  
volatile unsigned char* mySREG = (unsigned char*) 0x3F; 
volatile unsigned char* myEICRB = (unsigned char*) 0x6A;

void setup() {
  DDRE &= ~(1 << 4);  //shift a 0 into bit 4 of port E (digital 2) sets 2 to input with pullup
  PORTE |= (1 << 4);  //shift a 1 into bit digital 2

  DDRE |= (1 << 1);
  
  EICRB |= (1 << ISC41);   //these lines make the interrupt rely on a FALLING state
  EICRB &= ~(1 << ISC40);  //^

  EIMSK |= (1 << INT4);   //This enables int4 (digital 2) to be an interrupt

  sei();   //sets status register I bit to 1, according to ATMEL datasheet
}

void loop() {
  
 while(!previousStatus & currentStatus){  //checks to see if previous is 0 and current is 1 (ON state) then runs included code until interrupt switches the states
  PORTE |= (1 << 1);    //this is purely for testing with an LED
 }
 while(previousStatus & !currentStatus){ //checks to see if previous is 1 and current is 0 (OFF state) then runs included code until interrupt switches the states
  PORTE &= ~(1 << 1);  //this is purely for testing with an LED
 }
}

ISR (INT4_vect){
  previousStatus = !previousStatus;   //the interrupt will flip the states previous/current to the opposite truth
  currentStatus = !currentStatus;
}
