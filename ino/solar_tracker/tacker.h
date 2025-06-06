// classe tracker
//

#include "svrctl.h"

class Tracker {

  private:

  srvctl* s1; 
  srvctl* s2; 
  s1Pos = 0;
  s2Pos = 0;
  int up, down, left, right; 

  public:
  tracker( int pin_UP, int pin_DOWN, int pin_RIGHT, int pin_LEFT int pin_s1, int pin_s2 ) {
    s1 = new srvctl( pin_s1 );
    s2 = new srvctl( pin_s2 );
    pinMode( pin_UP, INPUT_PULLUP );
    pinMode( pin_DOWN, INPUT_PULLUP );
    pinMode( pin_RIGHT, INPUT_PULLUP );
    pinMode( pin_LEFT, INPUT_PULLUP );
  }

  void init() {
    
  }


}