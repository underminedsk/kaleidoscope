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
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};
byte rowPins[ROWS] = {A1, 0, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad 
Keypad customKeypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void keypadSetup()
{
  lcd.setCursor(0,0);
  lcd.print("Enter Password");

  customKey = customKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {
    Data[data_count] = customKey; // store char into data array
    lcd.setCursor(data_count,1); // move cursor to show each new char
    lcd.print(Data[data_count]); // print char at said cursor
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
  }

  if(data_count == Password_Lenght-1) // if the array index is equal to the number of expected chars, compare data to master
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password is ");

    if(!strcmp(Data, Master)) {// equal to (strcmp(Data, Master) == 0)
      lcd.print("Good");
      gameDone = true; 
    }
    else
      lcd.print("Bad");

    delay(1000);// added 1 second delay to make sure the password is completely shown on screen before it gets cleared.
    lcd.clear();
    clearData();   
  }
}

void clearData()
{
  while(data_count !=0)
  {   // This can be used for any array size, 
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}
