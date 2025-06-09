#include <Servo.h>

Servo s1;
Servo s2;

int S1angulo = 90, S2angulo = 90;
long tempo = 0;

char* command="";

void setup() {
  Serial.begin( 9600 );
  Serial.println( "inicializando os servos");
  s1.attach( 5 );
  s2.attach( 6 );
  // put your setup code here, to run once:
  s1.write( S1angulo );
  s2.write( S2angulo );

}

void loop() {
  char input;
  while( !Serial.available() ) { 
    if (abs( millis()-tempo) > 5000 ) {
      Serial.println("-----------------------");
      Serial.print( "LDR1: ");
      Serial.println( analogRead( A0 ));
      Serial.print( "LDR2: ");
      Serial.println( analogRead( A1 ));
      Serial.print( "LDR3: ");
      Serial.println( analogRead( A2 ));
      Serial.print( "LDR4: ");
      Serial.println( analogRead( A3 ));
      tempo = millis();
    }
  }

  input = Serial.read();

  switch( input ){
    case '1':
      S1angulo +=1;
      break;
    case '2':
      S1angulo -= 1;
      break;
    case '4':
      S2angulo +=1;
      break;
    case '5':
      S2angulo -= 1;
      break;
  }

  if( S1angulo > 180 ) { S1angulo = 180; }
  if( S1angulo < 0 ) { S1angulo = 0; }
  if( S2angulo > 180 ) { S2angulo = 180; }
  if( S2angulo < 0 ) { S2angulo = 0; }
  
  Serial.println( command );
  Serial.print( "S1Angulo: ");
  Serial.println( S1angulo );
  Serial.print( "S2Angulo: ");
  Serial.println( S2angulo );

  s1.write( S1angulo );
  s2.write( S2angulo );

  if (abs( millis()-tempo) > 5000 ) {
    Serial.print( "LDR1: ");
    Serial.println( analogRead( A0 ));
    Serial.print( "LDR2: ");
    Serial.println( analogRead( A1 ));
    Serial.print( "LDR3: ");
    Serial.println( analogRead( A2 ));
    Serial.print( "LDR4: ");
    Serial.println( analogRead( A3 ));
    tempo = millis();
  }

 
}
