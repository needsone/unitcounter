/**
 * @file ascii_print.ino
 * @brief Display functions for OLED output
 */

// ASCII character range for valid name characters
#define CHAR_ALPHA_START  0x41  // 'A'
#define CHAR_ALPHA_END    0x7A  // 'z'
#define CHAR_SPACE        0x20  // ' '

// ============================================================================
// Name Display Functions
// ============================================================================

/**
 * Print a name field from the card to OLED
 * @param block Block number to read (BLOCK_NAME or BLOCK_CLUB)
 * @return 1 on success, 0 on error
 */
int printName(int block) {
    unsigned int retRead = read_block(block, keya, readbackblock, 0);

    if (retRead == 3 || retRead == 4) {
        print("No card");
        return 0;
    }

    // Print valid ASCII characters only
    for (int i = 0; i < 16; i++) {
        uint8_t c = readbackblock[i];
        if ((c >= CHAR_ALPHA_START && c <= CHAR_ALPHA_END) || c == CHAR_SPACE) {
            lcd.sendData(c);
        } else {
            break;
        }
    }

    return 1;
}

/**
 * Print byte array to OLED (debug utility)
 * @param buffer Byte buffer to print
 * @param bufferSize Size of buffer
 */
void PrintByteLcd(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        lcd.sendData(buffer[i]);
    }
}

// ============================================================================
// Credit Display Functions
// ============================================================================

/**
 * Display credit information on OLED
 * @param speed Current speed in km/h
 * @param accel Current acceleration magnitude
 */
void printCredit(float speed, float accel) {
    unsigned int minutes = HowManyUnit() / SECONDS_PER_MIN;

    lcd.clearLcd();

    // Line 0: Credit in minutes and speed
    lcd.cursPos(0, 0);
    print("Cr:");
    print(minutes);
    print("m");

    if (speed >= 0) {
        print(" S:");
        print((unsigned int)speed);
    }

    // Line 1: Customer name
    lcd.cursPos(0, 1);
    printName(BLOCK_NAME);
}
