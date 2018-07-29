void splash(LiquidCrystal_I2C& lcd, float ver, long splashTime) {

  uint8_t g1[8]  = {30,30,30,30,0,0,0,0};
  uint8_t g2[8]  = {3,7,15,31,30,28,28,28};
  uint8_t g3[8]  = {28,28,30,30,15,15,7,3};
  uint8_t g4[8]  = {30,30,30,30,30,30,30,30};
  uint8_t a1[8]  = {16,16,16,16,24,24,24,28};
  uint8_t a2[8]  = {1,1,1,1,3,3,3,7};
  uint8_t a3[8]  = {7,7,15,15,14,30,30,30};
  uint8_t a4[8]  = {28,28,30,30,14,15,15,15};

  lcd.createChar(0, g1);
  lcd.createChar(1, g2);
  lcd.createChar(2, g3);
  lcd.createChar(3, g4);
  lcd.createChar(4, a1);
  lcd.createChar(5, a2);
  lcd.createChar(6, a3);
  lcd.createChar(7, a4);
  
  lcd.setCursor(5,1);
  lcd.write((uint8_t)1); 
  lcd.setCursor(6,1);
  lcd.write((uint8_t)0); 
  lcd.setCursor(7,1);
  lcd.write((uint8_t)5); 
  lcd.setCursor(8,1);
  lcd.write((uint8_t)4); 
  lcd.setCursor(9,1);
  lcd.write((uint8_t)1); 
  lcd.setCursor(10,1);
  lcd.write((uint8_t)0); 
  lcd.setCursor(11,1);
  lcd.write((uint8_t)1); 
  lcd.setCursor(12,1);
  lcd.write((uint8_t)0); 
  lcd.setCursor(13,1);
  lcd.write((uint8_t)3); 
  lcd.setCursor(14,1);
  lcd.write((uint8_t)5); 
  lcd.setCursor(15,1);
  lcd.write((uint8_t)4); 

  lcd.setCursor(5,2);
  lcd.write((uint8_t)2); 
  lcd.setCursor(6,2);
  lcd.write((uint8_t)3); 
  lcd.setCursor(7,2);
  lcd.write((uint8_t)6); 
  lcd.setCursor(8,2);
  lcd.write((uint8_t)7); 
  lcd.setCursor(9,2);
  lcd.write((uint8_t)2); 
  lcd.setCursor(10,2);
  lcd.write((uint8_t)3); 
  lcd.setCursor(11,2);
  lcd.write((uint8_t)2); 
  lcd.setCursor(12,2);
  lcd.write((uint8_t)3); 
  lcd.setCursor(13,2);
  lcd.write((uint8_t)3); 
  lcd.setCursor(14,2);
  lcd.write((uint8_t)6); 
  lcd.setCursor(15,2);
  lcd.write((uint8_t)7); 

  lcd.setCursor(8,3);
  lcd.print("v");
  lcd.print(ver);

  delay(splashTime);
}

void progress(LiquidCrystal_I2C& lcd, int col, int row, int width, float frac) {

  uint8_t lend[8]  = {0,0,0,31,16,16,31};
  uint8_t rend[8]  = {0,0,0,31,1,1,31};
  uint8_t empty[8]  = {0,0,0,31,0,0,31};
  uint8_t full[8]  = {0,0,0,31,31,31,31};

  lcd.createChar(4, lend);
  lcd.createChar(5, rend);
  lcd.createChar(6, empty);
  lcd.createChar(7, full);

  int filled = frac * width;

  lcd.setCursor(col,row);
  
  if (filled < 1) {
    lcd.write((uint8_t)4); 
  } else {
    lcd.write((uint8_t)7);
  }
    
  for (int ind = 2; ind < width; ind++) {
    if (ind <= filled) {
      lcd.write((uint8_t)7);
    } else {
      lcd.write((uint8_t)6);
    }
  }
  
  if (filled == width) {
    lcd.write((uint8_t)7);
  } else {
    lcd.write((uint8_t)5);
  }
  
}

// rotate is called anytime the rotary inputs change state.
void rotate() {
  unsigned char result = rotary.process();
  if (result == DIR_CW) {
    sub++;
  } else if (result == DIR_CCW) {
    add++;
  }
}

// button is called anytime the push button changes state.
void button() {
  
  if (setting < 3) {
    setting++;
  } else {
    setting = 0;
  }
}

void EEPROM_writeDouble(int ee, const double& value)
{
   const byte* p = (const byte*)(const void*)&value;
   int i;
   for (i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
}

double EEPROM_readDouble(int ee)
{
  double value;
  
  byte* p = (byte*)(void*)&value;
  int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);   

  return value;
}


