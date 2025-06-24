// Registra dados coletados do sensor na Eprom
// Metodos:
// void write( int h, int m, int r, int g, int b) - Salva um registro de dados
// void read( int numReg, data[]) - retorna, por referência, a matriz data[] com o registro de dados apontado por numReg
// void reset() - reseta a posicao do ponteiro de registro de dados para a posição zero)
// void recover() - recupera o ultimo valor salvo para o ponteiro de registro de dados )
#include <EEPROM.h>

class Regdata {

  int pos = 0; //ponteiro de posição do slot atual de registro de dados
  int reserva = 100; //quantidade de bytes reservada para o registro de parâmetros de funcionamento 
  int lenRegister = 5; // largura do slot de registro de dados em quantidade de bytes
  int maxPos = reserva+(int((2048 - reserva)/lenRegister)); //Valor máximo do ponteiro de registro 

  //dec2bytes
  //converte um inteiro decimal com valor até 65535 em 2 bytes
  void dec2bytes( int val, uint8_t bytes[] ){
    bytes[0] = uint8_t( val/255 );
    bytes[1] = uint8_t( val-(bytes[0]*255) );
  }

  // salva o ponteiro de memoria
  void savePos(){
    //o maior valor que pos pode assumir é 2000. Requer 2 bytes portanto.
    uint8_t bytes[2];
    dec2bytes( pos, bytes );
    EEPROM.write(0, bytes[0] ); //salva o byte mais significativo
    EEPROM.write(1, bytes[1] ); //salva o byte menos significativo
  }  

  public:
  Regdata( int lenReg=5, int lenReserved=100 ) { 
    pos = 0; 
    lenRegister = lenReg;
    reserva = lenReserved;
  };

  void write( uint8_t data[] ) {
    //verifica se haverá overflow de memória
    if( ( maxPos-pos) < lenRegister ) { pos = 0; }
    //Grava os dados na memória 
    int i = 0;
    int posfinal = pos+lenRegister;
    for( pos; pos<(posfinal); pos++){ 
      EEPROM.write( pos+reserva, data[i] );
      i++;
    }      
    savePos();
  };

  void read( int numReg, int data[] ) {
    int p = (numReg*lenRegister)+reserva;
    int pfim = p+lenRegister;
    int i=0;
    for( p; p<pfim; p++){ 
      data[i] = EEPROM.read( p );
      i++;
    }      
  };

  void reset() { pos = 0; savePos(); };
  int free() { return (maxPos-pos); };
  int curAddr() { return (int(pos/lenRegister)); };

  //recupera a última posição do ponteiro de memória
  void recover(){
    int Lbyte, Hbyte;
    Hbyte = EEPROM.read( 1 );
    Lbyte = EEPROM.read( 2 );
    pos = int( (Hbyte*255)+Lbyte );
  }
};
