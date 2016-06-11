
#include <SPI.h>
#include <MFRC522.h>

#include "FastLED.h"

//game constants
const int NUM_NODES = 3; //this game has 3 nodes
const long ALLOWED_TIME_FOR_PUZZLE = 60000; //user has this many milliseconds to solve the game, after which it returns to idle.
const int NUM_COLORS = 3; //TODO can I get this at runtime?
const int MAX_COLOR_LEN = 8; //maximum number of characters of any color 
const char COLORS[NUM_COLORS][MAX_COLOR_LEN] = {"RED", "GREEN", "BLUE"};//, "GREEN"}; 
const CRGB CRGB_COLORS[NUM_COLORS] = {CRGB::Red, CRGB::Green, CRGB::Blue };//, CRGB::Green};

//pin definitions
const int button_pins[NUM_NODES] = {2, 3, 4}; //holds the pin ids of the buttons
const int led_pins[NUM_NODES] = {5, 6, 7}; //holds the poins of the buttons 
const int LED_PIN_0 = 5;
const int LED_PIN_1 = 6;
const int LED_PIN_2 = 7;


//uid data locations:
#define light_tries_byte 2 
#define name_rfid_block 4 
#define data_rfid_block 5 

//LED definitions 
const int NUM_LEDS_BUFFER = 11;
const int NUM_LEDS_BOXES = 11;
const int NUM_LEDS = NUM_LEDS_BOXES + NUM_LEDS_BUFFER;
CRGB leds[NUM_NODES][NUM_LEDS]; //define array of leds.  one strip per node


//array to store user id.  sometimes the user_id will be null, signifying that the game is in the idle state.
byte nuid[4];
byte playerName[16]; //array to read player name.  not being used in this puzzle but needed to pass to the rfid reading function
byte playerData[16]; //array to read player data.  not being used in this puzzle but needed to pass to the rfid reading function

//array to store the current state of the puzzle. the outputs of game (lights)
//will be updated to match the values in this array using the 'publish_current_state'
//function below.
char puzzle_state[NUM_NODES][MAX_COLOR_LEN]; 
CRGB puzzle_state_crgb[NUM_NODES]; 

//holds the start time of the puzzle
unsigned long puzzle_start_time;

//global state setting functions.  the state is either 'idle' or 'active'.  The program determines
//if it's active by whether the first byte of the nuid == '\0'
void set_idle_state() {
  nuid[0] = '\0';
  for (int i=0; i<NUM_NODES; i++) {
    strcpy(puzzle_state[i], "");
    puzzle_state_crgb[i] = CRGB::Black;
    change_led_color(i, CRGB::Black, 0); 
    log_d("setting node %d to black", i);
  }  
  FastLED.show();
  
  log("idle state set");
}

void set_active_state() {
  for (int i=0; i<NUM_NODES; i++) {
    strcpy(puzzle_state[i], COLORS[0]);
    puzzle_state_crgb[i] = CRGB_COLORS[0];
  }
  puzzle_start_time = millis();
}


unsigned long elapsed_time() { 
  return millis() - puzzle_start_time; 
}

bool puzzle_is_solved() {
  for (int i=0; i<NUM_NODES; i++) { 
    if (strcmp(puzzle_state[i], COLORS[NUM_COLORS-1]) != 0) {
      return false;
    }
  }
  return true;
}

long last_debounce_times[NUM_NODES]; //store the last debounce times for each node 
int last_button_states[NUM_NODES]; //the last state read from the buttons.
int button_states[NUM_NODES];
const long DEBOUNCE_TIME = 50; 
/*
 * returns true if a node has been activated by the user.  this implementation uses digital buttons.
 */
bool is_node_activated(int button_index) {
  int button_state = digitalRead(button_pins[button_index]);
  //log_dd("button %d state read as %d", button_index, button_state); 

  // If the switch has changed state since last time we checked, record the time
  if (button_state != last_button_states[button_index]) {
    last_debounce_times[button_index] = millis();
  }

  bool registered_button_press = false; 
  long elapsed_time = millis() - last_debounce_times[button_index];
  if (elapsed_time > DEBOUNCE_TIME) {
    //ok, reading hasn't changed for longer than the debounce time, so it must 
    //not be noise.    
    
    // if the button state has changed:
    if (button_state != button_states[button_index]) {
      button_states[button_index] = button_state;
      
      //register button press if the new button state is HIGH
      if (button_state == HIGH) {
        registered_button_press = true;
        //log_d("button %d pressed", button_index);
      }
    }
  }

  //next time through the loop, the current button state is the last.
  last_button_states[button_index] = button_state;

  return registered_button_press;
}

/*
 * given that the node has been activated, update the puzzle state.  Algorithm is: whatever node is 
 * activated, the other nodes change by N colors, where N is the distance 
 */
void update_puzzle_state_for_node_activated(int node_id) {
  for (int i=0; i<NUM_NODES; i++) {  
    int distance = abs(node_id - i);
    if (distance > 0) {
      update_node_state(i, distance);     
    }
  }
}

/*
 * change the given node by the specified number of color states.   
 */
void update_node_state(int node_id, int num_colors_to_change_by) { 
   int cur_idx_of_color = get_index_of_color(puzzle_state[node_id]);
   int new_idx_of_color = (cur_idx_of_color + num_colors_to_change_by) % NUM_COLORS;
   strcpy(puzzle_state[node_id], COLORS[new_idx_of_color]);
   puzzle_state_crgb[node_id] = CRGB_COLORS[new_idx_of_color];
}

int get_index_of_color(char color[]) {
  for (int i=0; i<NUM_COLORS; i++) {
    if (strcmp(color, COLORS[i]) == 0) {
      return i;
    }
  }
  return -1;
}

void setup() {
  //rfid setup
  Serial.begin(9600); 

  rfidSetup();

  //button initialization
  for (int i=0; i<NUM_NODES; i++) {
    last_button_states[i] = LOW; //set initial button state to off.
    last_debounce_times[i] = millis();
    pinMode(button_pins[i], INPUT); //initialize button pins for reading
  } 

  //led setup
  FastLED.addLeds<WS2812B, LED_PIN_0, GRB>(leds[0], NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_1, GRB>(leds[1], NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_2, GRB>(leds[2], NUM_LEDS);
  
  set_idle_state();
  log("setup complete");
}

void loop() { //idle state 
  if (nuid[0] == '\0') {
    //read card user id logic here
    if ( rfidDetected(nuid, playerName, playerData, light_tries_byte) ) {
      set_active_state();
      publish_puzzle_started();  
      publish_current_puzzle_state();
    } 
  } else { //active state 
    if (!puzzle_is_solved()) {
      //did we time out?
      if (elapsed_time() > ALLOWED_TIME_FOR_PUZZLE) { 
        publish_puzzle_timed_out();  
        set_idle_state();  
      } 
  
      //is node button currently being pressed?
      for (int i=0; i<NUM_NODES; i++) {
        if (is_node_activated(i)) {
           update_puzzle_state_for_node_activated(i);
           publish_current_puzzle_state(); 
           break; //dont process any more buttons this round.
        }
      } 
    } else { //has the user solved the puzzle?  
      log_d("puzzle has been solved in %d seconds", elapsed_time()/1000);
      publish_puzzle_solved();  
      log("after puzzle solved"); 
      set_idle_state(); 
    } 
    closeRfid();
  }   
}




// ---- puzzle state publishing functions.  these should update the lights to reflect the current internal state  ---- 
void publish_puzzle_started() {
  //this might actually do nothing, or might just be the same as publish puzzle state
  long time = ALLOWED_TIME_FOR_PUZZLE / 1000;
  log_d("puzzle has been activated, user has %d seconds to solve it.", time);  
}

#define FRAMES_PER_SECOND 60
int success = 0;
int num_flash_blue = 6;
long num_success = 200;
void publish_puzzle_solved() { 
  for (int i=0; i<num_flash_blue; i++) { 
    for (int i=0; i<NUM_NODES; i++) {
      //flash on and off
      change_led_color(i, puzzle_state_crgb[i], 0); 
    }
    FastLED.show(); // display this frame
    delay(500);
    
    for (int i=0; i<NUM_NODES; i++) {
      //blink on and off briefly
      change_led_color(i, CRGB::Black, 0); 
    }
    FastLED.show(); // display this frame
    delay(500);
  }

  
  for (int i=0; i<num_success; i++) {
    for (int j=0; j<NUM_NODES; j++) {    
      fire2012(leds[j], 0, NUM_LEDS);
    } 
    
    FastLED.show(); // display this frame
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  }

  
  //log_d("puzzle has been solved in %d seconds", elapsed_time()/1000);
//  for (int i=0; i<num_success; i++) {  
//    for (int j=0; j<NUM_NODES; j++) {
//      fire2012(leds[j], 0, NUM_LEDS); 
//    }
//  }
  log("end of puzzle solved");
}

void publish_puzzle_timed_out() {
  log("timeout reached!");
}

void publish_current_puzzle_state() {
  log_partial("publishing current state: ");
  //just print out the current state of the game 
  for (int i=0; i<NUM_NODES; i++) {
    log_partial(puzzle_state[i]);
    log_partial(" ");
    change_led_color(i, puzzle_state_crgb[i], NUM_LEDS_BUFFER);
  }
  FastLED.show();
  log(""); // line break
} 

void change_led_color(int led_idx, CRGB color, int start_idx) {
  for (int i=start_idx; i<NUM_LEDS; i++) {
    leds[led_idx][i] = color;
  }
}

// --- logging functions- just wrappers around serial.print
void log(char msg[]) {
  //logs a message using Serial.println()
  Serial.println(msg);
}

void log_partial(char msg[]) {
  //logs a partial message using Serial.print()
  Serial.print(msg);
}

void log_d(char msg[], int var) {
  char buf[80];
  sprintf(buf, msg, var);
  log(buf);
}

void log_dd(char msg[], int var1, int var2) {
  char buf[80];
  sprintf(buf, msg, var1, var2);
  log(buf);
}

void log_ddd(char msg[], int var1, int var2, int var3) {
  char buf[80];
  sprintf(buf, msg, var1, var2, var3);
  log(buf);
}

void log_s(char msg[], char str[]) {
  char buf[80];
  sprintf(buf, msg, str);
  log(buf);
}


