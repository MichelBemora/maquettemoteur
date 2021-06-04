/* Menu system for a 16x2 LCD display
    and rotary encoder
    includes sleep mode for LCD
*/
#include <LiquidCrystal_I2C.h>
#include <rotary.h>                 // rotary handler
 
LiquidCrystal_I2C lcd(0x27,20,4);
 
#define PINA A0
#define PINB A1
#define PUSHB A2
 
// Initialize the Rotary object
// Rotary(Encoder Pin 1, Encoder Pin 2, Button Pin) Attach center to ground
Rotary r = Rotary(PINA, PINB, PUSHB);        // there is no must for using interrupt pins !!
 
int columnsLCD = 16;
char* MenuLine[] = {" Injection", " Allumage", " Donnees", " Puissance", " Frein", " Option 6", " Option 7"};
int MenuItems = 7;
int CursorLine = 0;
 
int backlightPin = 10;   //PWM pin
int brightness = 255;
int fadeAmount = 5;
 
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;  //the value is a number of milliseconds
 
void setup ()
{
  digitalWrite (PINA, HIGH);     // enable pull-ups
  digitalWrite (PINB, HIGH);
  digitalWrite (PUSHB, HIGH);
 
  pinMode(backlightPin, OUTPUT);          //backlightPin as an output
  digitalWrite(backlightPin, HIGH);
 
  //Serial.begin(9600);
  lcd.init();                      // initialize the lcd &&cs
  lcd.init();
  lcd.backlight();
  lcd.print ("Menu Master");
  lcd.setCursor(0, 1);
  lcd.print("Please Select");
  delay(2000);
  startMillis = millis();  //initial start time
  print_menu();
} //End of setup
 
void loop ()
{
  volatile unsigned char result = r.process();
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    LCDfadeOut();      //set LCD to sleep...
  } //End if currenMillis...
 
  if (result) {
    init_backlight();  //wake up LCD...
    result == DIR_CCW ? CursorLine = CursorLine - 1 : CursorLine = CursorLine + 1;
    if (CursorLine < 0) { CursorLine = MenuItems - 1; // roll over to last item } if (CursorLine > MenuItems - 1) {
      CursorLine = 0;                 // roll over to first item
    }
    print_menu();
  } //End if result
 
  if (r.buttonPressedReleased(25)) {
    init_backlight();     //wake up LCD...
    lcd.setCursor(0, 0);  //(col, row)
    lcd.print("ITEM");
    lcd.setCursor(0, 1);  //(col, row)
    selection();
    print_menu();
    
    
  } //endif buttonPressedReleased
} //End loop()
 
/************FUNCTIONS**************/
 
void print_menu()
{
  lcd.clear();
  lcd.setCursor(0, 0);     //(col, row)
  lcd.print("     Menu    ");
  lcd.setCursor(0, 1);    //2nd row
  lcd.print("<"); lcd.setCursor(columnsLCD - 1, 1); lcd.print(">");
  lcd.setCursor(1, 1);
  lcd.print(MenuLine[CursorLine]);
}
 
void selection()
{
  switch (CursorLine) {
    case 0:
      lcd.print("Injection    ");
      //set a flag or do something....
      break;
    case 1:
      lcd.print("Allumage    ");
      //set a flag or do something....
      break;
    case 2:
      lcd.print("Donnees    ");
      //set a flag or do something....
      break;
    case 3:
      lcd.print("Option 4    ");
      //set a flag or do something....
      break;
    case 4:
      lcd.print("Option 5    ");
      //set a flag or do something....
      break;
    case 5:
      lcd.print("Option 6    ");
      //set a flag or do something....
      break;
    case 6:
      lcd.setCursor(0, 1);
      lcd.print("Romain est passer");
      lcd.setCursor(0, 2);
      lcd.print("Par la - Theo 2021");
      //set a flag or do something....
      break;
    default:
      break;
  } //end switch
 
  delay(2000);
  CursorLine = 0;     // reset to start position
} //End selection
 
void LCDfadeOut()
{
  while (brightness > 0) {
    analogWrite(backlightPin, brightness);
    brightness -= fadeAmount;
    delay(20);
  } //End while
  digitalWrite(backlightPin, LOW);
  lcd.clear();
} //End LCDfadeOut
 
void init_backlight()
{
  digitalWrite(backlightPin, HIGH);
  startMillis = millis();  //initial start time
  brightness = 255;   //reset to initial brightness
} //End init_backlight
