int printName(int block) {
  unsigned int retRead = read_block(block, keya, readbackblock,0);
  lcd.setCursor(0, 1);
  if (retRead == 3 || retRead == 4) {
    lcd.print("No Card");
    return 0;
  }
  int i = 0;
  while ( (readbackblock[i] >= 0x41 &&  readbackblock[i] <= 0x7A) || readbackblock[i] == 32) {
    lcd.print((char)readbackblock[i]);
    i++;
  }
  return (1);
}

void printCredit(float freqOn) {
  unsigned int minutes = HowManyUnit() / 60;
  int i = 0;
  
  lcd.clear();
  lcd.print("mn:");
  lcd.print(minutes); 
  lcd.setCursor(0, 1);
  lcd.print("N:");
  getName(4);
  while (i < 16 && readbackblock[i] != 0 &&(  (readbackblock[i] >= 0x41 &&  readbackblock[i] <= 0x7A) || readbackblock[i] == 0x20)) {
    lcd.print(char(readbackblock[i]));
    i++;
  }
  lcd.setCursor(0, 2);
  lcd.print("C:");
  i=0;
  getName(6); // RO
  while (i <= 16 && readbackblock[i] != 0) {
    lcd.print(char(readbackblock[i]));
    i++;
  }
  lcd.setCursor(0, 3);
  Process p;        // Create a process and call it "p"
  p.begin("getIp");
  p.run();
  while (p.available() > 1) {
    char c = p.read();
    lcd.print(c);
  }
  delay(2000);
}


