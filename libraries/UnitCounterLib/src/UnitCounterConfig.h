/**
 * @file UnitCounterConfig.h
 * @brief Configuration constants for UnitCounter RFID credit management system
 *
 * This file contains all constants, pin definitions, and configuration values
 * used across the UnitCounter projects (Master and GPS versions).
 */

#ifndef UNIT_COUNTER_CONFIG_H
#define UNIT_COUNTER_CONFIG_H

#include <Arduino.h>

// ============================================================================
// RFID Block Configuration (Mifare Classic)
// ============================================================================

/** Block number for credit/unit storage (stores seconds as 16-bit value) */
static const uint8_t BLOCK_CREDIT     = 2;

/** Block number for club name (16 bytes ASCII) */
static const uint8_t BLOCK_CLUBNAME   = 4;

/** Block number for customer/user name (16 bytes ASCII) */
static const uint8_t BLOCK_USERNAME   = 6;

/** Block number for master card identifier */
static const uint8_t BLOCK_MASTER_ID  = 8;

// ============================================================================
// RFID Constants
// ============================================================================

/** Mifare Classic block size in bytes */
static const uint8_t BLOCK_SIZE       = 16;

/** Maximum UID length for Mifare cards */
static const uint8_t UID_MAX_LENGTH   = 7;

/** Mifare key length in bytes */
static const uint8_t KEY_LENGTH       = 6;

/** Default Mifare key (factory default) */
static const uint8_t DEFAULT_KEY[KEY_LENGTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// ============================================================================
// Error Codes
// ============================================================================

/**
 * @enum RfidErrorCode
 * @brief Error codes returned by RFID operations
 */
typedef enum {
    ERR_SUCCESS           = 1,   /**< Operation completed successfully */
    ERR_READER_PROBLEM    = 2,   /**< Problem with RFID reader */
    ERR_AUTH_FAIL         = 3,   /**< Authentication failed */
    ERR_READ_FAIL         = 4,   /**< Block read failed */
    ERR_WRITE_FAIL        = 5,   /**< Block write failed */
    ERR_NO_CARD           = 6,   /**< No card detected */
    ERR_INSUFFICIENT_CREDIT = 7  /**< Not enough credit on card */
} RfidErrorCode;

// ============================================================================
// Credit/Unit Limits
// ============================================================================

/** Maximum credit in seconds (~16.6 hours) - limited by uint16_t storage */
static const unsigned int MAX_CREDIT_SECONDS = 60000;

/** Minimum credit (seconds) required to operate engine */
static const unsigned int MIN_CREDIT_THRESHOLD = 10;

/** Seconds per minute - for unit conversion */
static const unsigned int SECONDS_PER_MINUTE = 60;

// ============================================================================
// Speed Thresholds (km/h)
// ============================================================================

/** Low speed threshold - below this, use low deduction rate */
static const int SPEED_THRESHOLD_LOW  = 5;

/** High speed threshold - above this, use high deduction rate */
static const int SPEED_THRESHOLD_HIGH = 50;

// ============================================================================
// Credit Deduction Rates
// ============================================================================

/** Credit deduction rate at low speed (seconds per cycle) */
static const int DEDUCT_RATE_LOW      = 2;

/** Credit deduction rate at high speed (seconds per cycle) */
static const int DEDUCT_RATE_HIGH     = 3;

// ============================================================================
// GPS Configuration
// ============================================================================

/** GPS timeout in milliseconds (30 seconds) */
static const unsigned long GPS_TIMEOUT_MS = 30000;

/** GPS update interval in milliseconds */
static const unsigned long GPS_UPDATE_INTERVAL_MS = 2000;

/** Return value when GPS has no fix */
static const int GPS_NO_FIX = -1;

// ============================================================================
// Display Configuration
// ============================================================================

/** I2C address for LCD display */
static const uint8_t LCD_I2C_ADDRESS  = 0x27;

/** LCD columns count */
static const uint8_t LCD_COLS         = 20;

/** LCD rows count */
static const uint8_t LCD_ROWS         = 4;

// ============================================================================
// Character Validation (for name fields)
// ============================================================================

/** Start of valid alpha characters (ASCII 'A') */
static const uint8_t CHAR_ALPHA_START = 0x41;

/** End of valid alpha characters (ASCII 'z') */
static const uint8_t CHAR_ALPHA_END   = 0x7A;

/** Space character */
static const uint8_t CHAR_SPACE       = 0x20;

// ============================================================================
// Timing Constants
// ============================================================================

/** Short delay in milliseconds */
static const unsigned int DELAY_SHORT_MS     = 500;

/** Medium delay in milliseconds */
static const unsigned int DELAY_MEDIUM_MS    = 1000;

/** Long delay in milliseconds */
static const unsigned int DELAY_LONG_MS      = 2000;

/** Initialization delay in milliseconds */
static const unsigned int DELAY_INIT_MS      = 4200;

// ============================================================================
// Pin Definitions (can be overridden by project)
// ============================================================================

#ifndef PIN_ENGINE_SWITCH
/** Pin for engine relay switch */
#define PIN_ENGINE_SWITCH  6
#endif

#ifndef PIN_RPM_INPUT
/** Pin for RPM input from engine */
#define PIN_RPM_INPUT      13
#endif

#ifndef PIN_BUZZER
/** Pin for buzzer/alarm */
#define PIN_BUZZER         1
#endif

// ============================================================================
// PN532 NFC Configuration (can be overridden by project)
// ============================================================================

#ifndef PN532_IRQ
/** PN532 interrupt pin (I2C mode) */
#define PN532_IRQ          6
#endif

#ifndef PN532_RESET
/** PN532 reset pin (I2C mode) */
#define PN532_RESET        5
#endif

#ifndef PN532_SS
/** PN532 slave select pin (SPI mode) */
#define PN532_SS           10
#endif

// ============================================================================
// Mifare Sector Definitions
// ============================================================================

/** Number of short sectors in Mifare Classic */
static const uint8_t NR_SHORTSECTOR          = 32;

/** Number of long sectors in Mifare Classic */
static const uint8_t NR_LONGSECTOR           = 8;

/** Blocks per short sector */
static const uint8_t NR_BLOCK_OF_SHORTSECTOR = 4;

/** Blocks per long sector */
static const uint8_t NR_BLOCK_OF_LONGSECTOR  = 16;

#endif // UNIT_COUNTER_CONFIG_H
