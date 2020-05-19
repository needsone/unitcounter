/*
int readBlock(int blockNumber, byte arrayAddress[]) {
  uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, keya);

  if (success) {
    success = nfc.mifareclassic_ReadDataBlock(blockNumber, arrayAddress);
    return (1);
  } else {
    return (3);
  }
}

int writeBlock(int blockNumber, byte arrayAddress[]) {
  uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, keya);

  if (success) {
    success = nfc.mifareclassic_WriteDataBlock(blockNumber, arrayAddress);
    return (1);
  } else {
    return (3);
  }
}


*/

/*
void formatRfid() {
  byte blockcontent[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int retWrite = writeBlock(8, blockcontent);

  retWrite = writeBlock(4, blockcontent);
  retWrite = writeBlock(2, blockcontent);
  lcd.clear();
  lcd.print("Format done ...");
  return;
}
*/
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
    lcd.clear();
    lcd.print("Auth problem");
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
    lcd.clear();
    lcd.print("Auth problem");
    delay(2000);
    return (3);
  } else {
    success = nfc.mifareclassic_WriteDataBlock(blockNumber, arrayAddress);
    return (1);
  }
}

*/


void read_write_error_msg(uint8_t code) {

  lcd.clear();
  if (code == 2 ) {
    lcd.print("Reader Problem");
  } else if (code == 3) {
    lcd.print("Auth Fail");
  } else if (code == 4) {
    lcd.print("Read fail");
  } else if (code == 5) {
    lcd.print("Write fail");
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

void formatRfid() {
  byte blockcontent[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  write_block(8, keya, blockcontent, 0);
  write_block(6, keya, blockcontent, 0);
  write_block(4, keya, blockcontent, 0);
  write_block(2, keya, blockcontent, 0);
  write_block(1, keya, blockcontent, 0);
  write_block(5, keya, blockcontent, 0);
  lcd.clear();
  lcd.print("Format done ...");
  delay(500);
  return;
}

/// KEY CLUB TO HANDLE !!!!!!
/// Username en 6 ou empty

void initRfid(BridgeClient client) {
  String buffer;
  int i = 0;

  formatRfid();
  //  writeBlock(6, username);

  buffer = client.readString(); 
  buffer.getBytes(readbackblock, 16);

  write_block(4, keya, readbackblock, 0);
  write_block(6, keya, clubname, 0);
  
  // while (i < 15) {
  //  lcd.print(char(username[i]));
  //  i++;
  // }
  // int retWrite = writeBlock(3, club_key);
  // printName(4);
}

/*
 *  uint8_t mifareclassic_AuthenticateBlock (uint8_t * uid, uint8_t uidLen,
 *                                           uint32_t blockNumber, uint8_t keyNumber,
 *                                           uint8_t * keyData);
 * uint8_t mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t * data);
 * uint8_t mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t * data);
 */

