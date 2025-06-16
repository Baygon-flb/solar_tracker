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
    O = O/10;// put your main code here, to run repeatedly:
    
    Serial.println(F("---------------"));
    Serial.println("LDR's");
    Serial.println("LDR1 (N): "+String( int( N )));
    Serial.println("LDR2 (O): "+String( int( O )));
    Serial.println("LDR3 (L): "+String( int( L )));
    Serial.println("LDR4 (S): "+String( int( S )));    
    tempo = millis();
  }
  delay(200);
}
