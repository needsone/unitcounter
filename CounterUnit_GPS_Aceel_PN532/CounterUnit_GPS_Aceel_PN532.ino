/*
  Unit counter with RFID LCD screen and a relay
  - Manage authorisation with RFID tag to acceess to the counter
  - Fill unit un RFID card (60 minute or 195) with RFID master card
  - Create master* and slave cards !! key setting
  - Decrease the amount of Unit per minutes when the freq on RPM input > 1000 rpm)
  - *the lengh will increase of decrease depending ofn the RPM
  - Open the engine circuit (cswitch off the engine) whep$$$$$n we are at no more unit + 1 min and ring the buzzer
  - Possibility of changing the card during the run (1 min delay) $

  RFID card memory
  Name block 4
  Credit block 2
  Free for future usage
  Title id block 8  RFID_Master1


  The circuit: i2c !!!!
  SDA - 2
  SCL - 3
  §
  Signal :
  GPS   |
  Vin   | 5V
  Rx    | Tx
  Tx    | Rx

  Pin 13 RPM Brown getFreq

  - EEPROM
  - 5 bytes / master card - 4 (ID card) 5th type of card 0=1h 3=3h 6th / ...
  uint8_t mifareclassic_AuthenticateBlock (uint8_t * uid, uint8_t uidLen,
                              uint32_t blockNumber, uint8_t keyNumber,
                              uint8_t * keyData);
  uint8_t mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t * data);
  uint8_t mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t * data);

  Yun setting

  #define RST_PIN		13   // RFID pin
  #define SS_PIN		2  // RFID pin

**
** contact moteur off
** Noir = Masse
** Blanc = 5V (clean ??) Interupteur
** Bleu = 5V
**
** Marron = RPM:
**
*/

#include <Adafruit_GPS.h>
#include <Wire.h>

// #include <LiquidCrystal_I2C.h>
//#include "I2Cdev.h"

#include "OLedI2C.h"
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

#define PN532_SS   (10)
#define RST_PIN		       9  // RFID pin
#define SS_PIN		      10  // RFID pin
#define engineSwitch     6  // Engine Button
#define RpmInput        13  // Get RPM from Engine
#define buzzerPin        1  // Sound Alarm

#define LSM9DS0_XM_CS 10
#define LSM9DS0_GYRO_CS 9

// If using hardware serial:
//  Connect the GPS TX (transmit) pin to Arduino RX1 (Digital 0)
//  Connect the GPS RX (receive) pin to matching TX1 (Digital 1)
// If using hardware serial, comment
// out the above two lines and enable these two lines instead:

Adafruit_GPS GPS(&Serial1);
HardwareSerial mySerial = Serial1;
OLedI2C lcd;
#define GPSECHO  true;

float step_accel = 12;
float acc = 0;

// unsigned int block = 2;    //this is tardhe block number we will write  unit into and then read.
/* unsigned int block_master = 0; // master id  */
/* int address = 0; */

//byte value;
byte blockcontent[16];
byte readbackblock[16];
//byte readbackblockMaster[16];

boolean RFID_Master_Correct = 0; //true if Master fob is detected , false if not
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

uint8_t keya[6] =  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// uint8_t keya[6] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// uint8_t keya[6] =  { 0x42, 0x42, 0x42, 0x42, 0x42, 0x42 };
// uint8_t keya[6] =  { 0x42, 0x42, 0x42, 0x42, 0x43, 0x43 };

//LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 Yun for a 20 chars and 4 lines display
Adafruit_PN532 nfc(PN532_SS);
uint32_t timer = millis();
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(0x6B);  // Use I2C, ID #1000

void setup() {
  Serial.begin(115200);
  pinMode(engineSwitch, OUTPUT);
  digitalWrite(engineSwitch, HIGH);
  SPI.begin();			// Init SPI bus
  Wire.begin();
  lcd.init();                   // initialize the lcd
  lcd.setContrast( 0xFF);
  print("Initialization");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    lcd.clearLcd();
    print("No   PN53x board");
    while (1); // halt
  }
  /* Initialise the sensor */
  if (!lsm.begin())
  {
    /* There was a problem detecting the LSM9DS0 ... check your connections */
    Serial.print(F("Ooops, no LSM9DS0 detected ... Check your wiring or I2C ADDR!"));
    while (1);
  }
  displaySensorDetails();
  configureSensor();
  nfc.SAMConfig();

  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);   // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  mySerial.println(PMTK_Q_RELEASE);
}

void loop() {
  // nfc.SAMConfig();
  nfc.begin();
  float freqOn = getSpeed();
  lcd.clearLcd();
  print("Insert card ");
  uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  lcd.clearLcd();
  lcd.cursPos(0, 0);
  print("mn: ");
  lcd.cursPos(0, 1);
  printName(4);
  /*
    formatRfid();
    addUnit(60);
    lcd.cursPos(0,1);
    print("done");
    delay(42000);
  */
  while (int blocks = HowManyUnit() >= 10) {
    startEngine();
    sensors_event_t accel, mag, gyro, temp;
    lsm.getEvent(&accel, &mag, &gyro, &temp);
    acc = sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z + accel.acceleration.x * accel.acceleration.x);
    printCredit(freqOn, acc);
    /*
    **  if (acc > step_accel) {
    **  rmUnit(10);
    **  delay(10000);
    **    }
    */
    int step1 = 5;  //10 HIGH / 300 LOW
    int step2 = 50; //10 HIGH / 200 LOW
    freqOn = getSpeed();
    if (freqOn < step1) {
      rmUnit(2);
      delay(1000);
    } else {
      if (freqOn > step2) {
        rmUnit(3);
        delay(1000);
      } else {
        rmUnit(2);
        delay(1000);
      }

    }
     stopEngine();
    printCredit(freqOn, acc);
  }
}

