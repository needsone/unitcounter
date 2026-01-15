# UnitCounter

Taxi-meter style billing system for boats at Lac de Joux waterski club. Users have prepaid RFID cards with time credits that are deducted based on boat speed.

**Project page**: https://www.hackster.io/louis-poncet/unitcounter-70e3b1

## System Components

### 1. CounterUnitMaster (Base Station)

Credit management terminal for adding time to customer cards.

**Hardware:**
- Arduino Yun (ATmega32U4 + Linux)
- PN532 NFC/RFID reader (I2C mode)
- LCD 20x4 I2C display

**Features:**
- REST API for credit operations
- Add/remove credit from cards
- Display customer info and balance

### 2. CounterUnit_GPS (Onboard Unit)

Mounted on boats to track usage and deduct credits.

**Hardware:**
- Arduino Micro
- PN532 NFC/RFID reader (SPI mode)
- OLED I2C display
- Adafruit GPS module
- LSM9DS0 9-DOF accelerometer
- Engine relay

**Features:**
- GPS speed tracking
- Speed-based credit deduction
- Engine control (stops when credit depleted)

## Wiring

### PN532 NFC Reader

**Master (I2C mode):**
| PN532 | Arduino Yun |
|-------|-------------|
| SDA | SDA |
| SCL | SCL |
| VCC | 5V |
| GND | GND |

**GPS Unit (SPI mode):**
| PN532 | Arduino Micro |
|-------|---------------|
| SCK | SCK |
| MISO | MISO |
| MOSI | MOSI |
| SS | Pin 10 |
| VCC | 5V |
| GND | GND |

### GPS Module (GPS Unit only)

| GPS | Arduino Micro |
|-----|---------------|
| TX | RX1 (Pin 0) |
| RX | TX1 (Pin 1) |
| VCC | 5V |
| GND | GND |

### Engine Relay (GPS Unit only)

| Relay | Arduino Micro |
|-------|---------------|
| Signal | Pin 6 |
| VCC | 5V |
| GND | GND |

## RFID Card Format

Uses Mifare Classic 1K cards with the following memory layout:

| Block | Content | Format |
|-------|---------|--------|
| 2 | Credit balance | Big-endian uint16 (seconds) |
| 4 | Customer name | ASCII string (16 bytes) |
| 6 | Club name | ASCII string (16 bytes) |
| 8 | Master card ID | Card identifier |

Default authentication key: `FF FF FF FF FF FF`

## Credit System

- Credit stored in **seconds**
- Displayed in **minutes**
- Maximum: 60,000 seconds (~16.6 hours)
- Minimum to operate: 10 seconds

### Deduction Rates

| Speed | Rate |
|-------|------|
| < 5 km/h | 2 sec/cycle |
| 5-50 km/h | 2 sec/cycle |
| > 50 km/h | 3 sec/cycle |

## REST API (Master only)

Access via `http://<yun-ip>/arduino/`

| Endpoint | Description | Response |
|----------|-------------|----------|
| `GET /add/<minutes>` | Add credit | New balance |
| `GET /rm/<seconds>` | Remove credit | New balance |
| `GET /credit` | Get balance | Current credit |

### Examples

```bash
# Add 30 minutes
curl http://192.168.1.100/arduino/add/30

# Remove 60 seconds
curl http://192.168.1.100/arduino/rm/60

# Check balance
curl http://192.168.1.100/arduino/credit
```

## Installation

### Dependencies

Install via Arduino Library Manager:
- Adafruit PN532
- Adafruit GPS Library
- Adafruit LSM9DS0 Library
- Adafruit Unified Sensor
- LiquidCrystal I2C

### Build

1. Open the project folder for your target board:
   - `CounterUnitMaster/` for Arduino Yun
   - `CounterUnit_GPS_Aceel_PN532/` for Arduino Micro

2. Select the correct board in Arduino IDE:
   - **Master**: Arduino Yun
   - **GPS Unit**: Arduino Micro

3. Compile and upload

### Shared Library (Optional)

The `libraries/UnitCounterLib/` folder contains reusable components:

```
libraries/UnitCounterLib/
├── src/
│   ├── UnitCounterConfig.h   # Configuration constants
│   ├── RfidManager.h/.cpp    # RFID operations
│   ├── CreditManager.h/.cpp  # Credit management
│   ├── DisplayInterface.h    # Display abstraction
│   ├── LcdDisplay.h/.cpp     # LCD implementation
│   └── OledDisplay.h         # OLED implementation
├── library.properties
└── keywords.txt
```

Copy to your Arduino libraries folder to use.

## Usage

### Base Station (Master)

1. Power on the unit
2. Place customer card on reader
3. Use REST API or serial commands to add credit
4. Remove card

### Boat Unit (GPS)

1. Insert card before starting
2. Engine enables when card detected with sufficient credit
3. Credit deducts automatically based on speed
4. Engine stops when credit is depleted
5. Remove card to end session

## Troubleshooting

| Issue | Solution |
|-------|----------|
| "No PN53x board!" | Check NFC reader wiring and power |
| "GPS timeout!" | Check GPS antenna, wait for satellite fix |
| "Auth Fail" | Verify card is Mifare Classic with default key |
| "Low credit" | Add more credit at base station |
| Engine won't start | Check relay wiring, verify credit > 10 sec |

## Project History

- Original development: 2016
- Refactored: 2025 (bug fixes, shared library, documentation)

## License

MIT License

## Author

Louis Poncet - Lac de Joux Waterski Club
