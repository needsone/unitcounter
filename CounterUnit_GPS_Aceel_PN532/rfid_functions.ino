/**
 * @file rfid_functions.ino
 * @brief RFID block read/write operations for Mifare Classic cards
 */

// ============================================================================
// Error Handling
// ============================================================================

/**
 * Display error message on OLED based on error code
 * @param code Error code (2=reader, 3=auth, 4=read, 5=write)
 */
void read_write_error_msg(uint8_t code) {
    lcd.clearLcd();
    switch (code) {
        case 2:
            print("Reader Problem");
            break;
        case 3:
            print("Auth Fail");
            break;
        case 4:
            print("Read Fail");
            break;
        case 5:
            print("Write Fail");
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
// Card Formatting
// ============================================================================

/**
 * Format the RFID card by clearing data blocks
 */
void formatRfid() {
    byte emptyBlock[16] = {0};

    write_block(BLOCK_MASTER, keya, emptyBlock, 0);
    write_block(BLOCK_NAME, keya, emptyBlock, 0);
    write_block(BLOCK_CREDIT, keya, emptyBlock, 0);

    lcd.clearLcd();
    print("Format done!");
}
