#include <Stepper.h>
#define STEPS 100
Stepper stepper(STEPS, 8, 9, 10, 11);
int Old = 0;

void setup() {
  stepper.setSpeed(50);

}

void loop() {
  int New = analogRead(A0);
  stepper.step(New - Old);
  Old = New;

}
