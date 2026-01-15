/**
 * @file get.ino
 * @brief Utility functions for card ID and IP address retrieval
 */

// ============================================================================
// Card ID Functions
// ============================================================================

/**
 * Get the card ID from currently detected card
 * @param client BridgeClient (for compatibility, not used internally)
 * @return 32-bit card ID, or 0 on error
 */
uint32_t GetId(BridgeClient client) {
    uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
        // Build 32-bit ID from first 4 bytes of UID
        uint32_t cardid = ((uint32_t)uid[0] << 24) |
                          ((uint32_t)uid[1] << 16) |
                          ((uint32_t)uid[2] << 8)  |
                          ((uint32_t)uid[3]);

        lcd.clear();
        lcd.print(F("Card ID:"));
        lcd.setCursor(0, 1);
        lcd.print(cardid);
        Serial.println(cardid);

        return cardid;
    }

    return 0;
}

// ============================================================================
// Network Functions
// ============================================================================

/**
 * Display the IP address on LCD using Yun Linux command
 */
void GetIp() {
    lcd.clear();

    // Run shell script to get IP
    Process p;
    p.begin("getIp");
    p.run();

    lcd.clear();
    while (p.available() > 1) {
        char c = p.read();
        lcd.print(c);
    }

    lcd.setCursor(0, 1);
    lcd.print(F("Insert a card!"));
    delay(1000);
}
