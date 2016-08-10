#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Addr: 0x3F, 20 chars & 4 lines. Sometimes display boards use address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2); //Frentally display, use 0x3F if not working try 0x27
void lcdSetup()
{
  lcd.clear();
  lcd.init();
  delay(100);
  //lcd.noBacklight();
  lcdDisplayOn();
  lcd.setCursor(0, 0);
  firstLine = "";
  secondLine = "";
  thirdLine = "";
  fourthLine = "";
}

void lcdDisplayOn() {
  lcd.backlight();
  lcd.print(firstLine);
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
  lcd.setCursor(0, 2);
  lcd.blink();
}

void lcdDisplayOff() {
  lcd.clear();
  lcd.noBlink();
  lcd.noBacklight();
}

void lcdMessage(String first, String second, String third, String fourth) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(first);
  lcd.setCursor(0, 1);
  lcd.print(second);
  lcd.setCursor(0, 2);
  lcd.print(third);
  lcd.setCursor(0, 3);
  lcd.print(fourth);
}

void lcdMessageTwoLine(String first, String second) {
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(first);
  lcd.setCursor(0, 1);
  lcd.print(second);
}

