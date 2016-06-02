#define Sprintln(a) (Serial.println(F(a)))
//#define Sprintln(a)

byte playerUid[4];  
byte playerName[16];
byte playerData[16];

//boolean rfidDetected(byte &id);

#define checkin_tries_byte 0
#define speech_tries_byte 1
#define light_tries_byte 2 
#define name_rfid_block 4 
#define data_rfid_block 5 

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

void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC
  rfidSetup();
}

void loop() {
  //state machine
  boolean status;

      if (rfidDetected(playerUid, playerName, playerData, light_tries_byte)) {
        //writeRfid(dataBlock, speech_rfid_block, 16);
        //readRfid(speechRfidData,speech_rfid_block,16);
        //speechRfidData[0]++;
        //writeRfid(speechRfidData, speech_rfid_block, 16);
        //dump_byte_array(speechRfidData, 16); Serial.println();
        //incrementBlockRfid(speech_tries_byte);

        //resetProfileRfid();
        delay(1000);
      }
      
      closeRfid();


  delay(100);              // wait for 100 ms
}
