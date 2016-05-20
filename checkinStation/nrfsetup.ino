

unsigned long counter, rxTimer;          //Counter and timer for keeping track transfer info
unsigned long startTime, stopTime;  
bool TX=1,RX=0,role=0;
int standby = 0;

void nrfSetup() {

  radio.begin();                           // Setup and configure rf radio
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.setRetries(15,15);                  // Optionally, increase the delay between retries & # of retries
  
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

  radio.startListening();                 // Start listening
     
  radio.powerUp();                        //Power up the radio
}


boolean isNewPlayer(byte uid[], uint8_t uidSize) {
  //data[0] = uid;
  pinMode(5, OUTPUT);     
  digitalWrite(5, LOW);
  radio.writeFast(uid, uidSize);
  radio.stopListening();
  radio.write(uid,uidSize);
  
  standby = radio.txStandBy(1000);
  
  radio.startListening();
  Serial.println("blaaaaaaaaa");
  while (!radio.available()){
    delay(100);
  }
  Serial.println("asdasda");
  radio.read(&nrfData,1);
  Serial.write(nrfData[0]);
  if (nrfData[0]=='Y') {
    return true;
  } 
  else {
    return false;
  }
  
}

boolean videoDone() {
  return true;
}

void waitForRFdata(byte receivedData[], uint8_t receiveSize) { 
  radio.startListening();
  while (!radio.available()) {
    delay(100);
  }
  radio.read(receivedData, receiveSize);
}

