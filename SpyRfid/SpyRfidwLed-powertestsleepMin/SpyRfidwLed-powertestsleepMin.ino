#include <Sleep_n0m1.h>
#include <SimpleTimer.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    A3
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    11
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

//#define Sprintln(a) (Serial.println(F(a)))
#define Sprintln(a)

byte playerUid[4];  
byte playerName[16];
byte playerData[16];

//boolean rfidDetected(byte &id);

#define checkin_tries_byte 0
#define speech_tries_byte 1
#define light_tries_byte 2 
#define name_rfid_block 4 
#define data_rfid_block 5
#define scanned_byte 15 

byte speechRfidData[16];
/*
    byte dataBlock[]    = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x08, 0x09, 0xff, 0x0b, //  9, 10, 255, 12,
        0x0c, 0x0d, 0x0e, 0x0f  // 13, 14,  15, 16
    };
*/
byte dataBlock[16];

//char asciiUid[8];
char *asciiUid;

String firstLine = "";
String secondLine = "";
String thirdLine = "";
String fourthLine = "";

Sleep sleep;
unsigned long sleepTime = 10000; //how long you want the arduino to sleep

#define RST_PIN         10 

SimpleTimer timer;

void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC
  rfidSetup();
  lcdSetup();

    // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  timer.setInterval(10000, sleepNow);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

int sleepcount = 0;  
int divider = 15; // once every 5 mins

void loop() {
  //state machine
  timer.run();
  boolean status;

    if (Serial.available()) {
      int val = Serial.read();
      if (val == 'S') {  
        sleepNow();
      }
      if (val == 'A') {
        Serial.println("Hola Caracola"); // classic dummy message
      }
    }

      if (rfidDetected(playerUid, playerName, playerData, scanned_byte)) {
        uidToHexAscii();
       
        //readRfid(speechRfidData,speech_rfid_block,16);
        //lcdMessageTwoLine("test", "testtest");
        writeRfid(speechRfidData, speech_tries_byte, 16);
        //dump_byte_array(speechRfidData, 16); Serial.println();
        dump_byte_array(playerUid, 4); Serial.println();

        //resetProfileRfid();
        delay(100);
      }
      
      closeRfid();

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  if (sleepcount % divider == 0) {
  
    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    FastLED.delay(1000/FRAMES_PER_SECOND); 
  }
  // insert a delay to keep the framerate modest
  

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically

  delay(10);              // wait for 100 ms
}

void sleepNow() {
      Serial.println("Serial: Entering Sleep mode");
      
      Serial.print("sleeping for ");
      Serial.println(sleepTime); 
      delay(100); //delay to allow serial to fully print before sleep
      digitalWrite(RST_PIN, LOW); 
      //lcdSetup();
      lcdDisplayOff();

      for( int i = 0; i < NUM_LEDS; i++) { //9948
        leds[i] = CRGB::Black;
      }
      FastLED.show(); 
       
      sleep.pwrDownMode(); //set sleep mode
      sleep.sleepDelay(sleepTime); //sleep for: sleepTime
      sleepcount += 1;
      
      
      rfidSetup(); //mfrc522.PCD_Init();

      if (sleepcount % divider == 0) {
        lcdSetup();
        lcdMessageTwoLine(String(sleepcount),"blabla");
      }
      Serial.print("done sleeping!");
}

void uidToHexAscii() {
  bin_to_strhex((unsigned char *)playerUid, sizeof(playerUid), &asciiUid);
  lcdMessageTwoLine(asciiUid, (char*)playerName);
  delay(1000);
  lcdSetup();
}


void bin_to_strhex(unsigned char *bin, unsigned int binsz, char **result)
{
  char          hex_str[]= "0123456789abcdef";
  unsigned int  i;

  *result = (char *)malloc(binsz * 2 + 1);
  (*result)[binsz * 2] = 0;

  if (!binsz)
    return;

  for (i = 0; i < binsz; i++)
    {
      (*result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
      (*result)[i * 2 + 1] = hex_str[(bin[i]     ) & 0x0F];
    }  
}


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


