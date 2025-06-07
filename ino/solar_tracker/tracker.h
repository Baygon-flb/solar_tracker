// classe tracker
//

#include "svrctl.h"

class Tracker {

  private:

  srvctl* s1; 
  srvctl* s2; 
  s1Pos = 0;
  s2Pos = 0;
  ct1 = 0;
  ct2 = 0;
  pace = 5;
  erro = 10; //percentual de tolerância na diferença de leitura do sensores de cada eixo
  int up, down, left, right; 

  public:
  tracker( int pin_UP, int pin_DOWN, int pin_RIGHT, int pin_LEFT int pin_s1, int pin_s2 ) {
    s1 = new srvctl( pin_s1 );          //instancia objeto para controle do servo 1
    s2 = new srvctl( pin_s2 );          //instancia objeto para controle do servo 2
    pinMode( pin_UP, INPUT_PULLUP );    
    pinMode( pin_DOWN, INPUT_PULLUP );
    pinMode( pin_RIGHT, INPUT_PULLUP );
    pinMode( pin_LEFT, INPUT_PULLUP );
    up = pin_UP;
    down = pin_DOWN;
    right = pin_RIGHT;
    left = pin_LEFT;
  };

  void init();
  void goUp();
  void goDown();
  void goLeft();
  void goRight();
  bool Vstable();
  bool Hstable();
  bool stable();

  // Posiciona os servos para procurar a fonte de luz.
  void seek() {
    Posiciona os servos à 90o
    s1.setTarget( 90 );
    s2.setTarget( 90 );
    s1.start();
    s2.start();
    while (!( s1.stable() && s2.stable) ){ delay(50);}
    s1Pos = 90;
    s2Pos = 90;

    //Primeiro estabiliza o angulo de altura
    while( !Vstable() ){  
      float Vup = analogRead( up );
      float Vdown = analogRead( down );
      if (Vup > Vdown) { goDown(); }
      if (Vup < Vdown) { goUP(); }
    }

    void follow(){ while( !stable() ){ delay(1); } };

    //Depois estabiliza o angulo de azimute
    while( !Hstable() ){
      float Vrigth = analogRead( right );
      float Vleft = analogRead( left );
      if (Vleft > Vright) { goRight(); }
      if (Vleft < Vright) { goLeft(); }
    }
  };

  bool Vstable() {
    float Vup = 0;
    float vDown = 0;
    for ( i=1, i++, i<10 ){
      Vup += analogRead( up );
      Vdown += analogRead( down );
    }
    Vup = Vup/10;
    Vdown = Vdown/10;
    if abs( Vup - Vdown ) < (255*erro/100) { return true; }
    return false;
  };

  bool Hstable() {
    float Vleft = 0;
    float vright = 0;
    for ( i=1, i++, i<10 ){
      Vleft += analogRead( up );
      Vright += analogRead( down );
    }
    Vleft = Vleft/10;
    Vright = Vright/10;
    if abs( Vleft - Vright ) < (255*erro/100) { return true; }
    return false;
  };

  bool stable() {
    return (Vstable() && Hstable());
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

  // Metodo acionado pela rotina de Timer interrupt, a cada aproximados 1ms.
  void pulse() {
    s1.pulse();
    s2.pulse();
  };
 
}