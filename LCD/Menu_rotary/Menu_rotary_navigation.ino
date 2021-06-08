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
char* MenuLine[] = {" Vitesse", " Temp/Puiss", " Injection", " Allumage", " Mode prof", " Reset", " .."};
int MenuItems = 7;
int CursorLine = 0;
int check_CursorLine = 0;
 
int backlightPin = 10;   //PWM pin
int brightness = 255;
int fadeAmount = 5;
 
unsigned long startMillis;
unsigned long currentMillis;
unsigned long refreshMillis;
const unsigned long period = 60000;  //the value is a number of milliseconds
const unsigned int refresh_delay = 1000;

const int buttonPin = 12; 
int buttonState = 0; 

boolean main_menu;
bool menu_refresh;


unsigned int rpm;
unsigned int angle_papillon;

void setup ()
{
  pinMode(buttonPin, INPUT);
  digitalWrite (PINA, HIGH);     // enable pull-ups
  digitalWrite (PINB, HIGH);
  digitalWrite (PUSHB, HIGH);
 
  pinMode(backlightPin, OUTPUT);          //backlightPin as an output
  digitalWrite(backlightPin, HIGH);
 
  //Serial.begin(9600);
  lcd.init();                      // initialize the lcd &&cs
  lcd.init();
  lcd.backlight();
  lcd.print ("Maquette moteur");
  lcd.setCursor(0, 1);
  lcd.print("Appuyez pour valider");
  
  if (buttonState == LOW) {
    main_menu = true;
    menu_refresh = 0;
    rpm = 5000;
    angle_papillon = 25;
    startMillis = millis();  //initial start time
  }
} //End of setup
 
void loop ()
{
  buttonState = digitalRead(buttonPin);
  rpm = rpm + 6;
  
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

  
  
  if (buttonState == LOW) {
    init_backlight();     //wake up LCD...
    lcd.setCursor(0, 0);  //(col, row)
    lcd.print("ITEM");
    lcd.setCursor(0, 1);  //(col, row)

    check_CursorLine = CursorLine;
    
    if (main_menu == false) {
      main_menu = true;
      menu_refresh = 0;
      print_menu();
      delay(1000);
    }
    else if (main_menu == true) {
      main_menu = false;
      menu_refresh = 1;
      selection();
      delay(1000);
    }


  } //endif buttonPressedReleased

  if (menu_refresh == 1) {
    if (currentMillis - refreshMillis >= refresh_delay) {
      refreshMillis = millis();
      variable_selection();

      if (CursorLine != check_CursorLine) {
        print_menu();
        check_CursorLine = CursorLine;
        main_menu = true;
        menu_refresh = 0;
      }
      
    }
  }
  
  
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
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vitesse: ");
      lcd.print(rpm);
      lcd.setCursor(0, 1);
      lcd.print("Ang Papillon: ");
      lcd.print(angle_papillon);
      //set a flag or do something....
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temperature :    ");
      lcd.setCursor(0, 1);
      lcd.print("Puissance :    ");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Injection utile :    ");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Avance Allumage :    ");
      //set a flag or do something....
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mode prof active :    ");
      break;
    case 5:
      lcd.clear();
      lcd.setCursor(0, 2);
      lcd.print("Reset complete     ");
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
 
  //delay(2000);
  //CursorLine = 0;     // reset to start position
} //End selection


void variable_selection(){
  
  switch (CursorLine) {
    case 0:
      lcd.setCursor(9, 0);
      lcd.print(rpm);
  
      lcd.setCursor(14, 1);
      lcd.print(angle_papillon);
      break;
  }

  
}

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
