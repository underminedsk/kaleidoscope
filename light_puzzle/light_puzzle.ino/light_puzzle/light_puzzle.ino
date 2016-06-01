
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

//set up RFID functionality
MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 

//game constants
#define NUM_NODES 3 //this game has 3 nodes
#define TIMEOUT 60 //user has this many seconds to solve the game, after which it returns to idle.
char COLORS[3][8] = {"RED", "ORANGE", "YELLOW"};

//array to store user id.  sometimes the user_id will be null, signifying that the game
//is in the idle state.
byte nuid[3]; 

//array to store the current state of the puzzle. the outputs of game (lights)
//will be updated to match the values in this array using the 'publish_current_state'
//function below.
char puzzle_state[NUM_NODES][6]; 

//global state setting functions.  the state is either 'idle' or 'active'.  The program determines
//if it's active by whether the first byte of the nuid == '\0'
void set_idle_state() {
  nuid[0] = '\0';
  for (int i=0; i<NUM_NODES; i++) {
    strcpy(puzzle_state[i], "");
  }
  log("idle state set");
}

void set_active_state() {
  for (int i=0; i<NUM_NODES; i++) {
    strcpy(puzzle_state[i], COLORS[0]);
  }
}

bool read_nuid_from_rfid_card() {
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return false;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return false;

  for (byte i = 0; i < 4; i++) {
      nuid[i] = rfid.uid.uidByte[i];
  }
  return true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  set_idle_state();
  log("setup complete");
}

void loop() { 
  if (nuid[0] == '\0') {
    //read card user id logic here
    if ( read_nuid_from_rfid_card() ) {
      set_active_state();
      publish_puzzle_started();  
      publish_current_puzzle_state();
    } 
  } else {
    //puzzle logic here
  }
  
}

void log(char msg[]) {
  Serial.println(msg);
}

void log_partial(char msg[]) {
  Serial.print(msg);
}

// ---- puzzle state publishing functions.  these should update the lights to reflect the current state  ---- 
void publish_puzzle_started() {
  //this might actually do nothing, or might just be the same as publish puzzle state
  log("puzzle has been activated");  
}

void publish_puzzle_solved() {
  log("puzzle has been solved");
}

void publish_puzzle_timed_out() {
  log("timeout reached!");
}

void publish_current_puzzle_state() {

  //just print out the current state of the game 
  for (int i=0; i<NUM_NODES; i++) {
    log_partial(puzzle_state[i]);
    log_partial(" ");
  }
  log("");
}



