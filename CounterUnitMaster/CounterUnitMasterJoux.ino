/**
 * @file CounterUnitMasterJoux.ino
 * @brief UnitCounter Master Station - RFID credit management with REST API
 *
 * Master station for the UnitCounter system at Lac de Joux.
 * Provides REST API endpoints for card management:
 *   - /arduino/credit   - Check card credit
 *   - /arduino/add/N    - Add N minutes
 *   - /arduino/rm/N     - Remove N minutes
 *   - /arduino/format   - Format card
 *   - /arduino/init/NAME - Initialize card with name
 *   - /arduino/ip       - Show IP address
 *   - /arduino/id       - Get card ID
 *
 * Hardware:
 *   - Arduino Yun
 *   - PN532 NFC reader (I2C)
 *   - 20x4 I2C LCD display
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

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <EEPROM.h>

// Arduino Yun Bridge libraries
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Process.h>

// ============================================================================
// Configuration Constants
// ============================================================================

// PN532 NFC Configuration (I2C mode)
#define PN532_IRQ       6
#define PN532_RESET     5

// Pin Configuration
#define PIN_ENGINE      8   // Engine relay (not used in Master)
#define PIN_RPM        13   // RPM input (not used in Master)
#define PIN_BUZZER      1   // Buzzer (not used in Master)

// RFID Block Numbers
#define BLOCK_CREDIT    2   // Credit storage (seconds)
#define BLOCK_NAME      4   // Customer name
#define BLOCK_CLUB      6   // Club name
#define BLOCK_MASTER    8   // Master card ID

// Credit Limits
#define MAX_CREDIT      60000   // Maximum credit in seconds (~16.6 hours)
#define SECONDS_PER_MIN 60

// Display Configuration
#define LCD_ADDRESS     0x27
#define LCD_COLS        20
#define LCD_ROWS        4

// ============================================================================
// Global Variables
// ============================================================================

byte blockcontent[16];
byte readbackblock[16];
byte clubname[16] = {'L', 'A', 'C', ' ', 'D', 'E', ' ', 'J', 'O', 'U', 'X', ' ', ' ', ' ', ' ', ' '};

boolean RFID_Master_Correct = false;
uint8_t uid[7] = {0};
uint8_t uidLength;
uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Hardware objects
BridgeServer server;
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// ============================================================================
// Setup
// ============================================================================

void setup() {
    // Initialize Bridge (Arduino Yun Linux communication)
    Bridge.begin();

    // Initialize LCD
    lcd.init();
    lcd.home();
    lcd.backlight();

    // Initialize NFC reader
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        lcd.clear();
        lcd.print(F("PN532 not found!"));
        while (1);  // Halt
    }
    nfc.SAMConfig();

    // Start REST API server
    server.listenOnLocalhost();
    server.begin();

    // Initialize Serial for debugging
    Serial.begin(9600);

    // Display IP address
    GetIp();
    delay(4000);
}

// ============================================================================
// Main Loop
// ============================================================================

void loop() {
    BridgeClient client = server.accept();

    lcd.clear();
    printCredit(0);

    // Wait for card
    uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    if (success) {
        client = server.accept();
        if (client) {
            processCommand(client);
            client.stop();
        }
        delay(500);
    }
}

// ============================================================================
// REST API Command Processing
// ============================================================================

/**
 * Process incoming REST API commands
 * @param client BridgeClient with command data
 */
void processCommand(BridgeClient client) {
    String command = client.readStringUntil('/');

    if (command == "credit") {
        unsigned int credit = HowManyUnit();
        client.print(credit / SECONDS_PER_MIN);
        client.print(F(" minutes"));
    }
    else if (command == "add") {
        addCommand(client);
    }
    else if (command == "rm") {
        rmCommand(client);
    }
    else if (command == "format") {
        ndef_to_classic();
        formatRfid();
        client.print(F("Card formatted"));
    }
    else if (command == "init") {
        initRfid(client);
        client.print(F("Card initialized"));
    }
    else if (command == "ip") {
        GetIp();
    }
    else if (command == "id") {
        client.print(GetId(client));
    }

    lcd.clear();
    printCredit(0);
    delay(500);
}

/**
 * Handle add credit command
 * @param client BridgeClient with amount parameter
 */
void addCommand(BridgeClient client) {
    int value = client.parseInt();

    if (value > 0) {
        lcd.clear();
        lcd.print(F("Adding: "));
        lcd.print(value);
        lcd.print(F(" min"));

        nfc.begin();
        if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
            if (addUnit(value)) {
                client.print(F("Added "));
                client.print(value);
                client.print(F(" minutes"));
            } else {
                client.print(F("Add failed"));
            }
        } else {
            client.print(F("No card"));
        }
    } else {
        lcd.clear();
        lcd.print(F("Invalid value"));
        client.print(F("Invalid value"));
    }
}

/**
 * Handle remove credit command
 * @param client BridgeClient with amount parameter
 */
void rmCommand(BridgeClient client) {
    int value = client.parseInt();

    if (value > 0) {
        lcd.clear();
        lcd.print(F("Removing: "));
        lcd.print(value);
        lcd.print(F(" min"));

        nfc.begin();
        if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
            int seconds = value * SECONDS_PER_MIN;
            if (rmUnit(seconds)) {
                client.print(F("Removed "));
                client.print(value);
                client.print(F(" minutes"));
            } else {
                client.print(F("Remove failed"));
            }
        } else {
            client.print(F("No card"));
        }
    } else {
        lcd.clear();
        lcd.print(F("Invalid value"));
        client.print(F("Invalid value"));
    }
}
