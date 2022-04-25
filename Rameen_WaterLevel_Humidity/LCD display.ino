#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int resval = 0;  // holds the value
int respin = A5; // sensor pin used

void setup() {

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  
}

void loop() {
  lcd.print("WATER LEVEL: ");
  // set the cursor to column 0, line 1
  lcd.setCursor(0, 1);

  resval = analogRead(respin); //Read data from analog pin and store it to resval variable
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
