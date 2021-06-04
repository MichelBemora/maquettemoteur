#include <LCD16x2.h>
#include <Wire.h>
int page = 1;
boolean didMyOneTimeAction = false;

LCD16x2 lcd;
int buttons;

const int Broche_BP_HAUT = 13;
const int Broche_BP_BAS = 14;
unsigned int RPM;
int Temp;

int Injection = 500;
int Aallumage = 17000;

int Reglage = 0;
int Tempo = 3000;
unsigned int Tempo2 = 5000;

unsigned long Temps_Actuel;
unsigned long Temps_Precedent = 0;
unsigned long Temps_Precedent2 = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
 // Serial.print("Tps_Actuel :");  Serial.print(Temps_Actuel);
 // Serial.print(" Tps_Precedent :");   Serial.print(Temps_Precedent);
  
  Temps_Actuel = millis();
  buttons = lcd.readButtons();
  Tempo2 = 7000;
  if(buttons == 14){ 
    lcd.lcdClear(); 
    page = 1;
  }
  if(buttons == 13){
    lcd.lcdClear();
    page = 2;
  }
    if(buttons == 11){
    lcd.lcdClear();
    page = 3;
  }

if (page == 3){
    lcd.lcdGoToXY(1,1);
    lcd.lcdWrite("VITESSE:");
    lcd.lcdGoToXY(10,1);
    lcd.lcdWrite(RPM);

    lcd.lcdGoToXY(1,2);
    lcd.lcdWrite("TEMPERATURE:"); 
    lcd.lcdGoToXY(13,2);
    lcd.lcdWrite(Temp);
}

if (page == 1) {
  buttons = lcd.readButtons();
/////////////////////////////PAGE 1 //////////////////////////////////////////////////////
  //lcd.clear();
  lcd.lcdGoToXY(1,1);
  lcd.lcdWrite("INJECTION:");
  lcd.lcdGoToXY(11,1);
  lcd.lcdWrite(Injection);


  if ((buttons == 12) && (Temps_Actuel - Temps_Precedent >= Tempo)) {
    Temps_Precedent = Temps_Actuel;
    Temps_Precedent2 = Temps_Actuel;
    Reglage = 1;
    while (Reglage == 1) {
      buttons = lcd.readButtons();
       Serial.println(Tempo2);

      if ( buttons == 13) {
        Injection = Injection + 10;
        Tempo2 = Tempo2 + 300;
      }
      if (  buttons == 14) {
        Injection = Injection - 10;
        Tempo2 = Tempo2 + 300;
      }
       if ( buttons == 7) {
        Injection = Injection + 100;
        Tempo2 = Tempo2 + 2000;
      }
      if (  buttons == 11) {
        Injection = Injection - 100;
        Tempo2 = Tempo2 + 2000;
      }
  //    lcd.lcdClear();
      lcd.lcdGoToXY(1,1);
      lcd.lcdWrite("INJECTION:");
      lcd.lcdGoToXY(11,1);
      lcd.lcdWrite(Injection);
      lcd.lcdGoToXY(1,2);
      lcd.lcdWrite("REGLAGE");
      Temps_Actuel = millis();


      if (Temps_Actuel - Temps_Precedent2 >= Tempo2) {
        Reglage = 0;
        lcd.lcdClear();
        Tempo2 = 7000;
        break;
      }
    }
    Temps_Precedent = Temps_Actuel;
    Temps_Precedent2 = Temps_Actuel;
  }
}



if (page == 2) {
  buttons = lcd.readButtons();
//////////////////////////PAGE 2 //////////////////////////////////
  //lcd.clear();
  lcd.lcdGoToXY(1,1);
  lcd.lcdWrite("AALLUMAGE:");
  lcd.lcdGoToXY(11,1);
  lcd.lcdWrite(Aallumage);


  if ((buttons == 12) && (Temps_Actuel - Temps_Precedent >= Tempo)) {
    Temps_Precedent = Temps_Actuel;
    Temps_Precedent2 = Temps_Actuel;
    Reglage = 1;
    while (Reglage == 1) {
      buttons = lcd.readButtons();
      Serial.println(Tempo2);

      if ( buttons == 13) {
        Aallumage = Aallumage + 25;
        Tempo2 = Tempo2 + 300;
      }
      if (  buttons == 14) {
        Aallumage = Aallumage - 25;
        Tempo2 = Tempo2 + 300;
      }
       if ( buttons == 7) {
        Aallumage = Aallumage + 500;
        Tempo2 = Tempo2 + 2000;
      }
      if (  buttons == 11) {
        Aallumage = Aallumage - 500;
        Tempo2 = Tempo2 + 2000;
      }
  //    lcd.lcdClear();
      lcd.lcdGoToXY(1,1);
      lcd.lcdWrite("AALLUMAGE:");
      lcd.lcdGoToXY(11,1);
      lcd.lcdWrite(Aallumage);
      lcd.lcdGoToXY(1,2);
      lcd.lcdWrite("REGLAGE");
      Temps_Actuel = millis();


      if (Temps_Actuel - Temps_Precedent2 >= Tempo2) {
        Reglage = 0;
        lcd.lcdClear();
        Tempo2 = 7000;
        break;
      }
    }
    Temps_Precedent = Temps_Actuel;
    Temps_Precedent2 = Temps_Actuel;
  }




  
}
////////////////////////////////////////////////////////////////////////
}
