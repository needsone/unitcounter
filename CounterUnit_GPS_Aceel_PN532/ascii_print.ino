  /*
  void fillData(int block, byte blockcontent[] ) {
    int retWrite = writeBlock(block, blockcontent);
    lcd.clear();
    print("Name written ...");
    return;
  }
  */
  int printName(int block) {
    unsigned int retRead = read_block(block, keya, readbackblock, 0);

    //lcd.clearLcd();
    int i = 0;
    while ( (readbackblock[i] >= 0x41 &&  readbackblock[i] <= 0x7A) || readbackblock[i] == 32) {
      lcd.sendData((char)readbackblock[i]);
      i++;
    }
    return (1);
  }

  void PrintByteLcd(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
      lcd.sendData(buffer[i]);
    }
  }
  /*
  int getName(int blockNumber) {
    // Username  = 6;
    // Clubname  = 4;
   return (read_block(blockNumber, keya, readbackblock, 0));
  }
  */

  void printCredit(float freqOn,float accel) {
    int i = 0;

    unsigned int minutes = HowManyUnit() / 60;
    lcd.cursPos(0, 3);
    print(minutes);
    print("       ");
  //  print("mn: ");
  /*  print(" s:");
   print(HowManyUnit());*/

    //while (i < 16 && readbackblock[i] != 0 && (  (readbackblock[i] >= 0x41 &&  readbackblock[i] <= 0x7A) || readbackblock[i] == 0x20)) {
    //  print(char(readbackblock[i]));
    //  i++;
    //}

  /*
    print("Sp:");
    print(freqOn);
    print(" Acc:");
    print(accel); */

  }
