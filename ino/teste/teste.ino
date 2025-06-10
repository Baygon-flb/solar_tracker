#include <SPI.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Servo.h>

//Instanciando as classes
Adafruit_TCS34725 TCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_300MS, TCS34725_GAIN_1X);
Servo s1;
Servo s2;

int S1angulo = 90, S2angulo = 90;
long tempo = 0;
bool mudou = false;
float tolerancia = 8;

char* command="";

void setup() {
  Serial.begin( 115200 );
  Serial.println( "inicializando os_SErvos");
  s1.attach( 5 );
  s2.attach( 6 );
  // put your_SEtup code here, to run once:
  s1.write( S1angulo );
  s2.write( S2angulo );

}

void loop() {
  //char input;
  //while( !Serial.available() ) { 
  float L=0, N=0, S=0, O=0;
  for (int i=1;i<10;i++){  
    L += analogRead( A0 );
    N += analogRead( A1 );
    S += analogRead( A2 );
    O += analogRead( A3 );
  }
  L = L/10;
  N = N/10;
  S = S/10;
  O = O/10;

  if (abs( millis()-tempo) > 2000 || mudou ) {

    //Lendo TSC34725
    uint16_t r, g, b, c, TempCor, LUX;
    //O sensor lê os valores do Vermelho (R), Verde(G), Azul(B)
    TCS.getRawData(&r, &g, &b, &c);
    //Calcula a temperatura da cor 
    TempCor = TCS.calculateColorTemperature(r, g, b);
    //Calcula a luminosidade 
    LUX = TCS.calculateLux(r, g, b);

    Serial.println("-----------------------");
    Serial.print( "LDR1 (L): ");
    Serial.println( L );
    Serial.print( "LDR2 (N): ");
    Serial.println( N );
    Serial.print( "LDR3 (S): ");
    Serial.println( S );
    Serial.print( "LDR4 (O): ");
    Serial.println( O );
    Serial.println( "-------------------" );
    Serial.print( "S1Angulo: ");
    Serial.println( S1angulo );
    Serial.print( "S2Angulo: ");
    Serial.println( S2angulo );
    Serial.println( "-------------------" );
    Serial.print( "Vermelho: ");
    Serial.println( r );
    Serial.print( "Verde: ");
    Serial.println( g );
    Serial.print( "Azul: ");
    Serial.println( b );
    Serial.print( "Itensidade: ");
    Serial.println( LUX );
    Serial.print( "Temperatura: ");
    Serial.println( TempCor );
    tempo = millis();
    mudou = false;
  }
 
  int S1AnguloAnterior = S1angulo;
  int S2AnguloAnterior = S2angulo;

  float _NE = N + L;
  float _SE = S + L;
  float _NO = N + O;
  float _SO = S + O;

  if ( abs( _SE - _SO) > tolerancia || abs( _SE - _SO) > tolerancia ) {
    if (_SE > _SO && _NE > _NO ) { S2angulo--; }
    if (_SE < _SO && _NE < _NO ) { S2angulo++; }
  }
  if ( abs( _SE - _NE) > tolerancia || abs( _SO - _NO) > tolerancia ) {
    if (_SE > _NE && _SO > _NO ) { S1angulo--; }
    if (_SE < _NE && _SO < _NO ) { S1angulo++; }
  }

  if( S1angulo > 170 ) { S1angulo = 170; }
  if( S1angulo < 10 ) { S1angulo = 10; }
  if( S2angulo > 170 ) { S2angulo = 170; }
  if( S2angulo < 10 ) { S2angulo = 10; }

  // if ( !( S1angulo == S1AnguloAnterior && S1angulo == S1AnguloAnterior  )) { mudou = true; }

  s1.write( S1angulo );
  s2.write( S2angulo );

  delay( 18 );

}
