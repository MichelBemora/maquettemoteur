int decimalPrecision = 2;
int ThermistorPin = A1;
int Ventilo = 13;
float voltageDividerR1 = 2200;
float BValue = 3470;
float R1 = 2200;
float T1 = 298.15;
float R2 ;
float T2 ;
float a ;
float b ;
float c ;
float d ;
float e = 2.718281828 ;
float tempp = 0; 
float tempSampleRead = 0;
float tempLastSample = 0;
float tempSampleSum = 0;
float tempSampleCount = 0;
float tempMean ;

void setup()
{
Serial.begin(9600);
} 

void loop() 
{ 
if(millis() >= tempLastSample + 1)
{
tempSampleRead = analogRead(ThermistorPin);
tempSampleSum = tempSampleSum+tempSampleRead;
tempSampleCount = tempSampleCount+1;
tempLastSample = millis();
}
 
if(tempSampleCount == 1000)
{
tempMean = tempSampleSum / tempSampleCount;
R2 = (voltageDividerR1*tempMean)/(1023-tempMean);
 
a = 1/T1;
b = log10(R1/R2);
c = b / log10(e);
d = c / BValue ;
T2 = 1 / (a- d);

tempp = T2 - 273.15,decimalPrecision;
Serial.print(tempp);
Serial.println(" °C"); 
tempSampleSum = 0;
tempSampleCount = 0;
}
if(tempp < 60.00){
  digitalWrite (13,LOW);
}
else if(tempp > 60.00){
  digitalWrite (13,HIGH);
}
}
