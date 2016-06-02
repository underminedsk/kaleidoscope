#include <SPI.h>
#include "RF24.h"
#include <IRremote.h>

//#define Sprintln(a) (Serial.println(a))
#define Sprintln(a)

#define WAIT_FOR_TAG 0
#define CHECK_ID 1
#define SEND_RESPONSE 2
#define ENTER_NAME 3
#define WAIT_FOR_WRITE_TAG 4
#define PLAY_VIDEO 5
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
}

void loop()
{ 
  //state machine
  switch (checkin_fsm_state)
  {
    case WAIT_FOR_TAG:  //Starting state to wait for RFID tag

      if (receivedRFData(playerPacket, playerPacketSize)) {
        parsePlayerPacket(playerUid, playerName, playerData, playerPacket);
        //Serial.print(playerPacketSize, DEC); Serial.print('\n');
        //dump_byte_array(playerPacket, playerPacketSize);
        Serial.write(playerPacket, playerPacketSize); Serial.print('\n');     
        checkin_fsm_state = CHECK_ID;       
      }
      
    break;

    case CHECK_ID:
      
      if (checkForNewPlayer(isNewPlayer)) {
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
          Serial.print("PLAY_VIDEO"); Serial.print('\n');
          checkin_fsm_state = PLAY_VIDEO;   
        }
      }  
      break;
        

    case ENTER_NAME:
      if (nameEntered()) {
        checkin_fsm_state = WAIT_FOR_WRITE_TAG;
        
      }  
      break;

    case WAIT_FOR_WRITE_TAG:
      if (writeTagDone()) {
        Serial.print("PLAY_VIDEO"); Serial.print('\n');
        checkin_fsm_state = PLAY_VIDEO;
      }
      break;

    case PLAY_VIDEO:
      //Serial.print("PLAY_VIDEO"); Serial.print('\n');
      if (videoDone()) {
        checkin_fsm_state = WAIT_FOR_TAG;
      }
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
    case WAIT_FOR_WRITE_TAG: Sprintln("WAIT_FOR_WRITE_TAG"); break;
    case PLAY_VIDEO: Sprintln("PLAY_VIDEO"); break;
    default: Sprintln("Unknown State"); break;   
  }  
}
