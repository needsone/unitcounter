/**
 * @file UnitCounterLib.h
 * @brief Main header for UnitCounter RFID credit management library
 *
 * UnitCounter is an RFID-based time/credit management system designed
 * for controlling access to equipment (boats, vehicles, etc.). Users
 * present NFC/RFID cards with prepaid credits that are deducted based
 * on usage time and speed.
 *
 * @author Louis Poncet
 * @version 1.0.0
 *
 * @section Features
 * - RFID card credit management using Mifare Classic cards
 * - Support for PN532 NFC reader (I2C and SPI modes)
 * - Configurable deduction rates based on speed
 * - Maximum credit limit to prevent overflow
 *
 * @section Usage
 * @code
 * #include <UnitCounterLib.h>
 *
 * // Create managers
 * RfidManager rfid(PN532_IRQ, PN532_RESET);  // I2C mode
 * CreditManager credit(&rfid);
 *
 * void setup() {
 *     rfid.begin();
 * }
 *
 * void loop() {
 *     if (rfid.waitForCard(1000)) {
 *         unsigned int minutes = credit.getCreditMinutes();
 *         // Use the credit...
 *     }
 * }
 * @endcode
 */

#ifndef UNIT_COUNTER_LIB_H
#define UNIT_COUNTER_LIB_H

// Include all library components
#include "UnitCounterConfig.h"
#include "RfidManager.h"
#include "CreditManager.h"
#include "DisplayInterface.h"
#include "LcdDisplay.h"
// Note: Include OLedI2C.h before OledDisplay.h when using OLED
// #include "OledDisplay.h"

#endif // UNIT_COUNTER_LIB_H
