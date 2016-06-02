

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


boolean checkForNewPlayer(boolean &isNew) {

  byte nrfResponse[1];
  if (receivedRFData(nrfResponse,1)) {
    if (nrfResponse[0]=='Y') {
      isNew = true;
      return true;
    } 
    else if (nrfResponse[0]=='N') {
      isNew = false;
      return true;
    }
  }
  return false;
}

boolean videoDone() {
  byte videoDoneRf[1];
  if (receivedRFData(videoDoneRf,1)) {
    if (videoDoneRf[0] == 'F' || videoDoneRf[0] == 'D') {
      return true;
    }
  }
  return false;
}

boolean receivedRFData(byte receivedData[], uint8_t receiveSize) { 
  if (radio.available()) {
    Sprintln("found data!");
    radio.read(receivedData, receiveSize);
    //dump_byte_array(receivedData, receiveSize); Sprintln();
    return true;
  }
  return false;  
}

boolean sendRFData(byte dataToSend[], uint8_t sendSize) {
  boolean sendStatus;
  int sendTries = 0;
  radio.stopListening();
  //sendStatus = radio.write(dataToSend, sendSize);
  while (sendTries < 10) {
    sendStatus = radio.write(dataToSend, sendSize);
    if (sendStatus) {
      break;
    }
    sendTries++;
    delay(100);
  }
  radio.startListening();
  return sendStatus;
}

