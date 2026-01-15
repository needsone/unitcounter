/**
 * @file CounterUnit_GPS_Aceel_PN532.ino
 * @brief UnitCounter Onboard Unit - GPS/Accelerometer based credit deduction
 *
 * Onboard unit for the UnitCounter system at Lac de Joux.
 * Monitors boat usage and deducts credit based on speed:
 *   - Low speed (<5 km/h): 2 seconds per cycle
 *   - High speed (>50 km/h): 3 seconds per cycle
 *   - Normal speed: 2 seconds per cycle
 *
 * Hardware:
 *   - Arduino Micro
 *   - PN532 NFC reader (SPI)
 *   - OLED I2C display
 *   - Adafruit GPS module
 *   - LSM9DS0 accelerometer
 *   - Engine relay
 *
 * RFID Card Memory Layout (Mifare Classic):
 *   - Block 2: Credit (seconds, big-endian uint16)
 *   - Block 4: Customer name (16 bytes ASCII)
 *   - Block 6: Club name (16 bytes ASCII)
 *   - Block 8: Master card identifier
 */

// ============================================================================
// Includes
// ============================================================================

#include <Adafruit_GPS.h>
#include <Wire.h>
#include "OLedI2C.h"
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>

// ============================================================================
// Configuration Constants
// ============================================================================

// PN532 NFC Configuration (SPI mode)
#define PN532_SS        10

// Pin Configuration
#define PIN_ENGINE       6   // Engine relay
#define PIN_RPM         13   // RPM input
#define PIN_BUZZER       1   // Buzzer

// RFID Block Numbers
#define BLOCK_CREDIT     2   // Credit storage (seconds)
#define BLOCK_NAME       4   // Customer name
#define BLOCK_CLUB       6   // Club name
#define BLOCK_MASTER     8   // Master card ID

// Credit Limits
#define MAX_CREDIT       60000   // Maximum credit in seconds (~16.6 hours)
#define MIN_CREDIT       10      // Minimum credit to operate
#define SECONDS_PER_MIN  60

// Speed Thresholds (km/h)
#define SPEED_LOW        5       // Below this: low deduction
#define SPEED_HIGH       50      // Above this: high deduction

// Deduction Rates (seconds per cycle)
#define DEDUCT_LOW       2       // At low/normal speed
#define DEDUCT_HIGH      3       // At high speed

// GPS Configuration
#define GPS_TIMEOUT_MS   30000   // 30 second timeout

// Accelerometer threshold
#define ACCEL_THRESHOLD  12.0f

// ============================================================================
// Global Variables
// ============================================================================

// GPS
Adafruit_GPS GPS(&Serial1);
HardwareSerial mySerial = Serial1;

// Display
OLedI2C lcd;

// Sensors
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(0x6B);
float accelerationMagnitude = 0;

// RFID
Adafruit_PN532 nfc(PN532_SS);
byte blockcontent[16];
byte readbackblock[16];
uint8_t uid[7] = {0};
uint8_t uidLength;
uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// State
boolean RFID_Master_Correct = false;
uint32_t timer = millis();

// ============================================================================
// Setup
// ============================================================================

void setup() {
    Serial.begin(115200);

    // Initialize engine control
    pinMode(PIN_ENGINE, OUTPUT);
    stopEngine();

    // Initialize SPI and I2C
    SPI.begin();
    Wire.begin();

    // Initialize OLED display
    lcd.init();
    lcd.setContrast(0xFF);
    print("Initializing...");

    // Initialize NFC reader
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        lcd.clearLcd();
        print("No PN53x board!");
        while (1);  // Halt
    }
    nfc.SAMConfig();

    // Initialize accelerometer
    if (!lsm.begin()) {
        Serial.print(F("No LSM9DS0 detected!"));
        while (1);  // Halt
    }
    displaySensorDetails();
    configureSensor();

    // Initialize GPS
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
    GPS.sendCommand(PGCMD_ANTENNA);
    mySerial.println(PMTK_Q_RELEASE);

    lcd.clearLcd();
    print("Ready!");
    delay(1000);
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    // Reinitialize NFC for each cycle
    nfc.begin();

    // Get current speed from GPS
    int speed = getSpeed();

    // Display prompt
    lcd.clearLcd();
    print("Insert card");

    // Wait for card
    uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
        // Display credit and name
        lcd.clearLcd();
        lcd.cursPos(0, 0);
        print("Credit: ");
        lcd.cursPos(0, 1);
        printName(BLOCK_NAME);

        // Main operation loop - run while credit available
        int credit;
        while ((credit = HowManyUnit()) >= MIN_CREDIT) {
            startEngine();

            // Read accelerometer
            sensors_event_t accel, mag, gyro, temp;
            lsm.getEvent(&accel, &mag, &gyro, &temp);
            accelerationMagnitude = sqrt(
                accel.acceleration.x * accel.acceleration.x +
                accel.acceleration.y * accel.acceleration.y +
                accel.acceleration.z * accel.acceleration.z
            );

            // Update display
            printCredit((float)speed, accelerationMagnitude);

            // Get current speed
            speed = getSpeed();

            // Deduct credit based on speed
            if (speed < 0) {
                // GPS timeout - use default deduction
                rmUnit(DEDUCT_LOW);
            } else if (speed < SPEED_LOW) {
                rmUnit(DEDUCT_LOW);
            } else if (speed > SPEED_HIGH) {
                rmUnit(DEDUCT_HIGH);
            } else {
                rmUnit(DEDUCT_LOW);
            }

            delay(1000);
        }

        // Credit exhausted
        stopEngine();
        printCredit((float)speed, accelerationMagnitude);

        lcd.clearLcd();
        print("Credit depleted!");
        delay(2000);
    }
}

// ============================================================================
// Engine Control
// ============================================================================

/**
 * Start the engine by activating relay
 */
void startEngine() {
    digitalWrite(PIN_ENGINE, LOW);
}

/**
 * Stop the engine by deactivating relay
 */
void stopEngine() {
    digitalWrite(PIN_ENGINE, HIGH);
}
