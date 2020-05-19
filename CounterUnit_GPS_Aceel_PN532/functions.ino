/*
void initRfid() {
  byte username[16] = {'L', 'A', ' ', 'P', 'E', 'R', 'L', 'E', ' ', 'D', 'U', ' ', 'L', 'A', 'C', ' '};
  fillData(4, username);
  printName(4);
}
*/

int getSpeed() {
  // put your main code here, to run repeatedly:  char c = GPS.read();
  // float speed = 0;

  while (true) {
    char c = GPS.read();
    int speed = 0;

    if (GPS.newNMEAreceived()) {
      if (GPS.parse(GPS.lastNMEA()))  {
        // th is also sets the newNMEAreceived() flag to false
        // if millis() or timer wraps around, we'll just reset it
        if (timer > millis()) timer = millis();
        // approximately every 2 seconds or so, print out the current stats
        if (millis() - timer > 2000) {
          timer = millis(); // reset the timer
          //    Serial.print(GPS.day, DEC); Serial.print('/');
          if (GPS.fix) {
            float speed  = GPS.speed * 1.852; // knots to km/h
            // Serial.print(GPS.minute, DEC); Serial.print(':');
            Serial.print("Sec : ");
            Serial.println(GPS.seconds, DEC);
            //Serial.print("  Speed : ");
            // Serial.println(GPS.speed);
            // Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
            return (speed);
          } else {
            lcd.clearLcd();
            print("GPS SEARCHING");
            lcd.cursPos(0, 1);
          }
        }
      }
    }
  }
  return (0);
}

void displaySensorDetails(void)
{
  sensor_t accel, mag, gyro, temp;
  lsm.getSensor(&accel, &mag, &gyro, &temp);
}

void configureSensor(void)
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
  // lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);

  // 2.) Set the magnetometer sensitivity
  //  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_12GAUSS);

  // 3.) Setup the gyroscope
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS);
}

// return the value of the block content
unsigned int HowManyUnit () {
  int block = 2;
  unsigned int retRead = read_block(block, keya, readbackblock, 0);

  if (retRead == 3 || retRead == 4) {
    return 0;
  }
  unsigned int readValue = readbackblock[1] + readbackblock[0] * 256;
  return (readValue);
}
/*
//  YES !!!!
unsigned int getCardId () {
  // Get the id of the card 4 first bytes of the block 0 and set an int with it
  unsigned int retRead = readBlock(0, readbackblock);

  if (retRead == 3 || retRead == 4) {
    lcd.clear();
    lcd.print("No Card 1");
    return 0;
  }
}
*/

void print(const char *String)
{
  unsigned char i = 0;
  while (String[i])
  {
    lcd.sendData(String[i]);      // *** Show String to OLED
    i++;
  }
}

void print(String s)
{
  unsigned char i = 0;
  while (s[i])
  {
    lcd.sendData(s[i]);      // *** Show String to OLED
    i++;
  }
}

void print(unsigned int val) {
  String str = String(val);
  print(str);
}

// In rmUnit amount is in sec
boolean rmUnit (int amount) {
  int block = 2;

  uint8_t  retRead = read_block(block, keya, readbackblock, 0);
  if (retRead == 3 || retRead == 4) {
    lcd.clearLcd();
    print("No Card rm");
    return 0;
  }

  unsigned int readValue = readbackblock[1] + readbackblock[0] * 256;
  if (readValue < amount) {
    lcd.clearLcd();
    print("Nout enough Unit :");
    lcd.cursPos(0, 1);
    print(readValue - amount);
    blockcontent[1] = 0;
    blockcontent[0] = 0;
    uint8_t  retWrite = write_block(block, keya, blockcontent, 0);
    return 0;
  }
  unsigned int writeValue = readValue - amount;
  blockcontent[1] = writeValue;
  blockcontent[0] = writeValue / 256;
  uint8_t  retWrite = write_block(block, keya, blockcontent, 0);
  if (retWrite == 4 || retWrite == 3 || retWrite == 2) {
    lcd.clearLcd();
    print("Write error : ");
    print(retWrite);
    return 0;
  }
  return 1;
}

// Add add amount is in minutes
boolean addUnit(int amount) {
  int block = 2;
  unsigned int retRead = read_block(block, keya, readbackblock, 0);

  if (retRead == 3 || retRead == 4) {
    lcd.clearLcd();
    print("No Card add");
    return 0;
  }

  unsigned int readValue = readbackblock[1] + readbackblock[0] * 256;
  unsigned int writeValue = readValue + amount * 60;

  if (writeValue < 60000) { // Limit max of the seconds amount (we use a unsigned int)
    blockcontent[1] = writeValue;
    blockcontent[0] = writeValue / 256;
    unsigned int retWrite = write_block(block, keya, readbackblock, 0);
    if (retWrite == 4 || retWrite == 3 || retWrite == 2) {
      lcd.clearLcd();
      print("Write error : ");
      print(retWrite);
      return 0;
    }
  }
  return 1;
}

void startEngine() {
  digitalWrite(engineSwitch, LOW);
}

void stopEngine() {
  digitalWrite(engineSwitch, HIGH);
}
