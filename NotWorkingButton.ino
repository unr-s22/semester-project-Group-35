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
  
  *DDR_E &= 0b11101111;  //should set bit 4 in port E(digital 2) to 0 (input with pullup) wihtout changing other values
  *PORT_E |= 0b00010000;  //should set state of bit 4 to high

  *DDR_E |= 0b00000010;  //should set bit 1 in port E to output without changing other values
  
  *myEICRB |= 0b00000010;  //these lines should make interrupt rely on FALLING action
  *myEICRB &= 0b11111110;

  *myEIMSK |= 0b00010000;  //this enables bit 4 in EIMSK (int4) as an interrupt

  *mySREG |= 0b10000000;  //this sets I bit of sreg to 1

  *myEIFR &= 0b00000000; //this should ensure the flag has low values initialized

void loop() {
  // put your main code here, to run repeatedly:
while(!previousStatus & currentStatus){  //checks to see if previous is 0 and current is 1 (ON state) then runs included code until interrupt switches the states
  *PORT_E |= 0b00000010;  //this is purely for testing with an LED
}
while(previousStatus & !currentStatus){  //checks to see if previous is 1 and current is 0 (OFF state) then runs included code until interrupt switches the states
  *PORT_E &= 0b11111101;  //this is purely for testing with an LED
}

}

ISR (INT4_vect){
  previousStatus = !previousStatus;  //the interrupt will flip the states previous/current to the opposite truth
  currentStatus = !currentStatus;
}
