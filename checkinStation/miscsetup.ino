#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Addr: 0x3F, 20 chars & 4 lines. Sometimes display boards use address 0x27
LiquidCrystal_I2C lcd(0x3F, 4, 20); //Frentally display, use 0x3F if not working try 0x27
void lcdSetup()
{
  lcd.init();
  //lcd.begin(20,4);
  delay(100);
  lcd.noBacklight();
  //lcd.backlight();
  lcd.setCursor(0, 0);
  //lcd.print("X=");
}



boolean nameEntered() {
  
  Serial.println("entering name");
  lcd.backlight();
  lcd.print(firstLine);
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
  lcd.setCursor(0, 2);
  lcd.blink();

  while (nrfData[0] != 13) {
    waitForRFdata(nrfData, 1);
    //lcd.write(nrfData[0]);
    //Serial.println(nrfData[0]);
    updateLcd(nrfData[0]);
  }
  Serial.write(nrfData[0]);


  return true;
}

void updateLcd(byte val) {
  switch (val) {
    case 8: // Backspace
      if (i > 0) {
        i--;
        playerName[i] = 0;
      }
      break;

    case 13: // Enter
      firstLine = "Your name is:";
      secondLine = "";
      lcd.noBlink();
      break;

    default:
      if (val >= 32 && val <= 122 && i < 16) {
        playerName[i] = val;
        i++;
      }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
  lcd.setCursor(0, 2);
  lcd.print((char*)playerName);
}

