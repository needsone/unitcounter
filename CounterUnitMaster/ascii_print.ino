/**
 * @file ascii_print.ino
 * @brief Display functions for LCD output
 */

// ASCII character range for valid name characters
#define CHAR_ALPHA_START  0x41  // 'A'
#define CHAR_ALPHA_END    0x7A  // 'z'
#define CHAR_SPACE        0x20  // ' '

// ============================================================================
// Name Display Functions
// ============================================================================

/**
 * Print a name field from the card to LCD
 * @param block Block number to read (BLOCK_NAME or BLOCK_CLUB)
 * @return 1 on success, 0 on error
 */
int printName(int block) {
    unsigned int retRead = read_block(block, keya, readbackblock, 0);

    lcd.setCursor(0, 1);
    if (retRead == 3 || retRead == 4) {
        lcd.print(F("No Card"));
        return 0;
    }

    // Print valid ASCII characters only
    for (int i = 0; i < 16; i++) {
        uint8_t c = readbackblock[i];
        if ((c >= CHAR_ALPHA_START && c <= CHAR_ALPHA_END) || c == CHAR_SPACE) {
            lcd.print((char)c);
        } else {
            break;
        }
    }

    return 1;
}

// ============================================================================
// Credit Display Functions
// ============================================================================

/**
 * Display credit information on LCD
 * @param freqOn Current frequency/speed (for display, not used in Master)
 */
void printCredit(float freqOn) {
    unsigned int minutes = HowManyUnit() / SECONDS_PER_MIN;

    lcd.clear();

    // Line 0: Credit in minutes
    lcd.print(F("Credit: "));
    lcd.print(minutes);
    lcd.print(F(" min"));

    // Line 1: Customer name
    lcd.setCursor(0, 1);
    lcd.print(F("Name: "));
    getName(BLOCK_NAME);
    for (int i = 0; i < 14; i++) {
        uint8_t c = readbackblock[i];
        if (c != 0 && ((c >= CHAR_ALPHA_START && c <= CHAR_ALPHA_END) || c == CHAR_SPACE)) {
            lcd.print((char)c);
        } else {
            break;
        }
    }

    // Line 2: Club name
    lcd.setCursor(0, 2);
    lcd.print(F("Club: "));
    getName(BLOCK_CLUB);
    for (int i = 0; i < 14 && readbackblock[i] != 0; i++) {
        uint8_t c = readbackblock[i];
        if ((c >= CHAR_ALPHA_START && c <= CHAR_ALPHA_END) || c == CHAR_SPACE) {
            lcd.print((char)c);
        } else {
            break;
        }
    }

    // Line 3: IP address
    lcd.setCursor(0, 3);
    Process p;
    p.begin("getIp");
    p.run();
    while (p.available() > 1) {
        char c = p.read();
        lcd.print(c);
    }

    delay(2000);
}
