

boolean isNewPlayer(byte playerUid[], uint8_t uidSize) {
  boolean isNew = true;


  if (isNew) {
    nrfData[0] = 'Y';
  }
  else {
    nrfData[0] = 'N';
  }
  radio.stopListening();
  radio.write(nrfData, 1);
  return true;
}


boolean nameEntered() {
  //Get keyboard input and pass to LCD display on check in station

  /* DEBUG
    delay(3000);
    nrfData[0] = 'A';
    radio.write(nrfData, 1);
    delay(1000);
    nrfData[0] = 'B';
    radio.write(nrfData, 1);
    delay(1000);
    nrfData[0] = 'C';
    radio.write(nrfData, 1);
    delay(1000);
    nrfData[0] = 8;
    radio.write(nrfData, 1);
    delay(1000);
    nrfData[0] = 'D';
    radio.write(nrfData, 1);
    delay(1000);
    nrfData[0] = 13;  // carriage return
    radio.write(nrfData, 1);
  */

  if (Serial.available())
  {
    //Serial.println("got serial");
    nrfData[0] = Serial.read();
    radio.write(nrfData, 1);
  }

  if (nrfData[0] == 13) {
    return true;
  } else {
    return false;
  }

  delay(500);
}

