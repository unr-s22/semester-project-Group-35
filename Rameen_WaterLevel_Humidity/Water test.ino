int resval = 0;  // holds the value
int respin = A5; // sensor pin used

void setup() {

  // start the serial console
  Serial.begin(9600);
}

void loop() {

  resval = analogRead(respin); //Read data from analog pin and store it to resval variable
Serial.println(resval);
  delay(1000);
}
// 0-350 no water/ low
// 350-440 medium water
//420-450 HIGH