// Registra dados coletados do sensor na Eprom
#include <EEPROM.h>

class Regdata {

  int pos = 0; //ponteiro de posição da memória

  // salva o ponteiro de memoria
  void savePos(){
    //o maior valor que pos pode assumir é 2000
    uint8_t Hbyte = uint8_t( pos/100 ); //isola centena e milhar
    uint8_t Lbyte = uint8_t( pos-(Hbyte*100) ); //isola dezena e unidade
    EEPROM.write(0, Lbyte ); //salva dezena e unidade
    EEPROM.write(1, Hbyte ); //salva centena e milhar
  }  

  public:
  Regdata() { pos = 0; };

  void write( int h, int m, int r, int g, int b ) {
    char data[5]={ h,m,r,g,b };
    //verifica overflow de memória
    if( (2000-pos) < 5 ) { pos = 0; }
    //Grava os dados na memória 
    //Offset de 2 posiçoes: 
    //Posicoes 0 e 1 reservadas para 
    //o valor do ponteiro de memória
    for( int i=0; i<5; i++){ 
      EEPROM.write( pos+2, data[i] );
      pos++;
    }      
    pos++;
    savePos();
  };

  void read( int numReg, int data[] ) {
    int p = numReg*5;
    for( int i=0; i<5; i++){ 
      data[i] = EEPROM.read( p+2 );
      p++;
    }      
  };

  void reset() { pos = 0; };
  int free() { return (2000-pos); };
  int curAddr() { return (int(pos/6)); };

  //recupera a última posição do ponteiro de memória
  void recover(){
    int Lbyte, Hbyte;
    Lbyte = EEPROM.read( 0 );
    Hbyte = EEPROM.read( 1 );
    pos = int( (Hbyte*100)+Lbyte );
  }
};
