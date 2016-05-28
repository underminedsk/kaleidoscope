#define Sprintln(a) (Serial.println(a))
//#define Sprintln(a)

#define WAIT_FOR_TAG 0
#define WAIT_FOR_RESPONSE 1
#define ENTER_NAME 2
#define WRITE_TAG 3
#define PLAY_VIDEO 4
uint8_t checkin_fsm_state = WAIT_FOR_TAG;

#define checkin_tries_byte 0
#define speech_tries_byte 1
#define light_tries_byte 2 
#define name_rfid_block 4 
#define data_rfid_block 5 

#include "RF24.h"

RF24 radio(7,8);                        // Set up nRF24L01 radio on SPI bus plus pins 7 & 8
const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };   

byte data[32];                           //Data buffer for testing data transfer speeds
byte nrfData[40];

byte letterNum;


String firstLine = "Hello";
String secondLine = "What's your name?";
String thirdLine = "";
String fourthLine = "";

//boolean rfidDetected(byte uid[], byte playerName[], byte playerData[]);

byte playerUid[4];  
byte playerName[16];
byte playerData[16];
const int playerPacketSize = sizeof(playerUid) + sizeof(playerName) + sizeof(playerData);
byte playerPacket[playerPacketSize];

void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC
  rfidSetup();
  nrfSetup();
  lcdSetup();
}

void loop() {
  //state machine
  switch (checkin_fsm_state)
  {
    case WAIT_FOR_TAG:  //Starting state to wait for RFID tag
      
      if (rfidDetected(playerUid, playerName, playerData) || timeOut()) {
        // check rfid card exists in database

        // increment check in tries on rfid tag
        if (!incrementBlockRfid(checkin_tries_byte)) {break;} 
        closeRfid();       
        Sprintln("inc successful");
        
        createPlayerPacket(playerUid, playerName, playerData, playerPacket);
                 
        if (sendRFData(playerPacket, playerPacketSize)) {
          checkin_fsm_state = WAIT_FOR_RESPONSE;
          Sprintln("wrote rf uid");
        }
      }

      break;

    case WAIT_FOR_RESPONSE:
      boolean isNewPlayer;
      
      if (checkForNewPlayer(isNewPlayer)) {
          if (isNewPlayer) {
            memset(playerName, 0, sizeof(playerName));
            lcdDisplayOn();
            letterNum = 0;
            checkin_fsm_state = ENTER_NAME;
          } else {
            checkin_fsm_state = PLAY_VIDEO;  
          }
        }

      break;  

    case ENTER_NAME:   
      
      if (nameEntered()) {
        checkin_fsm_state = WRITE_TAG;
      }
      break;

    case WRITE_TAG:
      // Display name and ask to retag
      byte oldPlayerName[16];

      if (rfidDetected(playerUid, oldPlayerName, playerData) || timeOut()) {
        if (!writeRfid(playerName, name_rfid_block, 16)) {break;} 
        closeRfid();
        lcdSetup();
        memset(playerName, 0, sizeof(playerName)); //reset name
        checkin_fsm_state = PLAY_VIDEO;
      }
      break;

    case PLAY_VIDEO:
      // Play video

      if (videoDone()) {
        checkin_fsm_state = WAIT_FOR_TAG;
        Sprintln("Video Done!");
        delay(3000);
      }
      break;

    default:
      break;
  }
  printState(); //Sprintln(checkin_fsm_state);
  delay(500);              // wait for 100 ms
}

void printState()
{
  switch (checkin_fsm_state) {
    case WAIT_FOR_TAG: Sprintln("WAIT_FOR_TAG"); break;
    case WAIT_FOR_RESPONSE: Sprintln("WAIT_FOR_RESPONSE"); break;
    case ENTER_NAME: Sprintln("ENTER_NAME"); break;
    case WRITE_TAG: Sprintln("WRITE_TAG"); break;
    case PLAY_VIDEO: Sprintln("PLAY_VIDEO"); break;
    default: Sprintln("Unknown State"); break;   
  }  
}

void createPlayerPacket(byte playerUid[], byte playerName[], byte playerData[], byte combined[]) {
  memcpy(combined, playerUid, sizeof(playerUid));
  memcpy(combined+sizeof(playerUid), playerName, sizeof(playerName));
  memcpy(combined+sizeof(playerUid)+sizeof(playerName), playerData, sizeof(playerData));
}

