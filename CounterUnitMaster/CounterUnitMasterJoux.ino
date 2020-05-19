/*
   Unit counter with RFID LCD screen and a relay
   - Manage authorisation with RFID tag to acceess to the counter
   - Fill unit un RFID card (60 minute or 195) with RFID master card
   - Create master* and slave cards !! key setting
   - Decrease the amount of Unit per minutes when the freq on RPM input > 1000 rpm)
   - the lengh will increase of decrease depending ofn the RPM
   - Open the engine circuit (cswitch off the engine) when we are at no more unit + 1 min and ring the buzzer
   - Possibility of changing the card during the run (1 min delay)



  RFID card memory :
  ------------------
  Name of the customer 6
  Name  of the club 4
  Credit block 2
  Free for future usage
  Title id block 8  RFID_Master1

  The circuit: i2c !!!!
  SDA - 2
  SCL - 3
  #define PN532_IRQ   (6)

  Signal :
  GPS   |
  Rx    | Tx
  Tx    | Rx
  or
  The circuit:
    LCD :
    i2c

  - EEPROM
  - 5 bytes / master card - 4 (ID card) 5th type of card 0=1h 3=3h 6th / ...

  uint8_t mifareclassic_AuthenticateBlock (uint8_t * uid, uint8_t uidLen,
                             uint32_t blockNumber, uint8_t keyNumber,
                             uint8_t * keyData);
  uint8_t mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t * data);
  uint8_t mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t * data);
  Yun setting
  #define RST_PIN    13   // RFID pin
  #define SS_PIN    2  // RFID pin

** contact moteur off
** Noir = Masse
** Blanc = 5V (clean ??) Interupteur
** Bleu = 5V
**
** Marron = RPM:GPS GPS

  root@Arduino:/usr/bin# cat > getIp
  #!/bin/ash

  my_ip=`ifconfig  | grep  inet | grep -v '127.0.0.1'    |  cut -d : -f 2 | cut -d ' ' -f 1`
  echo $my_ip
  ##

  #!/bin/ash

  cat /etc/config/wireless  | grep ssid | cut  -d \' -f 2



  https://www.arduino.cc/en/Reference/YunBridgeLibrary

**
*/

#include <Adafruit_GPS.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

// #include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Process.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

#define PN532_IRQ   (6)
#define PN532_RESET (5)  // Not connected by default on the NFC Shield

//#define RST_PIN    9  // RFID pin
//#define PN532_SS  7
// RFID pin
#define engineSwitch     8  // Engine Button
#define RpmInput        13  // Get RPM from Engine
#define buzzerPin        1  // Sound Alarm

// If using hardware serial:
//  Connect the GPS TX (transmit) pin to Arduino RX1 (Digital 0)
//  Connect the GPS RX (receive) pin to matching TX1 (Digital 1)
// If using hardware serial, comment
// out the above two lines and enable these two lines instead:
/*
   unsigned int block = 2;    //this is the block number we will write  unit into and then read.
*/
/* unsigned int block_master = 0; // master id  */
/* int address = 0; */

byte value;
byte blockcontent[16];
boolean test_nfc = 0;
byte readbackblock[16] =  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
byte readbackblockMaster[16];
// byte username[16] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
byte clubname[16] = {'L', 'A', 'C', ' ', 'D', 'E', ' ', 'J', 'O', 'U', 'X', ' ', ' ', ' ', ' ', ' '};

boolean RFID_Master_Correct = 0; //true if Master fob is detected , false if not
uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;  // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

//int sensorValue = 0;
BridgeServer server;
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 Yun for a 20 chars and 4 lines display
//Adafruit_PN532 nfc(PN532_SS);
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
//Adafruit_PN532 nfc(PN532_IRQ);

uint32_t timer = millis();

void setup() {
  Bridge.begin();
//  SPI.begin();      // Init SPI bus
//  delay(1000);
  lcd.init(); // initialize the lcd
  lcd.home();
  lcd.backlight();
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    lcd.clear();
    lcd.print("Didn't find PN53x");
    while (1); // halt
  }
  server.listenOnLocalhost();
  server.begin();
  Serial.begin(9600);
  nfc.SAMConfig();
  GetIp();
  delay(4200);
}

void loop(void) {
  BridgeClient client = server.accept();
  lcd.clear();
  printCredit(0);

  uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    BridgeClient client = server.accept();
    if (client) {
      process(client);
      client.stop();
    }
    delay(500);
  }
}

void process(BridgeClient client) {
  String command = client.readStringUntil('/');

  /*
  nfc.begin();
  lcd.clear();
  GetIp();
  lcd.print("Insert a card...  ");
  */
  if (command == "credit") {
    HowManyUnit();
  }
  if (command == "add") {
    addCommand(client);

  }
  if (command == "rm") {
    rmCommand(client);
  }
  if (command == "format") {
    ndef_to_classic();
    formatRfid();
  }
  if (command == "init") {
    // DEBUG
    initRfid(client);
  }
  if (command == "ip") {
    GetIp();
  }
  if (command == "id") {
    client.print(GetId(client));
  }
 lcd.clear();
  printCredit(0);
  delay(500);
}

void addCommand(BridgeClient client) {
  int value;

  if (value = client.parseInt()) {
    lcd.clear();
    lcd.print("You will add: ");
    lcd.print(value);
    lcd.print("mn");
    nfc.begin();
    int success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    if (!success) return;
    addUnit(value); // minutes
  }  else {
    lcd.clear();
    lcd.print("wrong");
  }
}

void rmCommand(BridgeClient client) {
  int value, sec;

  if (value = client.parseInt()) {
    lcd.clear();
    lcd.print("You will remove: ");
    lcd.print(value);
    lcd.print("mn");
    nfc.begin();
    int success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    if (!success) return;
    int sec = value * 60;
    rmUnit(sec); // minutes
  }  else {
    lcd.clear();
    lcd.print("wrong");
  }
}
