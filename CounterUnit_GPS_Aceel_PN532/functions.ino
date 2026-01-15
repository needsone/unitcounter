/**
 * @file functions.ino
 * @brief Core functions for GPS unit - credit management, GPS, sensors
 */

// ============================================================================
// GPS Functions
// ============================================================================

/**
 * Get current speed from GPS module
 * @return Speed in km/h, or -1 if GPS timeout (no fix within 30 seconds)
 */
int getSpeed() {
    unsigned long startTime = millis();

    while (millis() - startTime < GPS_TIMEOUT_MS) {
        char c = GPS.read();

        if (GPS.newNMEAreceived()) {
            if (GPS.parse(GPS.lastNMEA())) {
                // Reset timer if it wrapped around
                if (timer > millis()) timer = millis();

                // Check approximately every 2 seconds
                if (millis() - timer > 2000) {
                    timer = millis();

                    if (GPS.fix) {
                        float speed = GPS.speed * 1.852;  // knots to km/h
                        Serial.print(F("Speed: "));
                        Serial.println(speed);
                        return (int)speed;
                    } else {
                        lcd.clearLcd();
                        print("GPS searching...");
                        lcd.cursPos(0, 1);
                    }
                }
            }
        }
    }

    // Timeout - no GPS fix
    lcd.clearLcd();
    print("GPS timeout!");
    return -1;
}

// ============================================================================
// Sensor Functions
// ============================================================================

/**
 * Display sensor details to Serial (debug)
 */
void displaySensorDetails() {
    sensor_t accel, mag, gyro, temp;
    lsm.getSensor(&accel, &mag, &gyro, &temp);
}

/**
 * Configure accelerometer sensor
 */
void configureSensor() {
    lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
}

// ============================================================================
// Credit Functions
// ============================================================================

/**
 * Get current credit from card in seconds
 * @return Credit in seconds, or 0 on error
 */
unsigned int HowManyUnit() {
    unsigned int retRead = read_block(BLOCK_CREDIT, keya, readbackblock, 0);

    if (retRead == 3 || retRead == 4) {
        return 0;
    }

    // Credit stored as big-endian 16-bit value
    unsigned int credit = ((unsigned int)readbackblock[0] << 8) | readbackblock[1];
    return credit;
}

// ============================================================================
// Print Functions (OLED wrapper)
// ============================================================================

/**
 * Print a string to the OLED display
 * @param str String to print
 */
void print(const char* str) {
    while (*str) {
        lcd.sendData(*str++);
    }
}

/**
 * Print a String object to the OLED display
 * @param s String to print
 */
void print(String s) {
    for (unsigned int i = 0; i < s.length(); i++) {
        lcd.sendData(s[i]);
    }
}

/**
 * Print an unsigned integer to the OLED display
 * @param val Value to print
 */
void print(unsigned int val) {
    char buffer[12];
    utoa(val, buffer, 10);
    print(buffer);
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
    uint8_t retRead = read_block(BLOCK_CREDIT, keya, readbackblock, 0);

    if (retRead == 3 || retRead == 4) {
        lcd.clearLcd();
        print("No card!");
        return false;
    }

    unsigned int currentCredit = ((unsigned int)readbackblock[0] << 8) | readbackblock[1];

    if (currentCredit < amount) {
        lcd.clearLcd();
        print("Low credit:");
        lcd.cursPos(0, 1);
        print(currentCredit);

        // Set credit to zero
        blockcontent[0] = 0;
        blockcontent[1] = 0;
        write_block(BLOCK_CREDIT, keya, blockcontent, 0);
        return false;
    }

    unsigned int newCredit = currentCredit - amount;
    blockcontent[0] = (newCredit >> 8) & 0xFF;
    blockcontent[1] = newCredit & 0xFF;

    uint8_t retWrite = write_block(BLOCK_CREDIT, keya, blockcontent, 0);
    if (retWrite != 1) {
        lcd.clearLcd();
        print("Write error:");
        print(retWrite);
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
    unsigned int retRead = read_block(BLOCK_CREDIT, keya, readbackblock, 0);

    if (retRead == 3 || retRead == 4) {
        lcd.clearLcd();
        print("No card!");
        return false;
    }

    unsigned int currentCredit = ((unsigned int)readbackblock[0] << 8) | readbackblock[1];
    unsigned long newCredit = (unsigned long)currentCredit + ((unsigned long)amount * SECONDS_PER_MIN);

    // Check for overflow
    if (newCredit >= MAX_CREDIT) {
        lcd.clearLcd();
        print("Max credit!");
        return false;
    }

    blockcontent[0] = (newCredit >> 8) & 0xFF;
    blockcontent[1] = newCredit & 0xFF;

    unsigned int retWrite = write_block(BLOCK_CREDIT, keya, blockcontent, 0);
    if (retWrite != 1) {
        lcd.clearLcd();
        print("Write error:");
        print(retWrite);
        return false;
    }

    return true;
}
