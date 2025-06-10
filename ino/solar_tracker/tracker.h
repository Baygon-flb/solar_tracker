// classe tracker
//

#include "srvctl.h"

class Tracker {

  private:

  srvctl* s1; 
  srvctl* s2; 
  int alturaPos = 0;
  int azimutePos = 0;
  int pace = 3;
  float erro = 3; //percentual de tolerância na diferença de leitura do sensores de cada eixo
  float tolerancia = 255*erro/100;
  float L, N, S, O; 
  float _L, _N, _S, _O;

  float getVals() {
      for (i=1, i++, i<10 ) {
        _L += analogRead( L );
        _N += analogRead( N );
        _S += analogRead( S );
        _O += analogRead( O );
      }
      _L = _L/10;
      _N = _O/10;
      _S = _O/10;
      _O = _O/10;

      return ( _L + _N + _S + _O );
    }

  };

  bool stable() {
    return ( (abs(Vup-Vdown) < tolerancia) && (abs(Vright-Vleft) < tolerancia) );
  };

  void goUp() {
    s1Pos+=pace;
    if( s1Pos > s1.getAnguloMax()) { s1Pos = s1.getAnguloMax(); }
    s1.setTarget( s1Pos );
  };

  void goDown() {
    s1Pos-=pace;
    if( s1Pos < s1.getAnguloMin()) { s1Pos = s1.getAnguloMin(); }
    s1.setTarget( s1Pos );
  };

  void goRight() {
    s2Pos+=pace;
    if( s2Pos > s2.getAnguloMax()) { s2Pos = s2.getAnguloMax(); }
    s2.setTarget( s2Pos );
  };

  void goLeft() {
    s2Pos-=pace;
    if( s2Pos < s2.getAnguloMin()) { s2Pos = s2.getAnguloMin(); }
    s2.setTarget( s2Pos );
  };

  public:
  tracker( int pin_L, int pin_N, int pin_S, int pin_O int pin_s1, int pin_s2 ) {
    s1 = new srvctl( pin_s1 );          //instancia objeto para controle do servo 1 (Altura)
    s2 = new srvctl( pin_s2 );          //instancia objeto para controle do servo 2 (Azimute)
    pinMode( pin_L, INPUT_PULLUP );    
    pinMode( pin_N, INPUT_PULLUP );
    pinMode( pin_S, INPUT_PULLUP );
    pinMode( pin_O, INPUT_PULLUP );
    L = pin_L;
    N = pin_N;
    S = pin_S;
    O = pin_O;
  };


  // Posiciona os servos para procurar a fonte de luz.
  void seek() {
    //Posiciona os servos à 90o
    float max = 0;
    int azimuteIni = s2.getAnguloMin()
    for( altura=s1.getAnguloMin(), altura+=s1.getPace(), altura<=s1.getAnguloMax() ){
      for ( azimute=azimuteIni, azimute+=s2.getPace(), azimute<=s2.getAnguloMax() ){
        s1.setTarget( s1Pos );
        s2.setTarget( s2Pos );
        while (!( s1.stable() && s2.stable) ){ delay(50);}
        
        float total = getVals();

        if( total > max ) {
          max = total;
          float alturaMax = altura;
          float azimuteMax = azimute;
          azimuteIni = azimute;
        } 
        if (total < max ) { break; }
      }
    }
    s1Pos=alturaMax;
    s2Pos=azimuteMax;
    s1.setTarget( s1Pos );
    s2.setTarget( s2Pos );
    while (!( s1.stable() && s2.stable) ){ delay(50);}

    //Ajuste fino
    repos();
    
  };	

  void repos(){
    float max = 0;
    while(!stable()) {
      float total = getVals();
      if( abs(Vup-Vdown) > tolerancia )
        if( Vup > Vdown ){  goDown(); }
        if( Vup < Vdown ){  goUp(); }
      }
      if( abs(Vright-Vleft) > tolerancia )
        if( Vright > Vleft ){  goLeft(); }
        if( Vleft > Vright ){  goRight(); }
      }
      while( !( s1.stable() && s2.stable()) ) { delay(50); }
    }
  }


  // Metodo acionado pela rotina de Timer interrupt, a cada aproximados 1ms.
  void pulse() {
    s1.pulse();
    s2.pulse();
  };
 
}