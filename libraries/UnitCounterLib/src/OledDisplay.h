/**
 * @file OledDisplay.h
 * @brief OLED display implementation using OLedI2C (header-only)
 *
 * Implementation of DisplayInterface for I2C OLED displays
 * used with Arduino Micro in the GPS version.
 *
 * Note: Include OLedI2C.h BEFORE including this header.
 */

#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include "DisplayInterface.h"

/**
 * @class OledDisplay
 * @brief DisplayInterface implementation for I2C OLED displays
 *
 * This is a header-only implementation to avoid linking issues.
 * Requires OLedI2C.h to be included before this header.
 */
class OledDisplay : public DisplayInterface {
public:
    /**
     * @brief Constructor
     * @param oled Reference to OLedI2C object
     * @param cols Number of columns (default 16)
     * @param rows Number of rows (default 2)
     */
    OledDisplay(OLedI2C& oled, uint8_t cols = 16, uint8_t rows = 2)
        : _oled(oled), _cols(cols), _rows(rows) {}

    void init() override {
        _oled.init();
    }

    void clear() override {
        _oled.clearLcd();
    }

    void setCursor(uint8_t col, uint8_t row) override {
        _oled.cursPos(col, row);
    }

    void print(const char* str) override {
        while (*str) {
            _oled.sendData(*str++);
        }
    }

    void print(const __FlashStringHelper* str) override {
        const char* p = (const char*)str;
        char c;
        while ((c = pgm_read_byte(p++))) {
            _oled.sendData(c);
        }
    }

    void print(int value) override {
        char buffer[12];
        itoa(value, buffer, 10);
        print(buffer);
    }

    void print(unsigned int value) override {
        char buffer[12];
        utoa(value, buffer, 10);
        print(buffer);
    }

    void print(long value) override {
        char buffer[12];
        ltoa(value, buffer, 10);
        print(buffer);
    }

    void print(unsigned long value) override {
        char buffer[12];
        ultoa(value, buffer, 10);
        print(buffer);
    }

    void print(float value, int decimals = 2) override {
        char buffer[16];
        dtostrf(value, 0, decimals, buffer);
        print(buffer);
    }

    void printChar(char c) override {
        _oled.sendData(c);
    }

    void setContrast(uint8_t contrast) override {
        _oled.setContrast(contrast);
    }

    uint8_t getCols() override { return _cols; }
    uint8_t getRows() override { return _rows; }

    /**
     * @brief Get direct access to underlying OLED object
     * @return Reference to OLedI2C object
     */
    OLedI2C& getOled() { return _oled; }

private:
    OLedI2C& _oled;
    uint8_t _cols;
    uint8_t _rows;
};

#endif // OLED_DISPLAY_H
