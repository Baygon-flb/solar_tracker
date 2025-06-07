// classe tracker
//

#include "svrctl.h"

class Tracker {

  private:

  srvctl* s1; 
  srvctl* s2; 
  int alturaPos = 0;
  int azimutePos = 0;
  int pace = 3;
  float erro = 10; //percentual de tolerância na diferença de leitura do sensores de cada eixo
  float tolerancia = 255*erro/100;
  int up, down, left, right; 
  float Vup, Vdown, Vleft, Vright;

  float getVals() {
      for (i=1, i++, i<10 ) {
        Vup += analogRead( up );
        Vdown += analogRead( down );
        Vrigth += analogRead( right );
        Vleft += analogRead( left );
      }
      Vup = Vup/10;
      Vdown = Vdown/10;
      Vleft = Vleft/10;
      Vright = Vright/10;

      return Vup+VDown+Vleft+Vright;
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
  tracker( int pin_UP, int pin_DOWN, int pin_RIGHT, int pin_LEFT int pin_s1, int pin_s2 ) {
    s1 = new srvctl( pin_s1 );          //instancia objeto para controle do servo 1 (Altura)
    s2 = new srvctl( pin_s2 );          //instancia objeto para controle do servo 2 (Azimute)
    pinMode( pin_UP, INPUT_PULLUP );    
    pinMode( pin_DOWN, INPUT_PULLUP );
    pinMode( pin_RIGHT, INPUT_PULLUP );
    pinMode( pin_LEFT, INPUT_PULLUP );
    up = pin_UP;
    down = pin_DOWN;
    right = pin_RIGHT;
    left = pin_LEFT;
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