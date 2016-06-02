IRsend irsend;
unsigned long CrappyProjIrCode = 0x40BFB847;
unsigned long EpsonMuteIrCode = 0xC1AAC936;

boolean checkForNewPlayer(boolean &isNew) {
  byte serialResp[1];
  if (Serial.available()) {
    serialResp[0] = Serial.read();
    if (serialResp[0]=='Y') {
      isNew = true; 
      return true;
    } 
    else if (serialResp[0]=='N') {
      isNew = false; 
      return true;
    }
    else {
      return false;
    }
  }
  
  return false;
}


boolean nameEntered() {
  //Get keyboard input and pass to LCD display on check in station
  byte letter[1];
  int sendKeyTries = 0; 
  if (Serial.available())
  {
    letter[0] = Serial.read();
    Sprintln(letter[0]);    
  
    while (sendKeyTries < 10 && letter[0]) {
      if (sendRFData(letter, 1)) {
        if (letter[0] == 13) {
          return true;
        }
        break; 
      }
    sendKeyTries++;
    delay(100);
    }
  }
  return false;
}

boolean writeTagDone() {
  byte writeTagDoneRf[1];
  if (receivedRFData(writeTagDoneRf,1)) {
    if (writeTagDoneRf[0] == 'W') {
      return true;
    }
  }
  return false;
}

boolean videoDone() {
  byte letter[1];
  int sendKeyTries = 0; 
  if (Serial.available())
  {
    letter[0] = Serial.read();
    Sprintln(letter[0]);

    // If received on signal, power on projector
    if (letter[0] == 'O') {
      irsend.sendNEC(EpsonMuteIrCode, 32);
    }
    // If received off signal, power off projector and send packet
    // to checkin station to exit Play_Video state
    else if (letter[0] == 'F') {
      irsend.sendNEC(EpsonMuteIrCode, 32);  
      while (sendKeyTries < 10 && letter[0]) {
        if (sendRFData(letter, 1)) {
          return true;
        }
      sendKeyTries++;
      delay(100);
      }
    }
    // if user is all done, just advance back to check in state
    else if (letter[0] == 'D') {
      while (sendKeyTries < 10 && letter[0]) {
        if (sendRFData(letter, 1)) {
          return true;
        }
      }
    }
  }
  return false;
}

