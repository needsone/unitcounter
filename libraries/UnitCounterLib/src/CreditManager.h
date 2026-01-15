/**
 * @file CreditManager.h
 * @brief Credit/unit management for RFID prepaid cards
 *
 * Handles credit operations including reading, adding, and removing
 * time credits stored on Mifare Classic cards.
 */

#ifndef CREDIT_MANAGER_H
#define CREDIT_MANAGER_H

#include <Arduino.h>
#include "RfidManager.h"
#include "UnitCounterConfig.h"

/**
 * @class CreditManager
 * @brief Manages credit operations on RFID cards
 *
 * Credit is stored as seconds in a 16-bit value (max ~18 hours).
 * This class provides high-level operations for credit management.
 */
class CreditManager {
public:
    /**
     * @brief Constructor
     * @param rfidManager Pointer to initialized RfidManager
     */
    CreditManager(RfidManager* rfidManager);

    /**
     * @brief Get current credit from card
     * @return Credit in seconds, or 0 on error
     */
    unsigned int getCredit();

    /**
     * @brief Get current credit in minutes
     * @return Credit in minutes (truncated)
     */
    unsigned int getCreditMinutes();

    /**
     * @brief Check if card has minimum credit to operate
     * @return true if credit >= MIN_CREDIT_THRESHOLD
     */
    bool hasMinimumCredit();

    /**
     * @brief Add credit to the card
     * @param minutes Amount to add in minutes
     * @return true if successful, false on error or overflow
     */
    bool addCredit(unsigned int minutes);

    /**
     * @brief Remove credit from the card
     * @param seconds Amount to remove in seconds
     * @return true if successful, false on error or insufficient credit
     */
    bool removeCredit(unsigned int seconds);

    /**
     * @brief Set credit to zero (used when credit exhausted)
     * @return true if successful
     */
    bool clearCredit();

    /**
     * @brief Read a name field from the card
     * @param blockNumber Block to read (BLOCK_USERNAME or BLOCK_CLUBNAME)
     * @param buffer Buffer to store name (must be BLOCK_SIZE+1 bytes for null terminator)
     * @return true if successful
     */
    bool getName(uint8_t blockNumber, char* buffer);

    /**
     * @brief Write a name field to the card
     * @param blockNumber Block to write (BLOCK_USERNAME or BLOCK_CLUBNAME)
     * @param name Name string (will be truncated to BLOCK_SIZE)
     * @return true if successful
     */
    bool setName(uint8_t blockNumber, const char* name);

    /**
     * @brief Get the last error code from operations
     * @return Last RfidErrorCode
     */
    RfidErrorCode getLastError() { return _lastError; }

private:
    RfidManager* _rfidManager;
    RfidErrorCode _lastError;
    uint8_t _buffer[BLOCK_SIZE];

    /**
     * @brief Convert 2 bytes to credit value (big-endian)
     * @param bytes Pointer to 2 bytes
     * @return Credit value in seconds
     */
    unsigned int bytesToCredit(const uint8_t* bytes);

    /**
     * @brief Convert credit value to 2 bytes (big-endian)
     * @param credit Credit value in seconds
     * @param bytes Output buffer for 2 bytes
     */
    void creditToBytes(unsigned int credit, uint8_t* bytes);

    /**
     * @brief Check if a character is valid for name fields
     * @param c Character to check
     * @return true if valid (A-Z, a-z, space)
     */
    bool isValidNameChar(uint8_t c);
};

#endif // CREDIT_MANAGER_H
