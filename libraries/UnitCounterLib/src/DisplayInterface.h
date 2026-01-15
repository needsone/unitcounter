/**
 * @file DisplayInterface.h
 * @brief Abstract display interface for LCD and OLED displays
 *
 * Provides a common interface for different display types used
 * in UnitCounter projects.
 */

#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

#include <Arduino.h>

/**
 * @class DisplayInterface
 * @brief Abstract base class for display operations
 *
 * This interface allows the same code to work with different
 * display hardware (I2C LCD, OLED, etc.).
 */
class DisplayInterface {
public:
    virtual ~DisplayInterface() {}

    /**
     * @brief Initialize the display
     */
    virtual void init() = 0;

    /**
     * @brief Clear the entire display
     */
    virtual void clear() = 0;

    /**
     * @brief Set cursor position
     * @param col Column (0-based)
     * @param row Row (0-based)
     */
    virtual void setCursor(uint8_t col, uint8_t row) = 0;

    /**
     * @brief Print a string
     * @param str String to print
     */
    virtual void print(const char* str) = 0;

    /**
     * @brief Print a string from flash memory
     * @param str String in PROGMEM
     */
    virtual void print(const __FlashStringHelper* str) = 0;

    /**
     * @brief Print an integer
     * @param value Integer value to print
     */
    virtual void print(int value) = 0;

    /**
     * @brief Print an unsigned integer
     * @param value Unsigned integer value to print
     */
    virtual void print(unsigned int value) = 0;

    /**
     * @brief Print a long integer
     * @param value Long integer value to print
     */
    virtual void print(long value) = 0;

    /**
     * @brief Print an unsigned long integer
     * @param value Unsigned long integer value to print
     */
    virtual void print(unsigned long value) = 0;

    /**
     * @brief Print a floating point number
     * @param value Float value to print
     * @param decimals Number of decimal places (default 2)
     */
    virtual void print(float value, int decimals = 2) = 0;

    /**
     * @brief Print a single character
     * @param c Character to print
     */
    virtual void printChar(char c) = 0;

    /**
     * @brief Turn on display backlight (if supported)
     */
    virtual void backlightOn() {}

    /**
     * @brief Turn off display backlight (if supported)
     */
    virtual void backlightOff() {}

    /**
     * @brief Set display contrast (if supported)
     * @param contrast Contrast level (0-255)
     */
    virtual void setContrast(uint8_t contrast) {}

    /**
     * @brief Get number of columns
     * @return Number of display columns
     */
    virtual uint8_t getCols() = 0;

    /**
     * @brief Get number of rows
     * @return Number of display rows
     */
    virtual uint8_t getRows() = 0;
};

#endif // DISPLAY_INTERFACE_H
