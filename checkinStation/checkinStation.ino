//Timer FSM numbers
uint16_t msCounts = 0;
uint16_t sCounts = 0;

#define CHECK_IN 0
#define ENTER_NAME 1
#define WRITE_TAG 2
#define PLAY_VIDEO 3
uint8_t checkin_fsm_state = CHECK_IN;


#include "RF24.h"

RF24 radio(7,8);                        // Set up nRF24L01 radio on SPI bus plus pins 7 & 8
const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };   // Radio pipe addresses for the 2 nodes to communicate.

byte data[32];                           //Data buffer for testing data transfer speeds
byte nrfData[40];

byte playerName[16];
int i = 0;
String firstLine = "Hello";
String secondLine = "What's you name?";

byte playerUid[4];  

boolean rfidDetected(byte &id);

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
    case CHECK_IN:  //Starting state to wait for RFID tag
      
      if (rfidDetected(playerUid, sizeof(playerUid))) {
        // check rfid card exists in database
        dump_byte_array(playerUid, sizeof(playerUid)); Serial.println();
        
        if (isNewPlayer(playerUid, sizeof(playerUid))) {
          checkin_fsm_state = ENTER_NAME;
        }
        else {
          // existing rfid card
          checkin_fsm_state = PLAY_VIDEO;
          Serial.println("entering state PLAY_VIDEO");
        }
      }



      break;

    case ENTER_NAME:   
      // Turn on LCD and ask for name 
        

      if (nameEntered()) {
        checkin_fsm_state = WRITE_TAG;
        dump_byte_array(playerName, sizeof(playerName)); Serial.println("wee");
      }
      break;

    case WRITE_TAG:
      // Display name and ask to retag

      if (rfidDetected(playerUid, sizeof(playerUid)) || timeOut()) {
        writeRfid(playerName, 4, 16);
        checkin_fsm_state = PLAY_VIDEO;
        Serial.println("Leaving WRITE_TAG, entering PLAY_VIDEO");
      }
      break;

    case PLAY_VIDEO:
      // Play video

      if (videoDone()) {
        checkin_fsm_state = CHECK_IN;
        Serial.println("Video Done!");
      }
      break;

    default:
      break;
  }

  delay(100);              // wait for 100 ms
}
