//Calibra_LDR

#define PIN_N A0 //LDR1 leste 
#define PIN_O A1 //LDR2 Norte
#define PIN_L A2 //LDR3 Sul
#define PIN_S A3 //LDR4 oeste
#define SERIAL_RATE 115200

long tempo = 0;

void setup() {
  Serial.begin( SERIAL_RATE );
  // Configurando os LDRS
  pinMode( PIN_L, INPUT_PULLUP );    
  pinMode( PIN_N, INPUT_PULLUP );
  pinMode( PIN_S, INPUT_PULLUP );
  pinMode( PIN_O, INPUT_PULLUP );

  Serial.println(F("---------------"));
  Serial.println("offset dos LDR's");
  Serial.println( " N     S     L     O ");

}

void loop() {
  if ( abs( millis()-tempo) > 1000 ) { 
    float L=0, N=0, S=0, O=0;
    for ( int i=1; i<10; i++ ) {
      L += analogRead( PIN_L );
      N += analogRead( PIN_N );
      S += analogRead( PIN_S );
      O += analogRead( PIN_O );
    }
    L = L/10;
    N = N/10;
    S = S/10;
    O = O/10;

    float offset_N = (( S - N )/2)/N;
    float offset_S = (( N - S )/2)/S;
    float offset_L = (( O - L )/2)/L;
    float offset_O = (( L - O )/2)/O;
    
    Serial.print( offset_N );
    Serial.print("  ");
    Serial.print( offset_S );
    Serial.print("  ");
    Serial.print( offset_L );
    Serial.print("  ");
    Serial.println( offset_O );

    tempo = millis();
  }
  delay(100);
}
