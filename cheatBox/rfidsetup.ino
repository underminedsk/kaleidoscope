#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         10           // Configurable, see typical pin layout above
#define SS_PIN          A0          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;


void rfidSetup()
{
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_33dB);  //RxGain_33dB RxGain_38dB RxGain_43dB RxGain_48dB
  

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}


boolean rfidDetected(byte uid[], byte playerName[], byte playerData[], int incByteLoc) {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return false;

  // make sure nuid has been read
  if ( ! mfrc522.PICC_ReadCardSerial())
    return false;

  for (int i = 0; i < 4; i++) {
    uid[i] = mfrc522.uid.uidByte[i];
  }
  Sprintln("found uid");
  //dump_byte_array(uid, 4);
  boolean status;
  status = readRfid(playerName, name_rfid_block, 16); 
  if (status != true) {return status;}
  status = readRfid(playerData, data_rfid_block, 16); 
  if (status != true) {return status;}
  //status = incrementBlockRfid(incByteLoc);
  //if (status != true) {return status;}
  return true;
}

boolean writeRfid(byte *dataBlock, byte blockAddr, byte bufferSize) {
  byte trailerBlock   = (blockAddr/4)*4 + 3;
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {return false;}
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, bufferSize);
  if (status != MFRC522::STATUS_OK) {return false;} Sprintln("wrote rf data");
  return true;
}

boolean readRfid(byte *dataBlock, byte blockAddr, byte bufferSize) {
  byte trailerBlock   = (blockAddr/4)*4 + 3;
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {return false;}
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {return false;}
  
  memcpy(dataBlock, buffer, 16);  Sprintln("read rf data");
  return true;
}

boolean incrementBlockRfid(byte dataBlockByte) {
  boolean status;
  byte rfidData[16];
  Sprintln("start increment function");
  status = readRfid(rfidData,data_rfid_block,16); 
  if (status != true) {return status;}
  rfidData[dataBlockByte]++;
  status = writeRfid(rfidData,data_rfid_block,16); 
  return status;
}

void closeRfid() {
  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}


void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

boolean timeOut() {
  return false;
}


