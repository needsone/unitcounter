/**
 * @file rfid_functions.ino
 * @brief RFID block read/write operations for Mifare Classic cards
 */

// ============================================================================
// Error Handling
// ============================================================================

/**
 * Display error message on LCD based on error code
 * @param code Error code (2=reader, 3=auth, 4=read, 5=write)
 */
void read_write_error_msg(uint8_t code) {
  lcd.clear();
  switch (code) {
    case 2:
      lcd.print(F("Reader Problem"));
      break;
    case 3:
      lcd.print(F("Auth Fail"));
      break;
    case 4:
      lcd.print(F("Read Fail"));
      break;
    case 5:
      lcd.print(F("Write Fail"));
      break;
  }
}

// ============================================================================
// Block Read/Write Operations
// ============================================================================

/**
 * Read a block from the RFID card
 * @param block Block number to read
 * @param key Authentication key (6 bytes)
 * @param data Buffer for read data (16 bytes)
 * @param a_or_b Key type: 0=Key A, 1=Key B
 * @return 1=success, 3=auth fail, 4=read fail
 */
uint8_t read_block(int block, uint8_t key[6], byte data[], boolean a_or_b) {
  uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, a_or_b, key);

  if (!success) {
    read_write_error_msg(3);
    return 3;
  }

  success = nfc.mifareclassic_ReadDataBlock(block, data);
  if (!success) {
    read_write_error_msg(4);
    return 4;
  }

  return 1;
}

/**
 * Write data to a block on the RFID card
 * @param block Block number to write
 * @param key Authentication key (6 bytes)
 * @param data Data to write (16 bytes)
 * @param a_or_b Key type: 0=Key A, 1=Key B
 * @return 1=success, 3=auth fail, 5=write fail
 */
uint8_t write_block(int block, uint8_t key[6], byte data[], boolean a_or_b) {
  uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, a_or_b, key);

  if (!success) {
    read_write_error_msg(3);
    return 3;
  }

  success = nfc.mifareclassic_WriteDataBlock(block, data);
  if (!success) {
    read_write_error_msg(5);
    return 5;
  }

  return 1;
}

// ============================================================================
// Card Initialization
// ============================================================================

/**
 * Format the RFID card by clearing all data blocks
 */
void formatRfid() {
  byte emptyBlock[16] = {0};

  write_block(8, keya, emptyBlock, 0);
  write_block(6, keya, emptyBlock, 0);
  write_block(4, keya, emptyBlock, 0);
  write_block(2, keya, emptyBlock, 0);
  write_block(1, keya, emptyBlock, 0);
  write_block(5, keya, emptyBlock, 0);

  lcd.clear();
  lcd.print(F("Format done"));
  delay(500);
}

/**
 * Initialize a new RFID card with customer name
 * @param client BridgeClient to read customer name from
 */
void initRfid(BridgeClient client) {
  formatRfid();

  String buffer = client.readString();
  buffer.getBytes(readbackblock, 16);

  write_block(4, keya, readbackblock, 0);
  write_block(6, keya, clubname, 0);
}
