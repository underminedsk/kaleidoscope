#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Addr: 0x3F, 20 chars & 4 lines. Sometimes display boards use address 0x27
LiquidCrystal_I2C lcd(0x3F, 4, 20); //Frentally display, use 0x3F if not working try 0x27
void lcdSetup()
{
  lcd.clear();
  lcd.init();
  delay(100);
  lcd.noBacklight();
  lcd.setCursor(0, 0);
  firstLine = "Hello";
  secondLine = "What's your name?";
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



boolean nameEntered() {

  byte letter[1];
  if (receivedRFData(letter,1)) {
    updateLcd(letter[0], letterNum);
    Serial.write(letter[0]);

    if (letter[0]==13) { 
      return true;
    }
  }
  return false;
}

void updateLcd(byte val, byte &letterNum) {
  switch (val) {
    case 8: // Backspace
      if (letterNum > 0) {
        letterNum--;
        playerName[letterNum] = 0;
        thirdLine = (char*)playerName;
      }
      break;

    case 13: // Enter
      firstLine = "Your name is:";
      secondLine = (char*)playerName;
      thirdLine = "";
      fourthLine = "Please retag card";
      lcd.noBlink();
      break;

    default:
      if (val >= 32 && val <= 122 && letterNum < 15) {
        playerName[letterNum] = val;
        letterNum++;
        
      }
      thirdLine = (char*)playerName;
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
  lcd.setCursor(0, 2);
  lcd.print(thirdLine);
  lcd.setCursor(0, 3);
  lcd.print(fourthLine);
  lcd.setCursor(letterNum, 2);
}

