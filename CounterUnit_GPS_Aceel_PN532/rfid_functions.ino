
void formatRfid() {
  byte blockcontent[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int retWrite = write_block(8, keya, blockcontent, 0);
  retWrite = write_block(4, keya, blockcontent, 0);
  retWrite = write_block(2, keya, blockcontent, 0);
  lcd.clearLcd();
  print("Format done ...");
  return;
}

/*
 *  uint8_t mifareclassic_AuthenticateBlock (uint8_t * uid, uint8_t uidLen,
 *                                           uint32_t blockNumber, uint8_t keyNumber,
 *                                           uint8_t * keyData);
 * uint8_t mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t * data);
 * uint8_t mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t * data);
 */
/*
int readBlock(int blockNumber, byte arrayAddress[]) {
  uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, keya);

  if (!success) {
    lcd.clearLcd();
    print("Auth problem");
    delay(2000);
    return (3);
  } else {
    success = nfc.mifareclassic_ReadDataBlock(blockNumber, arrayAddress);
    return (1);
  }
}

int writeBlock(int blockNumber, byte arrayAddress[]) {
  uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, keya);

  if (!success) {
    lcd.clearLcd();
    print("Auth problem");
    delay(2000);
    return (3);
  } else {
    success = nfc.mifareclassic_WriteDataBlock(blockNumber, arrayAddress);
    return (1);
  }
}
*/

void read_write_error_msg(uint8_t code) {

  lcd.clearLcd();
  if (code == 2 ) {
    print("Reader Problem");
  } else if (code == 3) {
    print("Auth Fail   ");
  } else if (code == 4) {
    print("Read fail   ");
  } else if (code == 5) {
    print("Write fail   ");
  }
}

uint8_t read_block(int block, uint8_t key[6], byte data[], boolean a_or_b) {
  //  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  //  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  // 4 = read fail
  // 2 = read passive fail
  // 3 = auth fail
  // 1 = success
  // uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    uint8_t  success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, a_or_b, key);

    if (success) {
      success = nfc.mifareclassic_ReadDataBlock(block, data);
      } else {
    read_write_error_msg(3);
    return (3);
  }
  if (success) {
    return (1);
  } else {
    read_write_error_msg(4);
    return (4);
  }
}

uint8_t write_block(int block, uint8_t key[6], byte data[], boolean a_or_b) {
  // uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  // uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  // write Fail
  // uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  uint8_t  success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, a_or_b, key);
  if (success) {
    success = nfc.mifareclassic_WriteDataBlock(block, data);
  } else {
    read_write_error_msg(3);
    return (3);
  }
  if (success) {
    return (1);
  } else {
    read_write_error_msg(5);
    return (5);
  }
}
