#include <Stepper.h>
#define STEPS 64
Stepper stebber = Stepper(STEPS, 8, 9, 10, 11);
int OldVal = 0;
signed int Back = -128;
signed int Forward = 128;


void setup() {
  stebber.setSpeed(200);
  pinMode(A0, INPUT);
  //Serial.begin(9600);
  OldVal = analogRead(A0);
}

void loop() {
  int NewVal = analogRead(A0);
  //Serial.print(New);
  //Serial.print(" New");    for testing
  //Serial.print("\n");
  
  if(NewVal > OldVal + 25){    //adding 25 allows for pot corrections
    stebber.step(Forward);
    //Serial.print("NEW >");
    //Serial.print("\n");      for testing
  }
  else if(NewVal + 25 < OldVal){   //adding 25 allows for pot corrections
    stebber.step(Back);
    
    //Serial.print("OLD > ");
    //Serial.print("\n");
  }
  else{
    
  }
  OldVal = NewVal;
  //Serial.print(Old);
  //Serial.print(" Old");
  //Serial.print("\n");
  
} 
