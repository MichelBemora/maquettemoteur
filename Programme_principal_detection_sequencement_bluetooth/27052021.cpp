#include <Arduino.h>
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;
unsigned int valueBT;
unsigned long delayBT;
//=================================================================================
// ---------- DEFINITION DE VARIABLES ---------------------------------------------
//=================================================================================

//led de test
const int led = 25;
const int led1= 26;


const int crankshaft_each_rising = 27;


//VARIABLES DE CALCULS

unsigned int angle = 0;
unsigned long PulseStartTime;
unsigned long Periode;
unsigned long DiffPeriode;
unsigned int rpm;
const int PMH_pos = 168; // angle du PMH
unsigned int PMH;
unsigned long chrono;

//VARIABLES DE CONDITIONS

bool top = 0;
boolean etat = false;
boolean cycle = 0;

//ACTIONNEUR-BOBINE

const int T_charge_bobine = 3000; // valeur fixe à 3 ms
int T_avance_bobine;

//ACTIONNEUR-INJECTION

const int position_injection_pos = 138;
unsigned int position_injection_T; // 138° converti en temps en µs (voir le loop pour le calcul)
const int T_ouverture_injection = 1700;
int T_injection_utile;

int temp = 24; // test température
int angle_papillon;

boolean modif_parametres = false; // condition de modification par Bluetooth

// VARIABLE RECUPERATION DE PARAMETRES USUELS


//TABLEAU DE VALEURS USUELLES
//Type Nom [Nombre de lignes ] [Nombre de données par lignes ]={donnée X, donnée Y, etc };
// Tableau[valeur_cycle_en_degré][rpm]


  

int TableauInjectionUtile[6][13] = {
    //{50, 100, 150, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000}, RPM
    {1090, 1090, 1090, 1090, 1090, 1090, 1090, 1090, 1090, 1090, 1090, 1090, 1090}, //20% papillon
    {1640, 1640, 1640, 1640, 1640, 1640, 1640, 1640, 1640, 1640, 1640, 1640, 1640}, //30%
    {2180, 2180, 2180, 2180, 2180, 2180, 2180, 2180, 2180, 2180, 2180, 2180, 2180}, //40%
    {3270, 3270, 3270, 3270, 3270, 3270, 3270, 3270, 3270, 3270, 3270, 3270, 3270}, //60%
    {4310, 4310, 4310, 4310, 4310, 4310, 4310, 4310, 4310, 4310, 4310, 4310, 4310}, //80%
    {5450, 5450, 5450, 5450, 5450, 5450, 5450, 5450, 5450, 5450, 5450, 5450, 5450} //100%
  };

int TableauAA[6][13] = {
    //{50, 100, 150, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000}, RPM
    {16667, 9333, 6549, 2573, 1714, 1423, 1289, 1207, 1150, 1108, 1074, 1043, 1016}, //20% papillon
    {16667, 9333, 6542, 2483, 1603, 1302, 1173, 1092, 1035, 992, 955, 918, 885}, //30%
    {16667, 9333, 6536, 2396, 1499, 1192, 1068, 988, 932, 888, 849, 808, 771}, //40%
    {16667, 9333, 6523, 2232, 1310, 998, 884, 809, 755, 711, 671, 625, 585}, //60%
    {16667, 9333, 6510, 2078, 1145, 835, 732, 663, 611, 570, 530, 484, 444}, //80%
    {16667, 9333, 6497, 1935, 1001, 699, 606, 543, 495, 456, 419, 375, 337} //100%
  };


//=================================================================================
// ---------- PROGRAMME INTERRUPTION SUR LES FRONT MONTANT-------------------------
//=================================================================================

void incrementation() {
  
  DiffPeriode = Periode;
  Periode = micros() - PulseStartTime;
  PulseStartTime = micros();
  angle = angle + 6;



//=================================================================================
// ---------- DETECTION DES DEUX DENTS MANQUANTES DANS L'INTERRUPTION--------------
//=================================================================================

  if (top == 1) {
    top = 0;
    chrono = micros();
    etat = true;
    if (cycle == 0) {
      cycle = 1;
    }
    else if (cycle == 1) {
      cycle = 0;
    }
  }

  if ( (DiffPeriode*1.05) < Periode) {
    top = 1;
    angle = 0;
  }


//=================================================================================
// ------------------------ CONDITION DU CYCLE A EXECUTER -------------------------
//=================================================================================

  //===========CYCLE INJECTION===========//
  if ( (cycle == 0) && (etat = true)) {
    if( (micros() - chrono) >= position_injection_T) {
      //INJECTION ON
      digitalWrite(led, HIGH);
    }
    

    if ( (micros() - chrono) >= (position_injection_T + T_ouverture_injection + T_injection_utile) ) {
      //INJECTION OFF
      digitalWrite(led, LOW);
      etat = false;
    }
  }
  

  //===========ALLUMAGE===========//
 
    if( (micros() - chrono) >= (PMH - T_avance_bobine - T_charge_bobine) ) {
      //BOBINE CHARGE
      digitalWrite(led1, HIGH);
    }
    if( (micros() - chrono) >= (PMH - T_avance_bobine) ) {
      digitalWrite(led1, LOW);
      //BOBINE DECHARGE
  }
}



//=================================================================================
// ---------- VOID SETUP-----------------------------------------------------------
//=================================================================================

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32");
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(crankshaft_each_rising, INPUT);
  attachInterrupt(crankshaft_each_rising, incrementation, RISING);  // A chaque front montant on exécute le programme incrementation()
}


//=================================================================================
// ------------------------------SOUS-PROGRAMMES-----------------------------------
//=================================================================================



///// SOUS PROGRAMME de condition de valeur du tableau en fonction de l'angle papillon // valeur ligne
int calcul_l(int angle_pap_local) {
  int l;
   //CONDTION DE VALEURS USUELLES EN FONCTION DE L'ANGLE PAPILLON
  if (angle_pap_local < 20) {
    l = 0;
  }
  else if ( (angle_pap_local >= 20) && (angle_pap_local < 30) ) {
    l = 1;
  }
  else if ( (angle_pap_local >= 30) && (angle_pap_local < 40) ) {
    l = 2;
  }
  else if ( (angle_pap_local >= 40) && (angle_pap_local < 60) ) {
    l = 3;
  }
  else if ( (angle_pap_local >= 60) && (angle_pap_local < 80) ) {
    l = 4;
  }
  else if (angle_pap_local >= 80) {
    l = 5;
  }
  return l;
}

///// SOUS PROGRAMME de condition de valeur du tableau en fonction des RPM // valeur colonne
int calcul_c(int rpm_local) {
  int c;
  if (rpm_local < 50) {
    c = 0;
  }
  else if ( (rpm_local >= 50) && (rpm_local < 100) ) {
    c = 1;
  }
  else if ( (rpm_local >= 100) && (rpm_local < 150) ) {
    c = 2;
  }
  else if ( (rpm_local >= 150) && (rpm_local < 500) ) {
    c = 3;
  }
  else if ( (rpm_local >= 500) && (rpm_local < 1000) ) {
    c = 4;
  }
  else if ( (rpm_local >= 1000) && (rpm_local < 1500) ) {
    c = 5;
  }
  else if ( (rpm_local >= 1500) && (rpm_local < 2000) ) {
    c = 6;
  }
  else if ( (rpm_local >= 2000) && (rpm_local < 2500) ) {
    c = 7;
  }
  else if ( (rpm_local >= 2500) && (rpm_local < 3000) ) {
    c = 8;
  }
  else if ( (rpm_local >= 3000) && (rpm_local < 3500) ) {
    c = 9;
  }
  else if ( (rpm_local >= 3500) && (rpm_local < 4000) ) {
    c = 10;
  }
  else if ( (rpm_local >= 4000) && (rpm_local < 4500) ) {
    c = 11;
  }
  else if ( rpm_local >= 4500 ) {
    c = 12;
  }
  return c;
}

////========SOUS-PROGRAMME RECUPERATION INJECTION============/////////////////
int recup_injection(int angle_pap_local, int rpm_local) {
  int l;
  int c;
  l = calcul_l(angle_pap_local);
  c = calcul_c(rpm_local);
  return TableauInjectionUtile[l][c];
}

////========SOUS-PROGRAMME RECUPERATION AVANCE A L'ALLUMAGE============/////////////////
int recup_allumage(int angle_pap_local, int rpm_local) {
  int l;
  int c;
  l = calcul_l(angle_pap_local);
  c = calcul_c(rpm_local);
  return TableauAA[l][c];
}






//=================================================================================
// ---------- VOID LOOP-------------PROGRAMME PRINCIPAL----------------------------
//=================================================================================


void loop() {

//Calculs =================================================================================================================

  //Eviter les divisions par 0
  if (Periode == 0) {
    // 1 minute divisé par 60 dents fois le temps entre deux dents correspondant à un 1 tour complet
    rpm = 1;
  }
  else if (Periode > 0) {
    rpm = 1000000/Periode;
  }
  
  PMH = PMH_pos * 1000000 / (rpm * 6);
  position_injection_T = position_injection_pos * 1000000 / (rpm * 6);

  angle_papillon = analogRead(A0);

//=========================================================================================================================

  // Envoie de paramètres vers l'application Android par Bluetooth ==========================================================

  
  if ( (millis() - delayBT) >= 500) {
    delayBT = millis();

    SerialBT.print(rpm);
    SerialBT.print("|");
    SerialBT.print(temp); //température à compléter
    SerialBT.print("|");
    SerialBT.print(angle_papillon); //angle_papillon à compléter
    SerialBT.print("|");
    SerialBT.print(T_injection_utile);
    SerialBT.print("|");
    SerialBT.print(T_avance_bobine);
    SerialBT.print("|");
  }
  
  /*Serial.print("rpm  ");
  Serial.println(rpm);
  Serial.print("Cycle :  ");*/

  Serial.println(cycle);

  if (SerialBT.available()) {
    valueBT = SerialBT.read();
    Serial.println(valueBT);
    if (valueBT == 10) {
      modif_parametres = false;
    }
    if (valueBT >= 99) {
      modif_parametres = true;
    }
  }

  if (modif_parametres == false) {
    T_injection_utile = recup_injection(angle_papillon, rpm);
    T_avance_bobine = recup_allumage(angle_papillon, rpm);
  }
  else {
    if (valueBT >= 100 && valueBT <= 2000) {
      T_avance_bobine = valueBT;
      valueBT = 0;
    }
    if (valueBT >= 3000 && valueBT <= 45000) {
      T_injection_utile = valueBT / 3;
      valueBT = 0;
    }
  }
 
}