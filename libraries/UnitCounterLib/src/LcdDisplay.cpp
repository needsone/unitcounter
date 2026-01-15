/**
 * @file LcdDisplay.cpp
 * @brief Implementation of LCD display wrapper
 */

#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(uint8_t address, uint8_t cols, uint8_t rows)
    : _lcd(address, cols, rows), _cols(cols), _rows(rows) {
}

void LcdDisplay::init() {
    _lcd.init();
    _lcd.home();
    _lcd.backlight();
}

void LcdDisplay::clear() {
    _lcd.clear();
}

void LcdDisplay::setCursor(uint8_t col, uint8_t row) {
    _lcd.setCursor(col, row);
}

void LcdDisplay::print(const char* str) {
    _lcd.print(str);
}

void LcdDisplay::print(const __FlashStringHelper* str) {
    _lcd.print(str);
}

void LcdDisplay::print(int value) {
    _lcd.print(value);
}

void LcdDisplay::print(unsigned int value) {
    _lcd.print(value);
}

void LcdDisplay::print(long value) {
    _lcd.print(value);
}

void LcdDisplay::print(unsigned long value) {
    _lcd.print(value);
}

void LcdDisplay::print(float value, int decimals) {
    _lcd.print(value, decimals);
}

void LcdDisplay::printChar(char c) {
    _lcd.write(c);
}

void LcdDisplay::backlightOn() {
    _lcd.backlight();
}

void LcdDisplay::backlightOff() {
    _lcd.noBacklight();
}
