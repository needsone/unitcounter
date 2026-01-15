/**
 * @file RfidManager.cpp
 * @brief Implementation of RFID/NFC card operations manager
 */

#include "RfidManager.h"

// Constructor for I2C mode
RfidManager::RfidManager(uint8_t irq, uint8_t reset)
    : _spiMode(false), _irqPin(irq), _resetPin(reset), _ssPin(0), _uidLength(0) {
    _nfc = new Adafruit_PN532(irq, reset);
    memcpy(_key, DEFAULT_KEY, KEY_LENGTH);
    memset(_uid, 0, UID_MAX_LENGTH);
}

// Constructor for SPI mode
RfidManager::RfidManager(uint8_t ss)
    : _spiMode(true), _irqPin(0), _resetPin(0), _ssPin(ss), _uidLength(0) {
    _nfc = new Adafruit_PN532(ss);
    memcpy(_key, DEFAULT_KEY, KEY_LENGTH);
    memset(_uid, 0, UID_MAX_LENGTH);
}

bool RfidManager::begin() {
    _nfc->begin();

    uint32_t versiondata = _nfc->getFirmwareVersion();
    if (!versiondata) {
        return false;
    }

    // Configure to read RFID tags
    _nfc->SAMConfig();
    return true;
}

bool RfidManager::waitForCard(uint16_t timeoutMs) {
    uint8_t success;

    if (timeoutMs == 0) {
        // No timeout - blocking call
        success = _nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, _uid, &_uidLength);
    } else {
        // With timeout
        success = _nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, _uid, &_uidLength, timeoutMs);
    }

    return success;
}

uint32_t RfidManager::getCardId() {
    if (_uidLength < 4) {
        return 0;
    }

    // Combine first 4 bytes into a 32-bit card ID
    uint32_t cardId = ((uint32_t)_uid[0] << 24) |
                      ((uint32_t)_uid[1] << 16) |
                      ((uint32_t)_uid[2] << 8)  |
                      ((uint32_t)_uid[3]);
    return cardId;
}

void RfidManager::setKey(const uint8_t* key) {
    memcpy(_key, key, KEY_LENGTH);
}

bool RfidManager::authenticate(uint8_t blockNumber, bool useKeyB) {
    uint8_t keyType = useKeyB ? 1 : 0;
    return _nfc->mifareclassic_AuthenticateBlock(_uid, _uidLength, blockNumber, keyType, _key);
}

RfidErrorCode RfidManager::readBlock(uint8_t blockNumber, uint8_t* buffer, bool useKeyB) {
    // First, authenticate with the block
    if (!authenticate(blockNumber, useKeyB)) {
        return ERR_AUTH_FAIL;
    }

    // Then read the block
    if (_nfc->mifareclassic_ReadDataBlock(blockNumber, buffer)) {
        return ERR_SUCCESS;
    }

    return ERR_READ_FAIL;
}

RfidErrorCode RfidManager::writeBlock(uint8_t blockNumber, const uint8_t* data, bool useKeyB) {
    // First, authenticate with the block
    if (!authenticate(blockNumber, useKeyB)) {
        return ERR_AUTH_FAIL;
    }

    // Then write the block
    if (_nfc->mifareclassic_WriteDataBlock(blockNumber, (uint8_t*)data)) {
        return ERR_SUCCESS;
    }

    return ERR_WRITE_FAIL;
}

bool RfidManager::formatCard() {
    uint8_t emptyBlock[BLOCK_SIZE] = {0};

    // Clear the main data blocks
    // Note: We avoid sector trailer blocks (3, 7, 11, etc.)
    const uint8_t blocksToFormat[] = {BLOCK_MASTER_ID, BLOCK_USERNAME, BLOCK_CLUBNAME, BLOCK_CREDIT, 1, 5};

    for (uint8_t i = 0; i < sizeof(blocksToFormat); i++) {
        RfidErrorCode result = writeBlock(blocksToFormat[i], emptyBlock);
        if (result != ERR_SUCCESS) {
            return false;
        }
    }

    return true;
}

const char* RfidManager::getErrorMessage(RfidErrorCode code) {
    switch (code) {
        case ERR_SUCCESS:
            return "Success";
        case ERR_READER_PROBLEM:
            return "Reader problem";
        case ERR_AUTH_FAIL:
            return "Auth failed";
        case ERR_READ_FAIL:
            return "Read failed";
        case ERR_WRITE_FAIL:
            return "Write failed";
        case ERR_NO_CARD:
            return "No card";
        case ERR_INSUFFICIENT_CREDIT:
            return "Not enough credit";
        default:
            return "Unknown error";
    }
}
