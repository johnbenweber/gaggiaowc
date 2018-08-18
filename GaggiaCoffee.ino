#include <max6675.h>
#include <Rotary.h>
#include <EEPROM.h>
#include <Event.h>
#include <Timer.h>

// lcd hardware pins
const int lcdVCC = 22;
const int lcdGND = 23;
// thermocouple hardware pins
const int thermoVCC = 41;
const int thermoGND = 43;
const int thermoDO = 45;
const int thermoCS = 47;
const int thermoCLK = 49;
// rotary encoder hardware pins
const int rotVCC = 31;
const int rotGND = 33;
const int rotSW = 18;
const int rotDT = 2;
const int rotCLK = 3;
// steam switch hardware pins
const int steamVCC = 40;
const int steamSW = 42;
// SSR hardware pins
const int ssrVCC = 30;
const int ssrGND = 32;

// firmware version
const float ver = 0.12;

// timing variables
Timer poll;
const long splashTime = 3000;
const long tempInt = 1000;
const long serialInt = 500;
long tcLastPoll = 0;

// rotary encoder variables
volatile byte add = 0;
volatile byte sub = 0;
int setting = 0;
const int singleClick = 1;
const int doubleClick = 2;
const int shortHold = 3;
const int longHold = 4;

// EEPROM addresses
const int brewSP = 1;
const int steamSP = 8;

// pid variables
double alarm = 150.0;
double proValue, setpoint, heaterOut;
double Kp = 300, Ki = 10, Kd = 2250;
int pidWindowSize = 2000;
unsigned long pidWindowStart;

// steam/brew state variable
int brew = 0;
String arrows = "";

// set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystalDisplay lcd(0x27,20,4);

// set up thermocouple
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// setup rotary encoder
Rotary rotary = Rotary(rotDT, rotCLK);

// specify PID links and initial tuning parameters
Controller boiler(&thermocouple, &proValue, &heaterOut, &setpoint, &alarm, Kp, Ki, Kd);

void setup() {
  
  Serial.begin(9600);
  
  // configure LCD pins 
  pinMode(lcdVCC, OUTPUT); digitalWrite(lcdVCC, HIGH);
  pinMode(lcdGND, OUTPUT); digitalWrite(lcdGND, LOW);
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
  // attach rotary encoder switch to interrupts
  attachInterrupt(0, rotate, CHANGE);
  attachInterrupt(1, rotate, CHANGE);

  // initialize the lcd
  lcd.init();
  
  // print startup message to the LCD.
  lcd.backlight();
  lcd.splash(ver, splashTime);
  
  // start up PID
  boiler.SetOutputLimits(0, pidWindowSize);
  boiler.SetMode(AUTOMATIC);

  // populate LCD with static portion of display
  lcd.populate(EEPROM_readDouble(brewSP), EEPROM_readDouble(steamSP));
  
  // set up polling tasks
  poll.every(tempInt, boiler.MeasureTemp);
  poll.every(serialInt, SerialReceive);
  poll.every(serialInt, SerialSend);
  
  // initialize PID window
  pidWindowStart = millis();
}

void loop() {

  // boiler heater control
  boiler.ComputePID();
  boiler.ExecutePID(ssrVCC, &windowStart, windowSize);
 
  // boiler heater drive indicator
  lcd.printProgress(1,3,19,heaterOut/pidWindowSize);

  // push button mode selection
  int buttonEvent = checkButton();
  if (buttonEvent == shortHold) { setting++; }
  if (buttonEvent == longHold) { boilerPID.TogglePID(); }

  // display PID mode indicator
  lcd.setCursor(0,3);
  mode = boiler.GetMode();
  if (mode == AUTOMATIC) {
    lcd.print("A");
  } else if (mode == MANUAL) {
    lcd.print("M");
  }
  
  // handle changes to respective setpoints
  if (setting == 1) {
    lcd.printSetpoint(setting,EEPROM_readDouble(brewSP));
    EEPROM_writeDouble(brewSP, EEPROM_readDouble(brewSP) + (add - sub)*0.5);
  } else if (setting == 2) {
    lcd.printSetpoint(setting,EEPROM_readDouble(steamSP));
    EEPROM_writeDouble(steamSP, EEPROM_readDouble(steamSP) + (add - sub)*0.5);
  } else if (setting == 3) {
    lcd.printSetpoint(setting, 0.0);
  }

  add = 0;
  sub = 0;
  
  // handle mode from steam switch
  brew = digitalRead(steamSW);
  lcd.printProValue(brew, proValue)
  if (brew) {
    setpoint = EEPROM_readDouble(brewSP);
    arrows = "<<";
  } else {
    setpoint = EEPROM_readDouble(steamSP);
    arrows = ">>";
  }
  
  lcd.setCursor(9,1);
  lcd.print(arrows);

  } 
}
