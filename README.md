# UnitCounter
-----------

## UnitCounter box :

Project made to run on Arduino Micro
 * Arduino micro v3
 * Adafruit_PN532
 * GPS fw5632 adafruit.it/746

## UnitCoutner master :
Project made to run on Arduino Yun
 * Arduino micro Yun
 * Adafruit_PN532

### GPS connections :
 *  If using hardware serial:
 * Connect the GPS TX (transmit) pin to Arduino RX1 (Digital 0)
 * Connect the GPS RX (receive) pin to matching TX1 (Digital 1)
 * If using hardware serial, comment

```
Adafruit_GPS GPS(&Serial1);
HardwareSerial mySerial = Serial1;
#define GPSECHO  false
```
