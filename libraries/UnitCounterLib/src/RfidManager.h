/**
 * @file RfidManager.h
 * @brief RFID/NFC card operations manager for Mifare Classic cards
 *
 * Handles all low-level RFID operations including authentication,
 * reading, and writing blocks on Mifare Classic cards.
 */

#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <Arduino.h>
#include <Adafruit_PN532.h>
#include "UnitCounterConfig.h"

/**
 * @class RfidManager
 * @brief Manages RFID card operations using PN532 NFC reader
 *
 * This class encapsulates all RFID operations including card detection,
 * authentication, and block read/write operations.
 */
class RfidManager {
public:
    /**
     * @brief Constructor for I2C mode (using IRQ and RESET pins)
     * @param irq IRQ pin number
     * @param reset RESET pin number
     */
    RfidManager(uint8_t irq, uint8_t reset);

    /**
     * @brief Constructor for SPI mode (using SS pin)
     * @param ss Slave Select pin number
     */
    RfidManager(uint8_t ss);

    /**
     * @brief Initialize the NFC reader
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Wait for a card to be presented
     * @param timeoutMs Timeout in milliseconds (0 = no timeout)
     * @return true if card detected, false on timeout
     */
    bool waitForCard(uint16_t timeoutMs = 0);

    /**
     * @brief Get the UID of the currently detected card
     * @return 32-bit card ID from first 4 bytes of UID
     */
    uint32_t getCardId();

    /**
     * @brief Get pointer to raw UID buffer
     * @return Pointer to UID byte array
     */
    uint8_t* getUid() { return _uid; }

    /**
     * @brief Get length of the UID
     * @return UID length in bytes (4 or 7)
     */
    uint8_t getUidLength() { return _uidLength; }

    /**
     * @brief Read a block from the card
     * @param blockNumber Block number to read (0-63 for 1K cards)
     * @param buffer Buffer to store read data (must be BLOCK_SIZE bytes)
     * @param useKeyB Use Key B instead of Key A for authentication
     * @return RfidErrorCode indicating success or failure type
     */
    RfidErrorCode readBlock(uint8_t blockNumber, uint8_t* buffer, bool useKeyB = false);

    /**
     * @brief Write data to a block on the card
     * @param blockNumber Block number to write (0-63 for 1K cards)
     * @param data Data to write (must be BLOCK_SIZE bytes)
     * @param useKeyB Use Key B instead of Key A for authentication
     * @return RfidErrorCode indicating success or failure type
     */
    RfidErrorCode writeBlock(uint8_t blockNumber, const uint8_t* data, bool useKeyB = false);

    /**
     * @brief Format the card by clearing data blocks
     * @return true if format successful
     */
    bool formatCard();

    /**
     * @brief Set the authentication key
     * @param key 6-byte authentication key
     */
    void setKey(const uint8_t* key);

    /**
     * @brief Get error message string for an error code
     * @param code Error code to translate
     * @return Human-readable error message
     */
    static const char* getErrorMessage(RfidErrorCode code);

private:
    Adafruit_PN532* _nfc;
    uint8_t _uid[UID_MAX_LENGTH];
    uint8_t _uidLength;
    uint8_t _key[KEY_LENGTH];
    bool _spiMode;
    uint8_t _irqPin;
    uint8_t _resetPin;
    uint8_t _ssPin;

    /**
     * @brief Authenticate with a block using stored key
     * @param blockNumber Block to authenticate
     * @param useKeyB Use Key B instead of Key A
     * @return true if authentication successful
     */
    bool authenticate(uint8_t blockNumber, bool useKeyB);
};

#endif // RFID_MANAGER_H
