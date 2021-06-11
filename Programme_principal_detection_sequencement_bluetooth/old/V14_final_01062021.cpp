#include <Arduino.h>
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;
int which_BT_command;
unsigned int valueBT_byte_1;
unsigned int valueBT_byte_2;
unsigned int result_valueBT;
unsigned long delayBT;
boolean modif_injection = false; // condition de modification par Bluetooth
boolean modif_AA = false;
unsigned int value_modif_injection;
unsigned int value_modif_AA;
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
int l;
int c;

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



// VARIABLE RECUPERATION DE PARAMETRES USUELS


//TABLEAU DE VALEURS USUELLES
//Type Nom [Nombre de lignes ] [Nombre de données par lignes ]={donnée X, donnée Y, etc };
// Tableau[valeur_cycle_en_degré][rpm]


  

int TableauInjectionUtile[11][13] = {
  //{50,   100,  150,  500,  1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000}, RPM
    {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},    //0% angle papillon                                  //0% papillon
    {546,  546,  546,  546,  546,  546,  546,  546,  546,  546,  546,  546,  546},  //10%
    {1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091, 1091}, //20%
    {1637, 1637, 1637, 1637, 1637, 1637, 1637, 1637, 1637, 1637, 1637, 1637, 1637}, //30%
    {2182, 2182, 2182, 2182, 2182, 2182, 2182, 2182, 2182, 2182, 2182, 2182, 2182}, //40%
    {2728, 2728, 2728, 2728, 2728, 2728, 2728, 2728, 2728, 2728, 2728, 2728, 2728}, //50%
    {3273, 3273, 3273, 3273, 3273, 3273, 3273, 3273, 3273, 3273, 3273, 3273, 3273}, //60%
    {3819, 3819, 3819, 3819, 3819, 3819, 3819, 3819, 3819, 3819, 3819, 3819, 3819}, //70%
    {4364, 4364, 4364, 4364, 4364, 4364, 4364, 4364, 4364, 4364, 4364, 4364, 4364}, //80%
    {4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910, 4910}, //90%
    {5455, 5455, 5455, 5455, 5455, 5455, 5455, 5455, 5455, 5455, 5455, 5455, 5455} //100%
  };                                                                               

int TableauAA[11][13] = {
  //{50,    100,  150,  500,  1000, 1500, 2000, 2500, 3000, 3500,  4000, 4500, 5000}, RPM
    {16667, 9148, 7218, 2562, 1516, 1196, 1062, 1005, 991,  1006,  1041, 1095, 1167},  //0% angle papillon
    {16667, 9333, 6556, 2667, 1833, 1556, 1417, 1333, 1278, 1238,  1208, 1185, 1167}, //10%
    {16667, 9333, 6549, 2573, 1714, 1423, 1289, 1207, 1150, 1108,  1074, 1043, 1016}, //20%
    {16667, 9333, 6542, 2483, 1603, 1302, 1173, 1092, 1035, 992,   955,  918,  885}, //30%
    {16667, 9333, 6536, 2396, 1499, 1192, 1068, 988,  932,  888,   849,  808,  771}, //40%
    {16667, 9333, 6529, 2312, 1401, 1090, 971,  894,  838,  794,   755,  711,  671}, //50%
    {16667, 9333, 6523, 2232, 1310, 998,  884,  809,  755,  711,   671,  625,  585}, //60%
    {16667, 9333, 6516, 2153, 1225, 913,  804,  733,  679,  636,   596,  550,  509}, //70%
    {16667, 9333, 6510, 2078, 1145, 835,  732,  663,  611,  570,   530,  484,  444}, //80%
    {16667, 9333, 6503, 2005, 1071, 764,  666,  600,  550,  510,   471,  426,  386}, //90%
    {16667, 9333, 6497, 1935, 1001, 699,  606,  543,  495,  456,   419,  375,  337} //100%
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
      digitalWrite(led1, LOW);
    }
    if( (micros() - chrono) >= (PMH - T_avance_bobine) ) {
      digitalWrite(led1, HIGH);
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
  pinMode(A1, INPUT);
  pinMode(crankshaft_each_rising, INPUT);
  attachInterrupt(crankshaft_each_rising, incrementation, RISING);  // A chaque front montant on exécute le programme incrementation()
}


//=================================================================================
// ------------------------------SOUS-PROGRAMMES-----------------------------------
//=================================================================================



///// SOUS PROGRAMME de condition de valeur du tableau en fonction de l'angle papillon // valeur ligne
int calcul_l(int angle_pap_local) {
   //CONDTION DE VALEURS USUELLES EN FONCTION DE L'ANGLE PAPILLON
  if ( (angle_pap_local >= 0) && (angle_pap_local < 167) ) {
    l = 0;
  }
  else if ( (angle_pap_local >= 167) && (angle_pap_local < 410) ) {
    l = 1;
  }
  else if ( (angle_pap_local >= 410) && (angle_pap_local < 819) ) {
    l = 2;
  }
  else if ( (angle_pap_local >= 819) && (angle_pap_local < 1229) ) {
    l = 3;
  }
  else if ( (angle_pap_local >= 1229) && (angle_pap_local < 1638) ) {
    l = 4;
  }
  else if ( (angle_pap_local >= 1638) && (angle_pap_local < 2048) ) {
    l = 5;
  }
  else if ( (angle_pap_local >= 2048) && (angle_pap_local < 2457) ) {
    l = 6;
  }
  else if ( (angle_pap_local >= 2457) && (angle_pap_local < 2867) ) {
    l = 7;
  }
  else if ( (angle_pap_local >= 2867) && (angle_pap_local < 3276) ) {
    l = 8;
  }
  else if ( (angle_pap_local >= 3276) && (angle_pap_local < 3686) ) {
    l = 9;
  }
  else if (angle_pap_local >= 3686) {
    l = 10;
  }
  return l;
}

///// SOUS PROGRAMME de condition de valeur du tableau en fonction des RPM // valeur colonne
int calcul_c(int rpm_local) {
  if ( (rpm_local >= 0) && (rpm_local < 50) ) {
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

  angle_papillon = analogRead(A1);
  

  l = calcul_l(angle_papillon);
  c = calcul_c(rpm);

  if (modif_injection == false) {
    T_injection_utile = TableauInjectionUtile[l][c];
  }
  else {
    T_injection_utile = value_modif_injection;
  }

  if (modif_AA == false) {
    T_avance_bobine = TableauAA[l][c];
  }
  else {
    T_avance_bobine = value_modif_AA;
  }

//=========================================================================================================================

  // Envoie de paramètres vers l'application Android par Bluetooth ==========================================================

  if ( (millis() - delayBT) >= 500) {
      delayBT = millis();

      SerialBT.print(rpm);
      SerialBT.print("|");
      SerialBT.print(temp); //température à compléter
      SerialBT.print("|");
      SerialBT.print(angle_papillon); 
      SerialBT.print("|");
      SerialBT.print(T_injection_utile);
      SerialBT.print("|");
      SerialBT.print(T_avance_bobine);
      SerialBT.print("|");
    }
  
  
  /*Serial.print("rpm  ");
  Serial.println(rpm);
  Serial.print("Cycle :  ");*/

  //Serial.println(cycle);
  
  
//========= Réception de valeurs par BT ==============//
  if (SerialBT.available()) {
    which_BT_command = SerialBT.read();
    //////////////////////////////////////////////////////////////////////////////
    ////////////////Condition de commande par rapport au 1er octet////////////////
    //////////////////////////////////////////////////////////////////////////////
    if (which_BT_command == 1 || which_BT_command == 2) {
      valueBT_byte_1 = SerialBT.read();
      valueBT_byte_2 = SerialBT.read();
      result_valueBT = valueBT_byte_1 + valueBT_byte_2*256;
    }

    //Modification d'injection
    if (which_BT_command == 1) {
      Serial.print("Temps d'injection = ");
      Serial.println(result_valueBT);

      modif_injection = true;
      value_modif_injection = result_valueBT;
    }

    //Modification d'avance à l'allumage
    else if (which_BT_command == 2) {
      Serial.print("Temps d'avance à l'allumage = ");
      Serial.println(result_valueBT);

      modif_AA = true;
      value_modif_AA = result_valueBT;
    }

    //Réinitialisation des paramètres
    else if (which_BT_command == 3) {
      Serial.println("Paramètres réinitialisés :");
      modif_injection = false;
      modif_AA = false;
    }
    
    //Déconnexion de la liaison BT
    /*else if (which_BT_command == 4) {
      SerialBT.disconnect();
      Serial.println("Déconnexion");
    }*/
  }
}