#include <LCD16x2.h>
#include <Wire.h>
boolean page = false;
boolean didMyOneTimeAction = false;



LCD16x2 lcd;

int buttons;

void setup(){
  Wire.begin();
  Serial.begin(9600);

  lcd.lcdClear();
  
  lcd.lcdGoToXY(1,1);
  lcd.lcdWrite("VITESSE:");
  
  lcd.lcdGoToXY(1,2);
  lcd.lcdWrite("TEMPERATURE:");
  
}
void loop(){
  //Serial.print(page);
  buttons = lcd.readButtons();
  Serial.println(buttons);
  
  if(buttons == 14){  
    page = false;
  }
  if(buttons == 13){
    page = true;
  }


  
    
    
  if ( page == false ){
    if (didMyOneTimeAction == false) {
      didMyOneTimeAction = true;
      lcd.lcdClear();
      lcd.lcdGoToXY(1,1);
      lcd.lcdWrite("VITESSE:");
  
      lcd.lcdGoToXY(1,2);
      lcd.lcdWrite("TEMPERATURE:"); 
  
      lcd.lcdGoToXY(10,1);
      lcd.lcdWrite(5000);
    
      lcd.lcdGoToXY(13,2);
      lcd.lcdWrite(100);

      }
    }



  else if ( page == true ){
    if (didMyOneTimeAction == true) {
   
        didMyOneTimeAction = false;
        lcd.lcdClear();
        lcd.lcdGoToXY(1,1);
        lcd.lcdWrite("T INJECTION:");
  
        lcd.lcdGoToXY(1,2);
        lcd.lcdWrite("AALLUMAGE:"); 
  
        lcd.lcdGoToXY(13,1);
        lcd.lcdWrite(1234);

        lcd.lcdGoToXY(11,2);
        lcd.lcdWrite(12345);
   
        
      
     }

   }
     delay(100);
   }
    
  //lcd.lcdGoToXY(13,2);
  //if(buttons & 0x02){  
   // lcd.lcdWrite(0);}
  //else{
    //lcd.lcdWrite(100);}
 /* lcd.lcdGoToXY(7,2);
  if(buttons & 0x04)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(15,2);
  if(buttons & 0x08)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    */
    
