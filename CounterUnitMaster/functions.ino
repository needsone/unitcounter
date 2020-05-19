// return the value of the block content
unsigned int HowManyUnit () {
  int block = 2;
  unsigned int retRead = read_block(block, keya, readbackblock,0);

  if (retRead == 3 || retRead == 4) {
    return 0;
  }
  unsigned int readValue = readbackblock[1] + readbackblock[0] * 256;
  return (readValue);
}

//  YES !!!!
unsigned int getCardId () {
  // Get the id of the card 4 first bytes of the block 0 and set an int with it
  unsigned int retRead = read_block(0, keya, readbackblock,0);
  
  if (retRead == 3 || retRead == 4) {
    lcd.clear();
    lcd.print("No Card");
    return 0;
  }
}

// In rmUnit amount is in sec
boolean rmUnit (int amount) {
  int block = 2;

  unsigned int retRead = read_block(block, keya, readbackblock,0);
  if (retRead == 3 || retRead == 4) {
    lcd.clear();
    lcd.print("No Card");
    return 0;
  }

  unsigned int readValue = readbackblock[1] + readbackblock[0] * 256;
  if (readValue < amount) {
    lcd.clear();
    lcd.print("Nout enough Unit :");
    lcd.setCursor(0, 1);
    lcd.print(readValue - amount);
    blockcontent[1] = 0;
    blockcontent[0] = 0;
    unsigned int retWrite = read_block(block, keya, readbackblock,0);
    return 0;
  }
  
  unsigned int writeValue = readValue - amount;
  blockcontent[1] = writeValue;
  blockcontent[0] = writeValue / 256;
  
  unsigned int retWrite = write_block(block, keya, blockcontent,0);
  if (retWrite == 4 || retWrite == 3 || retWrite == 2) {
    lcd.clear();
    lcd.print("Write error : ");
    lcd.print(retWrite);
    return 0;
  }
  return 1;
}

// Add add amount is in minutes
boolean addUnit(int amount) {
  int block = 2;
  unsigned int retRead = read_block(block, keya, readbackblock,0);

  if (retRead == 3 || retRead == 4) {
    lcd.clear();
    lcd.print("No Card");
    return 0;
  }

  unsigned int readValue = readbackblock[1] + readbackblock[0] * 256;
  unsigned int writeValue = readValue + amount * 60;

  if (writeValue < 60000) { // Limit max of the seconds amount (we use a unsigned int)
    blockcontent[1] = writeValue;
    blockcontent[0] = writeValue / 256;
    unsigned int retWrite = write_block(block, keya, blockcontent,0);
    if (retWrite == 4 || retWrite == 3 || retWrite == 2) {
      lcd.clear();
      lcd.print("Write error : ");
      lcd.print(retWrite);
      return 0;
    }
  }
  return 1;
}
