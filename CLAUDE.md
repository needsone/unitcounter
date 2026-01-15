# UnitCounter - Claude Code Context

## Project Overview

UnitCounter is an RFID-based time/credit management system for a waterski club at Lac de Joux, Switzerland. It functions as a taxi-meter for boats - users have prepaid NFC cards with time credits that are deducted based on boat speed.

## Architecture

Two Arduino projects share common functionality through a library:

```
unitcounter/
├── CounterUnitMaster/          # Base station (Arduino Yun)
├── CounterUnit_GPS_Aceel_PN532/  # Onboard unit (Arduino Micro)
└── libraries/UnitCounterLib/   # Shared library
```

### CounterUnitMaster (Arduino Yun)
- **Purpose**: Base station for adding/managing credit on cards
- **Hardware**: Arduino Yun, PN532 (I2C), LCD 20x4 I2C
- **Features**: REST API via Bridge library, credit management
- **Main file**: `CounterUnitMasterJoux.ino`

### CounterUnit_GPS_Aceel_PN532 (Arduino Micro)
- **Purpose**: Onboard boat unit that deducts credit based on speed
- **Hardware**: Arduino Micro, PN532 (SPI), OLED I2C, GPS, LSM9DS0 accelerometer
- **Features**: GPS speed tracking, engine relay control
- **Main file**: `CounterUnit_GPS_Aceel_PN532.ino`

## RFID Card Memory Layout (Mifare Classic)

| Block | Content | Format |
|-------|---------|--------|
| 2 | Credit | Big-endian uint16 (seconds) |
| 4 | Customer name | 16 bytes ASCII |
| 6 | Club name | 16 bytes ASCII |
| 8 | Master card ID | Identifier |

## Key Constants

```cpp
// Block numbers
BLOCK_CREDIT     2
BLOCK_NAME       4
BLOCK_CLUB       6
BLOCK_MASTER     8

// Credit limits
MAX_CREDIT       60000   // ~16.6 hours in seconds
MIN_CREDIT       10      // Minimum to operate

// Speed thresholds (km/h)
SPEED_LOW        5       // Below: low deduction
SPEED_HIGH       50      // Above: high deduction

// Deduction rates (seconds per cycle)
DEDUCT_LOW       2
DEDUCT_HIGH      3

// Timeouts
GPS_TIMEOUT_MS   30000   // 30 seconds
```

## REST API (Master only)

Base URL: `http://<yun-ip>/arduino/`

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/add/<minutes>` | GET | Add credit in minutes |
| `/rm/<seconds>` | GET | Remove credit in seconds |
| `/credit` | GET | Get current credit |

## Common Patterns

### Credit byte conversion (big-endian)
```cpp
// Read credit from card
unsigned int credit = ((unsigned int)data[0] << 8) | data[1];

// Write credit to card
data[0] = (credit >> 8) & 0xFF;
data[1] = credit & 0xFF;
```

### RFID authentication
```cpp
uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keya);
```

### Error codes
- 1: Success
- 3: Authentication fail
- 4: Read fail
- 5: Write fail

## Development Notes

- Use `F()` macro for string literals to save RAM
- Credit stored in seconds, displayed in minutes
- GPS speed in knots, converted to km/h (* 1.852)
- Engine relay: LOW = running, HIGH = stopped
- Always reinitialize NFC at start of each loop cycle

## Testing

1. Compile both projects in Arduino IDE
2. Master: Test REST endpoints with curl
3. GPS unit: Test with card and monitor Serial output
4. Verify credit read/write operations
5. Check GPS timeout behavior (remove antenna)

## Dependencies

- Adafruit_PN532
- Adafruit_GPS
- Adafruit_LSM9DS0
- Adafruit_Sensor
- LiquidCrystal_I2C
- Bridge (built-in for Yun)
