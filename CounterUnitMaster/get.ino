
uint32_t  GetId(BridgeClient client) {
  uint8_t success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  uint8_t data[32];

  uint32_t cardid = client.parseInt();
  if (success) {
    uint32_t cardid = uid[0];
    cardid <<= 8;
    cardid |= uid[1];
    cardid <<= 8;
    cardid |= uid[2];
    cardid <<= 8;
    cardid |= uid[3];
    lcd.clear();
    lcd.print(cardid);
    Serial.println(cardid);
    return (cardid);
  } else {
    return (0);
  }
}

int getName(int blockNumber) {
  // Username  = 6;
  // Clubname  = 4;
 return (read_block(blockNumber, keya, readbackblock, 0));
}

void GetIp() {
  lcd.clear();
  Process p;        // Create a process and call it "p"
  p.begin("getIp");
  p.run();
  lcd.clear();
  while (p.available() > 1) {
    char c = p.read();
    lcd.print(c);
  }
  lcd.setCursor(0, 1);
  lcd.print("Insert a card !");
  delay(1000);
}


