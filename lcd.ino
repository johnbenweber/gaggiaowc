#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

class LiquidCrystalDisplay{

  public:
    LiquidCrystalDisplay(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows)
    : LiquidCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows) {};

    void printSetpoint(int setting, double setpoint) {
      if (setting == 1) {
        setCursor(13,2);
        write((byte)1);
        setCursor(0,2);
      } else if (setting == 2) {
        setCursor(0,2);
        write((byte)1);
        setCursor(6,2);
        print("C");
        setCursor(13,2);
      }
  
      if (setting == 1 || setting == 2) {
        print(">");
        if (setpoint < 100.0) { print(" "); }
        print(setpoint,1);
        print("<");
      } else if (setting == 3) {
        setCursor(13,2);
        write((byte)1);
        setCursor(19,2);
        print("C");
        setting = 0;
      }
    }

    void LiquidCrystalDisplay::printProValue(int mode, double provalue) {
      if (mode) {
        setCursor(1,1);
      } else {
        setCursor(14,1);
      }
      
      if (proValue < 100.0) {print(" ");}
      print(proValue,1);
      print("C");
    
      if (mode) {
        setCursor(14,1);
      } else {
        setCursor(1,1);
      }
      
      print(" -----");
    
    }

    void LiquidCrystalDisplay::splash(float ver, long splashTime) {
    
      uint8_t g1[8]  = {30,30,30,30,0,0,0,0};
      uint8_t g2[8]  = {3,7,15,31,30,28,28,28};
      uint8_t g3[8]  = {28,28,30,30,15,15,7,3};
      uint8_t g4[8]  = {30,30,30,30,30,30,30,30};
      uint8_t a1[8]  = {16,16,16,16,24,24,24,28};
      uint8_t a2[8]  = {1,1,1,1,3,3,3,7};
      uint8_t a3[8]  = {7,7,15,15,14,30,30,30};
      uint8_t a4[8]  = {28,28,30,30,14,15,15,15};
    
      createChar(0, g1);
      createChar(1, g2);
      createChar(2, g3);
      createChar(3, g4);
      createChar(4, a1);
      createChar(5, a2);
      createChar(6, a3);
      createChar(7, a4);
      
      setCursor(5,1);
      write((uint8_t)1); 
      setCursor(6,1);
      write((uint8_t)0); 
      setCursor(7,1);
      write((uint8_t)5); 
      setCursor(8,1);
      write((uint8_t)4); 
      setCursor(9,1);
      write((uint8_t)1); 
      setCursor(10,1);
      write((uint8_t)0); 
      setCursor(11,1);
      write((uint8_t)1); 
      setCursor(12,1);
      write((uint8_t)0); 
      setCursor(13,1);
      write((uint8_t)3); 
      setCursor(14,1);
      write((uint8_t)5); 
      setCursor(15,1);
      write((uint8_t)4); 
    
      setCursor(5,2);
      write((uint8_t)2); 
      setCursor(6,2);
      write((uint8_t)3); 
      setCursor(7,2);
      write((uint8_t)6); 
      setCursor(8,2);
      write((uint8_t)7); 
      setCursor(9,2);
      write((uint8_t)2); 
      setCursor(10,2);
      write((uint8_t)3); 
      setCursor(11,2);
      write((uint8_t)2); 
      setCursor(12,2);
      write((uint8_t)3); 
      setCursor(13,2);
      write((uint8_t)3); 
      setCursor(14,2);
      write((uint8_t)6); 
      setCursor(15,2);
      write((uint8_t)7); 
    
      lcdPrint(lcd, 8, 3, "v");
      lcdPrint(lcd, 8, 3, ver);
    
      delay(splashTime);
    }
    
    void LiquidCrystalDisplay::populate(double brewSP, double steamSP) {
    
      // custom symbols
      uint8_t pv[8] = {24,24,16,16,5,5,5,2};
      uint8_t sp[8] = {24,16,8,24,3,3,2,2};
    
    
      createChar(0, pv);
      createChar(1, sp);
      clear();
    
      setCursor(1,0);
      print("Brew");
      setCursor(0,1);
      write((byte)0);
      setCursor(6,1);
      print("C");
      setCursor(0,2);
      write((byte)1);
      if (brewSP < 100.0) { print(" "); }
      print(brewSP,1);
      print("C");
    
      setCursor(14,0);
      print("Steam");
      setCursor(13,1);
      write((byte)0);
      setCursor(19,1);
      print("C");
      setCursor(13,2);
      write((byte)1);
      if (steamSP < 100.0) { print(" "); }
      print(steamSP,1);
      print("C");
    }
    
    void LiquidCrystalDisplay::printProgress(int col, int row, int width, float frac) {
    
      uint8_t lend[8]  = {0,0,0,31,16,16,31};
      uint8_t rend[8]  = {0,0,0,31,1,1,31};
      uint8_t empty[8]  = {0,0,0,31,0,0,31};
      uint8_t full[8]  = {0,0,0,31,31,31,31};
    
      createChar(4, lend);
      createChar(5, rend);
      createChar(6, empty);
      createChar(7, full);
    
      int filled = frac * width;
    
      setCursor(col,row);
      
      if (filled < 1) {
        write((uint8_t)4); 
      } else {
        write((uint8_t)7);
      }
        
      for (int ind = 2; ind < width; ind++) {
        if (ind <= filled) {
          write((uint8_t)7);
        } else {
          write((uint8_t)6);
        }
      }
      
      if (filled == width) {
        write((uint8_t)7);
      } else {
        write((uint8_t)5);
      }
      
    }
};
