/**
 * @file CreditManager.cpp
 * @brief Implementation of credit/unit management operations
 */

#include "CreditManager.h"

CreditManager::CreditManager(RfidManager* rfidManager)
    : _rfidManager(rfidManager), _lastError(ERR_SUCCESS) {
    memset(_buffer, 0, BLOCK_SIZE);
}

unsigned int CreditManager::bytesToCredit(const uint8_t* bytes) {
    // Big-endian: high byte first
    return ((unsigned int)bytes[0] << 8) | bytes[1];
}

void CreditManager::creditToBytes(unsigned int credit, uint8_t* bytes) {
    // Big-endian: high byte first
    bytes[0] = (credit >> 8) & 0xFF;
    bytes[1] = credit & 0xFF;
}

bool CreditManager::isValidNameChar(uint8_t c) {
    return (c >= CHAR_ALPHA_START && c <= CHAR_ALPHA_END) || (c == CHAR_SPACE);
}

unsigned int CreditManager::getCredit() {
    _lastError = _rfidManager->readBlock(BLOCK_CREDIT, _buffer);

    if (_lastError != ERR_SUCCESS) {
        return 0;
    }

    return bytesToCredit(_buffer);
}

unsigned int CreditManager::getCreditMinutes() {
    return getCredit() / SECONDS_PER_MINUTE;
}

bool CreditManager::hasMinimumCredit() {
    return getCredit() >= MIN_CREDIT_THRESHOLD;
}

bool CreditManager::addCredit(unsigned int minutes) {
    // Read current credit
    unsigned int currentCredit = getCredit();
    if (_lastError != ERR_SUCCESS) {
        return false;
    }

    // Calculate new credit (convert minutes to seconds)
    unsigned long newCredit = (unsigned long)currentCredit + ((unsigned long)minutes * SECONDS_PER_MINUTE);

    // Check for overflow
    if (newCredit >= MAX_CREDIT_SECONDS) {
        _lastError = ERR_INSUFFICIENT_CREDIT;  // Reusing error for overflow indication
        return false;
    }

    // Prepare write buffer
    memset(_buffer, 0, BLOCK_SIZE);
    creditToBytes((unsigned int)newCredit, _buffer);

    // Write to card
    _lastError = _rfidManager->writeBlock(BLOCK_CREDIT, _buffer);
    return (_lastError == ERR_SUCCESS);
}

bool CreditManager::removeCredit(unsigned int seconds) {
    // Read current credit
    unsigned int currentCredit = getCredit();
    if (_lastError != ERR_SUCCESS) {
        return false;
    }

    // Check if sufficient credit
    unsigned int newCredit;
    if (currentCredit < seconds) {
        // Not enough credit - set to zero
        newCredit = 0;
        _lastError = ERR_INSUFFICIENT_CREDIT;
    } else {
        newCredit = currentCredit - seconds;
    }

    // Prepare write buffer
    memset(_buffer, 0, BLOCK_SIZE);
    creditToBytes(newCredit, _buffer);

    // Write to card
    RfidErrorCode writeResult = _rfidManager->writeBlock(BLOCK_CREDIT, _buffer);
    if (writeResult != ERR_SUCCESS) {
        _lastError = writeResult;
        return false;
    }

    // Return false if we had insufficient credit (even though we wrote zero)
    return (currentCredit >= seconds);
}

bool CreditManager::clearCredit() {
    // Prepare empty buffer
    memset(_buffer, 0, BLOCK_SIZE);

    // Write to card
    _lastError = _rfidManager->writeBlock(BLOCK_CREDIT, _buffer);
    return (_lastError == ERR_SUCCESS);
}

bool CreditManager::getName(uint8_t blockNumber, char* buffer) {
    _lastError = _rfidManager->readBlock(blockNumber, _buffer);

    if (_lastError != ERR_SUCCESS) {
        buffer[0] = '\0';
        return false;
    }

    // Copy valid characters to output buffer
    uint8_t i = 0;
    while (i < BLOCK_SIZE && _buffer[i] != 0 && isValidNameChar(_buffer[i])) {
        buffer[i] = (char)_buffer[i];
        i++;
    }
    buffer[i] = '\0';  // Null terminate

    return true;
}

bool CreditManager::setName(uint8_t blockNumber, const char* name) {
    // Prepare buffer with spaces (padding)
    memset(_buffer, CHAR_SPACE, BLOCK_SIZE);

    // Copy name to buffer (truncate if too long)
    uint8_t i = 0;
    while (i < BLOCK_SIZE && name[i] != '\0') {
        _buffer[i] = (uint8_t)name[i];
        i++;
    }

    // Write to card
    _lastError = _rfidManager->writeBlock(blockNumber, _buffer);
    return (_lastError == ERR_SUCCESS);
}
