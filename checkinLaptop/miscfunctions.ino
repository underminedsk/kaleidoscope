

boolean checkForNewPlayer(byte playerUid[], boolean &isNew) {
  isNew = true;
 
  return true;
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

