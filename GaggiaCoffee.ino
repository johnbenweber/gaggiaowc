#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <max6675.h>
#include <PID_v1.h>
#include <Rotary.h>
#include <EEPROM.h>

// lcd hardware pins
int lcdVCC = 22;
int lcdGND = 23;
// thermocouple hardware pins
int thermoVCC = 41;
int thermoGND = 43;
int thermoDO = 45;
int thermoCS = 47;
int thermoCLK = 49;
// rotary encoder hardware pins
int rotVCC = 31;
int rotGND = 33;
int rotSW = 18;
int rotDT = 2;
int rotCLK = 3;
// steam switch hardware pins
int steamVCC = 40;
int steamSW = 42;
// SSR hardware pins
int ssrVCC = 30;
int ssrGND = 32;

// firmware version
float ver = 0.10;

// custom symbols
uint8_t pv[8] = {24,24,16,16,5,5,5,2};
uint8_t sp[8] = {24,16,8,24,3,3,2,2};

// set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27,20,4);

// set up thermocouple
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// timing variables
long splashTime = 3000;
long tcPollInt = 1000;
long tcLastPoll = 0;
int pidWindowSize = 5000;
unsigned long pidWindowStart;

// rotary encoder variables
volatile byte add = 0;
volatile byte sub = 0;
volatile byte setting = 0;

// setup rotary encoder
Rotary rotary = Rotary(rotDT, rotCLK);

// pid variables
int brewSP = 1;
int steamSP = 8;
double proValue, setpoint, heaterOut;
double Kp=10, Ki=50, Kd=30; //https://www.home-barista.com/espresso-machines/add-pid-to-gaggia-classic-worth-it-t27206-40.html

// specify PID links and initial tuning parameters
PID heaterPID(&proValue, &heaterOut, &setpoint, Kp, Ki, Kd, DIRECT);

// steam/brew state variable
int brew = 0;
String arrows = "<<";

void setup()
{
  unsigned long start = millis();
  
  // configure LCD pins 
  pinMode(lcdVCC, OUTPUT); digitalWrite(lcdVCC, HIGH);
  pinMode(lcdGND, OUTPUT); digitalWrite(lcdGND, LOW);
  
  // initialize the lcd
  lcd.init();
  
  // print startup message to the LCD.
  lcd.backlight();
  splash(lcd, ver, splashTime);
  
  // start up PID
  heaterPID.SetOutputLimits(0, pidWindowSize);
  heaterPID.SetMode(AUTOMATIC);
  
  Serial.begin(57600);
    
  // configure thermocouple pins 
  pinMode(thermoVCC, OUTPUT); digitalWrite(thermoVCC, HIGH);
  pinMode(thermoGND, OUTPUT); digitalWrite(thermoGND, LOW);

  // configure steam switch pins 
  pinMode(steamVCC, OUTPUT); digitalWrite(steamVCC, LOW);
  pinMode(steamSW, INPUT_PULLUP);

  // configure heater controlling SSR pins
  pinMode(ssrVCC, OUTPUT); digitalWrite(ssrVCC, HIGH);
  pinMode(ssrGND, OUTPUT); digitalWrite(ssrGND, LOW);
  
  // configure rotary encoder pins
  pinMode(rotVCC, OUTPUT); digitalWrite(rotVCC, HIGH);
  pinMode(rotGND, OUTPUT); digitalWrite(rotGND, LOW);
  pinMode(rotSW, INPUT_PULLUP);
  
  attachInterrupt(0, rotate, CHANGE);
  attachInterrupt(1, rotate, CHANGE);
  attachInterrupt(5, button, FALLING);

  lcd.createChar(0, pv);
  lcd.createChar(1, sp);
  lcd.clear();

  lcd.setCursor(1,0);
  lcd.print("Brew");
  lcd.setCursor(0,1);
  lcd.write((byte)0);
  lcd.setCursor(6,1);
  lcd.print("C");
  lcd.setCursor(0,2);
  lcd.write((byte)1);
  if (EEPROM_readDouble(brewSP) < 100.0) { lcd.print(" "); }
  lcd.print(EEPROM_readDouble(brewSP),1);
  lcd.print("C");

  lcd.setCursor(14,0);
  lcd.print("Steam");
  lcd.setCursor(13,1);
  lcd.write((byte)0);
  lcd.setCursor(19,1);
  lcd.print("C");
  lcd.setCursor(13,2);
  lcd.write((byte)1);
  if (EEPROM_readDouble(steamSP) < 100.0) { lcd.print(" "); }
  lcd.print(EEPROM_readDouble(steamSP),1);
  lcd.print("C");
  
  pidWindowStart = millis();

}

void loop()
{
  unsigned long current = millis();

  // take care of PID control
  heaterPID.Compute();

  if (millis() - pidWindowStart > pidWindowSize)
  { //time to shift the Relay Window
    pidWindowStart += pidWindowSize;
  }
  
  if (heaterOut > millis() - pidWindowStart) {
    digitalWrite(ssrVCC, HIGH);
  } else {
    digitalWrite(ssrVCC, LOW);
  }
  
  progress(lcd,0,3,20,heaterOut/pidWindowSize);
  
  // handle changes to respective setpoints
  if (setting == 1) {
    lcd.setCursor(13,2);
    lcd.write((byte)1);
    EEPROM_writeDouble(brewSP, EEPROM_readDouble(brewSP) + (add - sub)*0.5);
    lcd.setCursor(0,2);
    lcd.print(">");
    if (EEPROM_readDouble(brewSP) < 100.0) { lcd.print(" "); }
    lcd.print(EEPROM_readDouble(brewSP),1);
    lcd.print("<");
  } else if (setting == 2) {
    lcd.setCursor(0,2);
    lcd.write((byte)1);
    lcd.setCursor(6,2);
    lcd.print("C");
    EEPROM_writeDouble(steamSP, EEPROM_readDouble(steamSP) + (add - sub)*0.5);
    lcd.setCursor(13,2);
    lcd.print(">");
    if (EEPROM_readDouble(steamSP) < 100.0) { lcd.print(" "); }
    lcd.print(EEPROM_readDouble(steamSP),1);
    lcd.print("<");
  } else if (setting == 3) {
    lcd.setCursor(13,2);
    lcd.write((byte)1);
    lcd.setCursor(19,2);
    lcd.print("C");
    setting = 0;
  }

  add = 0;
  sub = 0;
  
  // handle mode from steam switch
  brew = digitalRead(steamSW);
  if (brew) {
    setpoint = EEPROM_readDouble(brewSP);
    arrows = "<<";
  } else if (not(brew)) {   
    setpoint = EEPROM_readDouble(steamSP);
    arrows = ">>";
  }
  
  lcd.setCursor(9,1);
  lcd.print(arrows);

  // poll thermocouple and display current value
  if (current - tcLastPoll > tcPollInt) {
    proValue = thermocouple.readCelsius();
        
    if (brew) { lcd.setCursor(1,1); }
    else { lcd.setCursor(14,1); }
    
    if (proValue < 100.0) { lcd.print(" "); }
    lcd.print(proValue,1);
    lcd.print("C");

    if (brew) { lcd.setCursor(14,1); }
    else { lcd.setCursor(1,1); }
    
    lcd.print(" -----");
    
    tcLastPoll = current;
  }  
  
}
