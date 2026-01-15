/**
 * @file LcdDisplay.h
 * @brief LCD display implementation using LiquidCrystal_I2C
 *
 * Implementation of DisplayInterface for I2C LCD displays
 * commonly used with Arduino Yun.
 */

#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include "DisplayInterface.h"
#include "UnitCounterConfig.h"

/**
 * @class LcdDisplay
 * @brief DisplayInterface implementation for I2C LCD displays
 */
class LcdDisplay : public DisplayInterface {
public:
    /**
     * @brief Constructor
     * @param address I2C address (default 0x27)
     * @param cols Number of columns (default 20)
     * @param rows Number of rows (default 4)
     */
    LcdDisplay(uint8_t address = LCD_I2C_ADDRESS, uint8_t cols = LCD_COLS, uint8_t rows = LCD_ROWS);

    void init() override;
    void clear() override;
    void setCursor(uint8_t col, uint8_t row) override;

    void print(const char* str) override;
    void print(const __FlashStringHelper* str) override;
    void print(int value) override;
    void print(unsigned int value) override;
    void print(long value) override;
    void print(unsigned long value) override;
    void print(float value, int decimals = 2) override;
    void printChar(char c) override;

    void backlightOn() override;
    void backlightOff() override;

    uint8_t getCols() override { return _cols; }
    uint8_t getRows() override { return _rows; }

    /**
     * @brief Get direct access to underlying LCD object
     * @return Reference to LiquidCrystal_I2C object
     */
    LiquidCrystal_I2C& getLcd() { return _lcd; }

private:
    LiquidCrystal_I2C _lcd;
    uint8_t _cols;
    uint8_t _rows;
};

#endif // LCD_DISPLAY_H
