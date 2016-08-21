#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Addr: 0x3F, 20 chars & 4 lines. Sometimes display boards use address 0x27
LiquidCrystal_I2C lcd(0x3F, 16, 2); //Frentally display, use 0x3F if not working try 0x27

#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void playTone() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(A2, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(A2);
  }
}

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
  //lcd.blink();
}



boolean nameEntered() {

  byte letter[1];

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

void lcdMessage(String first, String second) {
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print(first);
  lcd.setCursor(0, 1);
  lcd.print(second);
}

#include <Keypad.h>

#define Password_Lenght 7 // Give enough room for six chars + NULL char

char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght] = "123456"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'.','0','=','/'}
};
byte rowPins[ROWS] = {A1, 0, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad 
Keypad customKeypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void clearData()
{
  while(data_count !=0)
  {   // This can be used for any array size, 
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}

void keypadSetup()
{
  lcd.setCursor(0,0);
  //lcd.print("Enter Password");
  //lcdMessage(String((char*)playerUid), String((char*)playerData));
  //lcd.clear();
  /*  
  lcd.setCursor(0, 0);
  for (int i=0; i < sizeof(playerUid); i++) {
    lcd.print(playerUid[i], DEC);
  }
  lcd.setCursor(0, 1);
  lcd.print(String((char*)playerName));
  */
  lcd.setCursor(0, 0);
  for (int i=0; i < sizeof(playerData); i++) {
    lcd.print(playerData[i], HEX);
  }
  lcd.setCursor(0, 1);
  lcd.print("past+ pres- done=");
  
  //lcd.print(playerData[0], DEC);

  customKey = customKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {
    /*Data[data_count] = customKey; // store char into data array
    lcd.setCursor(data_count,1); // move cursor to show each new char
    lcd.print(Data[data_count]); // print char at said cursor
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
    */
    switch (customKey)
    {
      case '1':
        playerData[0]++;
        playerData[1]++;
        break;
      case '2':
        playerData[2]++;
        playerData[3]++;
        break;
      case '3':
        playerData[4]++;
        playerData[5]++;
        break;
      case '4':
        playerData[6]++;
        playerData[7]++;
        break;
      case '5':
        playerData[8]++;
        playerData[9]++;
        break;
      case '6':
        playerData[10]++;
        playerData[11]++;
        break;
      case '7':
        playerData[12]++;
        playerData[13]++;
        break;          
      case '+':
        playerData[14]++;
        break;
      case '-':
        playerData[15]++;
        break;  
      case '=':
        gameDone = true; 
        break;      
      default:
        break;
    }      
  }

}


