#include <SPI.h>
#include "RF24.h"

//#define Sprintln(a) (Serial.println(a))
#define Sprintln(a)

#define WAIT_FOR_TAG 0
#define CHECK_ID 1
#define SEND_RESPONSE 2
#define ENTER_NAME 3
#define PLAY_VIDEO 4
uint8_t checkin_fsm_state = WAIT_FOR_TAG;


RF24 radio(7,8);  

//char playerName[16];
int i=0;
String firstLine = "Enter your name:";

byte playerUid[4];  
byte playerName[16];
byte playerData[16];
const int playerPacketSize = sizeof(playerUid) + sizeof(playerName) + sizeof(playerData);
byte playerPacket[playerPacketSize];

boolean isNewPlayer;

void setup() {
  nrfSetup();
  Sprintln("bla bla bla");
}

void loop()
{ 
  //state machine
  switch (checkin_fsm_state)
  {
    case WAIT_FOR_TAG:  //Starting state to wait for RFID tag

      if (receivedRFData(playerPacket, playerPacketSize)) {
        dump_byte_array(playerPacket, playerPacketSize); Sprintln();
        parsePlayerPacket(playerUid, playerName, playerData, playerPacket);      
        checkin_fsm_state = CHECK_ID;       
      }
      
    break;

    case CHECK_ID:
      
      if (checkForNewPlayer(playerUid, isNewPlayer)) {
        checkin_fsm_state = SEND_RESPONSE; 
      }
      break;
      
    case SEND_RESPONSE:
      byte response[1];
      
      if (isNewPlayer) {
        response[0] = 'Y';
        if (sendRFData(response, 1)) {
          checkin_fsm_state = ENTER_NAME;
        } 
      }
      else {
        response[0] = 'N';
        if (sendRFData(response, 1)) {
          checkin_fsm_state = PLAY_VIDEO;   
        }
      }  
      break;
        

    case ENTER_NAME:
      if (nameEntered()) {
        checkin_fsm_state = PLAY_VIDEO;
      }  
      break;

    case PLAY_VIDEO:
      checkin_fsm_state = WAIT_FOR_TAG;
      delay(3000);
      break;

    default: 
      break; 
      
  }

  printState();  //Sprintln(checkin_fsm_state);
  delay(100);              // wait for 100 ms
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void parsePlayerPacket(byte playerUid[], byte playerName[], byte playerData[], byte combined[]) {
  //subset = malloc((arraySize-i)*sizeof(int)); //Where i is the place you want to start your subset. 
  int offset;
  offset = 0;
  for(int j=0;j<sizeof(playerUid);j++)
    playerUid[j] = combined[j];
  offset = 4;  
  for(int j=0;j<sizeof(playerName);j++)
    playerName[j] = combined[offset + j];
  offset = 20;  
  for(int j=0;j<sizeof(playerData);j++)
    playerData[j] = combined[offset + j];  
}

void printState()
{
  switch (checkin_fsm_state) {
    case WAIT_FOR_TAG: Sprintln("WAIT_FOR_TAG"); break;
    case CHECK_ID: Sprintln("CHECK_ID"); break;
    case SEND_RESPONSE: Sprintln("SEND_RESPONSE"); break;
    case ENTER_NAME: Sprintln("ENTER_NAME"); break;
    case PLAY_VIDEO: Sprintln("PLAY_VIDEO"); break;
    default: Sprintln("Unknown State"); break;   
  }  
}
