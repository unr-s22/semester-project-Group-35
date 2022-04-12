//Colin Martires
//DC Motor Testing

//#define ENABLE 5
//#define DIRA 3

// define PORT E registers
volatile unsigned char* DDR_E = (unsigned char*) 0x2D;
volatile unsigned char* PORT_E = (unsigned char*) 0x2E;
volatile unsigned char* PIN_E = (unsigned char*) 0x2C;

int input;

void setup() {
  //pinMode(ENABLE, OUTPUT);
  //pinMode(DIRA, OUTPUT);

  //set pin 3 and 5 as OUTPUT
  *DDR_E |= B00111000;

  Serial.begin(9600);
}

void loop() {
  //digitalWrite(ENABLE, HIGH);
  //digitalWrite(ENABLE, LOW);
  //digitalWrite(DIRA, HIGH);

  if(Serial.available())
  {
    input = Serial.read();
    Serial.println(input);

    if(input > 50)
    {
      //set pin 5 to HIGH to ENABLE fan
  *PORT_E = (1 << 5) | *PORT_E;

      //set pin 3 to HIGH to spin fan
  *PORT_E = (1 << 3) | *PORT_E;
      //*PORT_E = ~(1 << 4) & *PORT_E;
    }
    else
    {
      *PORT_E = ~(1 << 5) & *PORT_E;
    }
  }

  //set pin 3 to LOW because it has to be low?
  //*PORT_E &= B11110111;

  //Serial.println("fan should be spinning");
}


