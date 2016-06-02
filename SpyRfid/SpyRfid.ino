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

void setup()
{
  Serial.begin(9600); // Initialize serial communications with the PC
  rfidSetup();
  lcdSetup();
}

void loop() {
  //state machine
  boolean status;
  itoa(playerUid[0], asciiUid ,16);
      if (rfidDetected(playerUid, playerName, playerData, scanned_byte)) {
        uidToHexAscii();
       
        //readRfid(speechRfidData,speech_rfid_block,16);
        //lcdMessageTwoLine("test", "testtest");
        //writeRfid(speechRfidData, speech_rfid_block, 16);
        //dump_byte_array(speechRfidData, 16); Serial.println();

        //resetProfileRfid();
        delay(1000);
      }
      
      closeRfid();


  delay(100);              // wait for 100 ms
}

void uidToHexAscii() {
  bin_to_strhex((unsigned char *)playerUid, sizeof(playerUid), &asciiUid);
  lcdMessageTwoLine(asciiUid, (char*)playerName);
  delay(10000);
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

