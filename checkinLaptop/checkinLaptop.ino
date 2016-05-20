#include <SPI.h>
#include "RF24.h"

#define WAIT_FOR_TAG 0
#define CHECK_NAME 1
#define ENTER_NAME 2
#define PLAY_VIDEO 3
uint8_t checkin_fsm_state = WAIT_FOR_TAG;


RF24 radio(7,8);  

char playerName[16];
int i=0;
String firstLine = "Enter your name:";

byte playerUid[4] = {0,0,0,0};
byte nrfData[40];

void setup() {
  nrfSetup();
  Serial.println("bla bla bla");
}

void loop()
{ 
  //state machine
  switch (checkin_fsm_state)
  {
    case WAIT_FOR_TAG:  //Starting state to wait for RFID tag

      waitForRFdata(playerUid, sizeof(playerUid));
dump_byte_array(playerUid, sizeof(playerUid)); Serial.println();
      
      if (isNewPlayer(playerUid, sizeof(playerUid))) {
        checkin_fsm_state = ENTER_NAME;
      } 
      else {
        checkin_fsm_state = PLAY_VIDEO;  
      }
      break;

    case ENTER_NAME:
      if (nameEntered()) {
        checkin_fsm_state = PLAY_VIDEO;
      }  
      break;

    case PLAY_VIDEO:
      checkin_fsm_state = WAIT_FOR_TAG;
      break;

    default: 
      break; 
      
  }


  /* 
  if (radio.available()) {   
    Serial.println("reading radio");    
    radio.read(&playerUid,4);
    dump_byte_array(playerUid, sizeof(playerUid)); Serial.println();

    

    
    //radio.startListening();
    Serial.println("sent radio");  

    for (byte i=0; i < 5; i++) { 
      delay(2000);
      nrfData[i] = 97+i; 
      radio.write(&nrfData[i],1);
    }  
  }
  /*
    
 /*   lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(firstLine);
    lcd.setCursor(0, 1);
    lcd.write(playerName);*/
    
  
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
