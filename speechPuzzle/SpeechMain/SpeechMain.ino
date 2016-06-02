//#define Sprintln(a) (Serial.println(F(a)))
#define Sprintln(a)

#include <stdlib.h>
#include <string.h>
#include <SimpleTimer.h>
SimpleTimer timer;
int timerId = 0;
int timeOutMs = 32000;  // Can't seem to make it much higher than his, maybe timeout overflow?

#include "MOVIShield.h"     // Include MOVI library, needs to be *before* the next #include

#ifdef ARDUINO_ARCH_AVR 
#include <SoftwareSerial.h> // This is nice and flexible but only supported on AVR architecture, other boards need to use Serial1 
#include <avr/pgmspace.h>
#endif
/*
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(11, PIN, NEO_GRB + NEO_KHZ800);
*/
#include <FastLED.h>

#define LED_PIN     8
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    11

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];


//MOVI recognizer(true);      // Get a MOVI object, true enables serial monitor interface, rx and tx for alternate communication pins on AVR architecture boards
MOVI recognizer(true, 3, 2);


/* //Short tests
const String  speechQuestions[]={ // Eliza's responses
  "Talk to me.", 
  "What do you want to talk about?", 
  "Anything specific?",
  "Test test"
}; 
*/

const char string_0a[] PROGMEM = "I may wax, but never wane. Wearing jewelry is a pain!"; 
const char string_0b[] PROGMEM = "My drum resounds for you to hear, Add a wig, in creeps the fear.";
const char string_1a[] PROGMEM = "There is a certain crime, that if it is attempted, is punishable.";
const char string_1b[] PROGMEM = "But if it is committed, is not punishable. What is the crime?";
const char string_2a[] PROGMEM = "As riddles go, I'm quite a stretch. My dog lies down and won't play fetch."; 
const char string_2b[] PROGMEM = "Camel won't spit, crow won't fly. Bent out of shape? Give me a try.";
//const char string_2[] PROGMEM = "As riddles go"; 
const char success[] PROGMEM = "That is correct! You are a logical human being. Please return to check in station for further instructions.";
const char failure[] PROGMEM = "Too many ill,logical answers. Practice science and return.";

const char* const string_table[] PROGMEM = {string_0a, string_0b, string_1a, string_1b, string_2a, string_2b, success, failure};
char buffer[150];

#define WAIT_FOR_TAG 0 
#define START_PUZZLE 1
#define END_PUZZLE 2

#define speech_tries_byte 1
#define speech_done_byte 5
#define speech_puzzlenum_byte 9 
#define name_rfid_block 4 
#define data_rfid_block 5 

uint8_t speech_fsm_state = WAIT_FOR_TAG;
int speechPuzzleNum;
int numTries;

byte playerUid[4];  
byte playerName[16];
byte playerData[16];

void setup() // setup Arduino and MOVI
{
  ledSetup();
  rfidSetup();
  recognizer.init();      // Initialize MOVI (waits for it to boot)

  //*
  // Note: training can only be performed in setup(). 
  // The training functions are "lazy" and only do something if there are changes. 
  // They can be commented out to save memory and startup time once training has been performed.
  recognizer.callSign("COMPUTER"); // Train callsign Arduino (may take 20 seconds)

  recognizer.addSentence(F("Ear")); // Add sentence 1
  recognizer.addSentence(F("Suicide"));  // Add sentence 2
  recognizer.addSentence(F("Yoga"));  // Add sentence 3

    // Add the top-50 most frequent English words as 'background model'.
  // This way, the keyword is not triggered by other random words.
  // See MOVI's user manual for a more detailed explanation on this.
  recognizer.addSentence(F("the be to of and a"));
  recognizer.addSentence(F("in that have I it for"));
  recognizer.addSentence(F("not on with he as you"));
  recognizer.addSentence(F("do at this but his by"));
  recognizer.addSentence(F("from they we say her she"));
  recognizer.addSentence(F("or an will my one all would"));
  recognizer.addSentence(F("there their what so up out"));
  recognizer.addSentence(F("if about who get which go me"));
  recognizer.train();                           // Train (takes about 20seconds)
  //*/
  recognizer.welcomeMessage(false); 
  recognizer.setThreshold(15);                 
  recognizer.responses(false); // turn of automatic responses (so we can react).
  recognizer.beeps(false);
  recognizer.setVolume(100);
  recognizer.setVoiceGender(MALE_VOICE);
  //recognizer.stopDialog();

}

void loop() // run over and over
{
  timer.run();
  
  int res=recognizer.poll();
  
  switch (speech_fsm_state)
  {
    case WAIT_FOR_TAG:  //Starting state to wait for RFID tag
      timer.disable(timerId);
      if (rfidDetected(playerUid, playerName, playerData, speech_tries_byte)) {
        String welcomeMsg  = "Welcome," + String((char*)playerName); 
               
        speechPuzzleNum = playerData[speech_puzzlenum_byte];    
        //speechPuzzleNum = 2; //random(0,4); //TEMP
        //recognizer.restartDialog();

        recognizer.say(welcomeMsg);
        recognizer.say(F("Here's your puzzle."));
        
        if (speechPuzzleNum==0) {
          recognizer.say(F("You are not ready yet."));
          delay(1000);
          break;  
        } else {
          delay(2000);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table[speechPuzzleNum*2-2])));          
          recognizer.ask(buffer);
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table[speechPuzzleNum*2-1])));          
          recognizer.ask(buffer);   
          //recognizer.ask(speechQuestions[speechPuzzleNum]);  
        }
        numTries = 0;
        
        //timer.enable(0);
        timer.restartTimer(timerId);
        timerId = timer.setTimeout(timeOutMs, timedOut);
        timer.enable(timerId);
        speech_fsm_state = START_PUZZLE;
      } 
      closeRfid();
      break;

    case START_PUZZLE:
      
      if (res==speechPuzzleNum) {
        strcpy_P(buffer, (char*)pgm_read_word(&(string_table[6])));
        recognizer.say(buffer);
        //recognizer.say(F("That is correct! You are a logical human being. Please return to check in station for further instructions.")); 
        timer.disable(timerId);
        timerId = timer.setTimeout(10000, ledDone);
        speech_fsm_state = END_PUZZLE;
      }
      if (res==UNKNOWN_SENTENCE || (res>0 && res!=speechPuzzleNum)) { 
        recognizer.ask(F("Wrong answer."));
        numTries++;
      }
      if (res==SILENCE) { // If nothing is said (e.g. noise only) then we ask again.
        recognizer.ask(F(""));
      }

      if (numTries > 4) {
        strcpy_P(buffer, (char*)pgm_read_word(&(string_table[7])));
        recognizer.say(buffer);
        //recognizer.say(F("Too many ill,logical answers. Practice science and return.")); 
        speech_fsm_state = WAIT_FOR_TAG;
      }
      break;  

    case END_PUZZLE:    
        
      //FastLED.show();  
        // insert a delay to keep the framerate modest
      //FastLED.delay(1000/FRAMES_PER_SECOND);
      
      Fire2012(); // run simulation frame
      FastLED.show(); // display this frame
      FastLED.delay(1000 / FRAMES_PER_SECOND); 
      break;  

    default:
      break;  
  
  }
}

void ledDone() {
  resetLed();
  speech_fsm_state = WAIT_FOR_TAG;
}

void timedOut() {
  Sprintln("timed out!");
  recognizer.restartDialog();
  delay(1000);
  recognizer.say(F("Time up. Logging out."));
  //timer.disable(0);
  speech_fsm_state = WAIT_FOR_TAG;
}


