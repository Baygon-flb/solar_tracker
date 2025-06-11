// Registra configuracao de temperatura na ROM
#include <EEPROM.h>

class Regdata {

  int pos = 0;
    
  public:
  Regdata() { pos = 0; };

  void write( int h, int m, int r, int g, int b ) {
    char data[5]={ h,m,r,g,b };
    //verifica overflow de memória
    if( (2000-pos) < 5 ) { pos = 0; }
    //grava os dados na memória
    for( int i=0; i<5; i++){ 
      EEPROM.write( pos, data[i] );
      pos++;
    }      
    pos++;
  };

  void read( int numReg, int data[] ) {
    int p = numReg*6;
    for( int i=0; i<5; i++){ 
      data[i] = EEPROM.read( p );
      p++;
    }      
  };

  void reset() { pos = 0; };
  int free() { return (2000-pos); };
  int curAddr() { return (int(pos/6)); };

};
