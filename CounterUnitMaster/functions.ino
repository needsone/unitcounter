/**
 * @file functions.ino
 * @brief Credit/unit management functions for RFID cards
 */

// Block numbers for Mifare Classic card
#define CREDIT_BLOCK  2
#define NAME_BLOCK    4
#define CLUB_BLOCK    6
#define MAX_CREDIT    60000  // Maximum credit in seconds (~16.6 hours)

// ============================================================================
// Credit Reading Functions
// ============================================================================

/**
 * Get current credit from card in seconds
 * @return Credit in seconds, or 0 on error
 */
unsigned int HowManyUnit() {
  unsigned int retRead = read_block(CREDIT_BLOCK, keya, readbackblock, 0);

  if (retRead == 3 || retRead == 4) {
    return 0;
  }

  // Credit stored as big-endian 16-bit value
  unsigned int credit = ((unsigned int)readbackblock[0] << 8) | readbackblock[1];
  return credit;
}

/**
 * Get the unique ID of the RFID card
 * @return 32-bit card ID from first 4 bytes of block 0, or 0 on error
 */
uint32_t getCardId() {
  unsigned int retRead = read_block(0, keya, readbackblock, 0);

  if (retRead == 3 || retRead == 4) {
    lcd.clear();
    lcd.print(F("No Card"));
    return 0;
  }

  // Combine first 4 bytes into a 32-bit card ID
  uint32_t cardId = ((uint32_t)readbackblock[0] << 24) |
                    ((uint32_t)readbackblock[1] << 16) |
                    ((uint32_t)readbackblock[2] << 8)  |
                    ((uint32_t)readbackblock[3]);
  return cardId;
}

// ============================================================================
// Credit Modification Functions
// ============================================================================

/**
 * Remove credit from the card
 * @param amount Amount to remove in seconds
 * @return true if successful, false on error or insufficient credit
 */
boolean rmUnit(int amount) {
  unsigned int retRead = read_block(CREDIT_BLOCK, keya, readbackblock, 0);

  if (retRead == 3 || retRead == 4) {
    lcd.clear();
    lcd.print(F("No Card"));
    return false;
  }

  unsigned int currentCredit = ((unsigned int)readbackblock[0] << 8) | readbackblock[1];

  if (currentCredit < amount) {
    lcd.clear();
    lcd.print(F("Not enough credit:"));
    lcd.setCursor(0, 1);
    lcd.print(currentCredit);

    // Set credit to zero
    blockcontent[0] = 0;
    blockcontent[1] = 0;
    write_block(CREDIT_BLOCK, keya, blockcontent, 0);
    return false;
  }

  unsigned int newCredit = currentCredit - amount;
  blockcontent[0] = (newCredit >> 8) & 0xFF;
  blockcontent[1] = newCredit & 0xFF;

  unsigned int retWrite = write_block(CREDIT_BLOCK, keya, blockcontent, 0);
  if (retWrite != 1) {
    lcd.clear();
    lcd.print(F("Write error: "));
    lcd.print(retWrite);
    return false;
  }

  return true;
}

/**
 * Add credit to the card
 * @param amount Amount to add in minutes
 * @return true if successful, false on error or overflow
 */
boolean addUnit(int amount) {
  unsigned int retRead = read_block(CREDIT_BLOCK, keya, readbackblock, 0);

  if (retRead == 3 || retRead == 4) {
    lcd.clear();
    lcd.print(F("No Card"));
    return false;
  }

  unsigned int currentCredit = ((unsigned int)readbackblock[0] << 8) | readbackblock[1];
  unsigned long newCredit = (unsigned long)currentCredit + ((unsigned long)amount * 60);

  // Check for overflow
  if (newCredit >= MAX_CREDIT) {
    lcd.clear();
    lcd.print(F("Max credit reached"));
    return false;
  }

  blockcontent[0] = (newCredit >> 8) & 0xFF;
  blockcontent[1] = newCredit & 0xFF;

  unsigned int retWrite = write_block(CREDIT_BLOCK, keya, blockcontent, 0);
  if (retWrite != 1) {
    lcd.clear();
    lcd.print(F("Write error: "));
    lcd.print(retWrite);
    return false;
  }

  return true;
}

// ============================================================================
// Name Reading Functions
// ============================================================================

/**
 * Read a name field from the card into readbackblock
 * @param block Block number to read (NAME_BLOCK or CLUB_BLOCK)
 * @return true if successful
 */
boolean getName(int block) {
  unsigned int retRead = read_block(block, keya, readbackblock, 0);
  return (retRead == 1);
}
