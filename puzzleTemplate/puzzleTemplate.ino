#define Sprintln(a) (Serial.println(F(a)))
//#define Sprintln(a)

#define WAIT_FOR_TAG 0
#define WAIT_FOR_GAME_DONE 1
#define GAME_DONE 2
#define WRITE_TAG 3
#define PLAY_VIDEO 4
uint8_t checkin_fsm_state = WAIT_FOR_TAG;
uint8_t puzzle_state = 0;

#define checkin_tries_byte 0
#define speech_tries_byte 1
#define light_tries_byte 2 
#define checkin_done_byte 4
#define speech_done_byte 5
#define speech_puzzlenum_byte 9 
#define name_rfid_block 4 
#define data_rfid_block 5 

#include <FastLED.h>

#define LED_PIN     A3
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    11

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 100
CRGB leds[NUM_LEDS];
bool gReverseDirection = false;



byte letterNum;


String firstLine = "Hello seeker";
String secondLine = "What is your name?";
String thirdLine = "";
String fourthLine = "";

//boolean rfidDetected(byte uid[], byte playerName[], byte playerData[]);

byte playerUid[4];  
byte playerName[16];
byte playerData[16];
const int playerPacketSize = sizeof(playerUid) + sizeof(playerName) + sizeof(playerData);
byte playerPacket[playerPacketSize];

boolean gameDone = false;

void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC
  rfidSetup();
  lcdSetup();
  ledSetup();
}

void loop() {
  //state machine
  switch (checkin_fsm_state)
  {
    case WAIT_FOR_TAG:  //Starting state to wait for RFID tag
      Fire2012(); // run simulation frame
      FastLED.show(); // display this frame
      FastLED.delay(1000 / FRAMES_PER_SECOND); 
      
      if (rfidDetected(playerUid, playerName, playerData, checkin_tries_byte)) {
        //String welcomeMsg  = "Welcome," + String((char*)playerName); 
        
        // increment check in tries on rfid tag
        //if (!incrementBlockRfid(checkin_tries_byte)) {break;} 
        gameDone = false; 
        lcdDisplayOn();
        lcdMessage("Welcome", String((char*)playerName));
        Sprintln("inc successful");
        checkin_fsm_state = WAIT_FOR_GAME_DONE;
        //dump_byte_array(playerPacket, playerPacketSize);          

      } else {
        closeRfid();
      }  
      break;

    case WAIT_FOR_GAME_DONE:
      updateGameState();
      
      if (gameDone == true) {
        checkin_fsm_state = GAME_DONE;
      }
      break;  



    case GAME_DONE:
      // Display name and ask to retag
      byte oldPlayerName[16];
      byte oldPlayerData[16];
      byte writeTagDone[1];
      lcdMessage("Please retag","your card");

      if (rfidDetected(playerUid, oldPlayerName, oldPlayerData, checkin_done_byte)) {
        //if (!writeRfid(playerData, data_rfid_block, 16)) {break;}

        
        
        lcdSetup();
        checkin_fsm_state = WAIT_FOR_TAG;
      } 
      closeRfid();
      break;

    default:
      break;
  }
  printState(); //Sprintln(checkin_fsm_state);
  delay(100);              // wait for 100 ms
}

void printState()
{
  switch (checkin_fsm_state) {
    case WAIT_FOR_TAG: Sprintln("WAIT_FOR_TAG"); break;
    case WAIT_FOR_GAME_DONE: Sprintln("WAIT_FOR_GAME_DONE"); break;
    case GAME_DONE: Sprintln("GAME_DONE"); break;
    case WRITE_TAG: Sprintln("WRITE_TAG"); break;
    case PLAY_VIDEO: Sprintln("PLAY_VIDEO"); break;
    default: Sprintln("Unknown State"); break;   
  }  
}

void createPlayerPacket(byte playerUid[], byte playerName[], byte playerData[], byte combined[]) {
  // for some reason, using sizeof(variable) in memcpy messes up
  memcpy(combined, playerUid, 4);
  memcpy(combined+4, playerName, 16);
  memcpy(combined+20, playerData, 16);
}

