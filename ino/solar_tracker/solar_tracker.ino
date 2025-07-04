// Solar_tracker
// Seguidor solar
// Este projeto implementa um seguidor solar com 2 eixos, que rastreia, posiciona e mantém um sensor apontado
// perpendicularmente ao sol (ou outra fonte de luz), de forma a garantir a incidência máxima da intensidade 
// luminosa sobre o sensor. Neste caso específico, o sensor utilizado é para medir a variação do espectro, 
// intensidade e a temperatura da luz emitida pela fonte, utilizando um módulo sensor RGB TCS34725. 
// O projeto conta ainda com um relógio RTC, que permite controlar o horário de coleta dos 
// parâmetros de medição. As medições são tomadas a cada 10 minutos, à partir do horário definido como
// alvorada, até o horário definido como crepúsculo, armazenando os dados coletados na EEPROM.
// Se conectada a serial, o projeto entra no modo de ajuste de parâmetros e descarga dos dados coletados
// 

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <RTClib.h>  //Adafruit RTClib
#include <avr/wdt.h>
#include "regdata.h"
#include "srvctrl.h"

#define PIN_S1 5
#define PIN_S2 6
#define PIN_N A0 //LDR1 leste 
#define PIN_O A1 //LDR2 Norte
#define PIN_L A2 //LDR3 Sul
#define PIN_S A3 //LDR4 oeste
#define SERIAL_RATE 115200
#define DEBUG 0

//Instanciando as classes
Adafruit_TCS34725 TCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_300MS, TCS34725_GAIN_1X);
RTC_DS1307 rtc;
Srvctrl s1( PIN_S1 ); //altura
Srvctrl s2( PIN_S2 ); //azimute

// variáveis de controle
float minuto = 0;
float hora = 0;
long tempo = 0;
int erro = 3;
float tolerancia = (2.55*erro);
float intervalo = 1;

//horarios de inicio e fim
float hora_inicio = 5;
float minuto_inicio=30;
float hora_fim = 22;
float minuto_fim=30;
float inicio = hora_inicio+(minuto_inicio/60);
float fim = hora_fim+(minuto_fim/60);

//controle dos servos
int azimuteMin = 15;
int azimuteMax = 175;
int alturaMin = 10;
int alturaMax = 175;
int S2angulo = int((azimuteMax-azimuteMin)/2);
int S1angulo = int((alturaMax-alturaMin)/2);

//Calibragem LDR's
float offset_N = 0.043;
float offset_S = -0.04;
float offset_L = -0.06;
float offset_O = 0.07;

//Configuracao do registro de dados
// O formato do registro de dados é  
// Para resoluçao de 8 bits por canal de cor: 
// [ hora - 1 byte ][ minuto - 1 byte][ Red - 1 byte][ green - 1 byte][ blue - 1 byte] = 5 bytes
// Capacidade de memória para cerca de 400 registros
// 
// Para resoluçao de 16 bits por canal de cor: 
// [ hora - 1 byte ][ minuto - 1 byte][ Red - 2 bytes][ green - 2 bytes][ blue - 1 bytes] = 8 bytes
// Capacidade de memória para cerca de 250 registros
//
int resolucao = 2; //Resolução em bytes do registro de dados do sensor
int lenReg = 2+(4*resolucao); //tamanho do registro de dados
int reserva = 10; //numero de bytes para serem reservados no inicio da memória
Regdata reg(lenReg, reserva);

void debug( String msg, bool nl = true ) {

  if( Serial && DEBUG ) {
    if (nl) { Serial.println( msg ); }
    else { Serial.print( msg ); }
  }
} 

// Rotina de interrupção do timer
ISR(TIMER2_OVF_vect) {//trata Overflow do Timer,
  //Executa a cada 1ms aproximadamente  
  s1.pulse();
  s2.pulse();
} //end Timer2 OVF

// Envia as dados das amostras para o terminal serial
void descarrega(){
  wdt_reset();
  Serial.println();
  Serial.println(F("Descarregando os dados da memoria..") );
  Serial.println();
  for( int p=0; p < reg.curAddr(); p++) {
    int data[ lenReg ];
    reg.read( p, data );
    if( resolucao == 2 ){
      data[2] = data[2]*255+data[3];
      data[3] = data[4]*255+data[5];
      data[4] = data[6]*255+data[7];
    }
    for( int i=0; i<5; i++) {
      Serial.print( data[i] );
      Serial.print(";");
    }
    Serial.println();
  }
  reg.reset();
}

void config( ) {

  DateTime now = rtc.now();
  Serial.println(F("---------------"));
  Serial.println(F("DEBUG"));
  Serial.println("Hora atual: "+String( now.hour())+":"+String(now.minute()));
  Serial.println("var minuto: "+String( int( minuto )));
  Serial.println("Memoria Eprom livre: "+String( reg.free()));
  Serial.println("Slot de memória atual: "+String( reg.curAddr()));
  Serial.println("LDR's");
  Serial.println("LDR1 (N): "+String( int( analogRead( PIN_N )+offset_N)));
  Serial.println("LDR2 (O): "+String( int( analogRead( PIN_O )+offset_O)));
  Serial.println("LDR3 (L): "+String( int( analogRead( PIN_L )+offset_L)));
  Serial.println("LDR4 (S): "+String( int( analogRead( PIN_S )+offset_S)));
  Serial.println("Servos:");
  Serial.println("Azimute: "+String( int(s2.getTarget())));
  Serial.println("Altura: "+String( int(s1.getTarget())));
  Serial.println();
}

void setup() {
  // coloca sensor TCS34725 em hibernação
  //TCS.disable();

  reg.recover(); // recupera a posicao de memoria anterior

  //habilita o whatchdog para 5s
  wdt_enable(WDTO_4S);

  Serial.begin( SERIAL_RATE );
  Serial.println(F("SOLAR TRACKER"));
  Serial.println(F("-------------"));
  
  Serial.println(F("Inicializando o RTC..."));
  while(!rtc.begin()) { delay(50); }
  wdt_reset();
  if (! rtc.isrunning() ) {
    Serial.println(F("RTC não inicializado. Ajustando horário"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  DateTime now = rtc.now();
  minuto = int(now.minute()/intervalo)*intervalo;
  hora = now.hour();

  Serial.println( "Hora atual: "+String( now.hour())+":"+String( now.minute()));
  
  Serial.println(F("Inicializando LDRs..."));
  // Configurando os LDRS
  pinMode( PIN_L, INPUT_PULLUP );    
  pinMode( PIN_N, INPUT_PULLUP );
  pinMode( PIN_S, INPUT_PULLUP );
  pinMode( PIN_O, INPUT_PULLUP );

  Serial.println(F("Inicializando Servos..."));
  //configura os servos
  s1.setAnguloMax(azimuteMax);
  s1.setAnguloMin(azimuteMin);
  s1.setDutyMax( int( 200/18*azimuteMax+400) );
  s1.setDutyMin( int( 200/18*azimuteMin+400) );
  s1.setTarget( S1angulo );
  s2.setAnguloMax(alturaMax);
  s2.setAnguloMin(alturaMin);
  s2.setDutyMax( int( 200/18*alturaMax+400) );
  s2.setDutyMin( int( 200/18*alturaMin+400) );
  s2.setTarget( S2angulo );

  s1.start();
  s2.start();

  Serial.println(F("Inicializando o timer..."));
  //configura o timer interrupt para enviar o sinal de controle dos servos
  TCCR2A = 0xA3;                                        //pwm não invertido, fast pwm 
  TCCR2B = 0x04;                                        //pre scaler 1:64
  TIMSK2 = 0x01;                                        //liga interrupção do Timer2
  sei();                                                //habilita interrupção global

  Serial.println(F("Tudo pronto!"));
  Serial.println(F("Aperte C para entrar no modo de configuração"));
  Serial.println(F("Aperte D para descarregar dados da memória"));
  Serial.println();

  //minuto = minuto_inicio;
  //hora = hora_inicio;
  
}

void loop() {
  wdt_reset();
  
  // Faz 10 medições dos valores dos LDR's e calcula a média
  float L=0, N=0, S=0, O=0;
  for ( int i=1; i<10; i++ ) {
    L += analogRead( PIN_L );
    N += analogRead( PIN_N );
    S += analogRead( PIN_S );
    O += analogRead( PIN_O );
  }
  L = L/10+(L/10*offset_L);
  N = N/10+(N/10*offset_N);
  S = S/10+(S/10*offset_S);
  O = O/10+(O/10*offset_O);

  float k=0.1; // Coeficiente proporcional

  // Ajuste do angulo de azimute
  if( abs( L - O ) > tolerancia) {
    S2angulo -= int(k*( L - O ));
  }
  
  //Ajuste do angulo de altura
  if( abs( N - S ) > tolerancia) {
    S1angulo -= int(k*( N - S ));
  }
  
  if( S1angulo > azimuteMax ) { S1angulo = azimuteMax; }
  if( S1angulo < azimuteMin ) { S1angulo = azimuteMin; }
  if( S2angulo > alturaMax ) { S2angulo = alturaMax; }
  if( S2angulo < alturaMin ) { S2angulo = alturaMin; }

  // Envia os novos angulos para os Servos
  s1.setTarget( S1angulo );
  s2.setTarget( S2angulo );

  // Controle do tempo de coleta de dados
  float agora, m;
  DateTime now;
  if ( abs( millis()-tempo) > 5000 ) { 
    now = rtc.now();
    hora=now.hour();
    m = now.minute();
    agora = hora+( m/60);
    tempo = millis();
  }

  //Se estiver no intervalo de amostragem, faz leitura e salva na memória.
  if ( agora >= inicio && agora <= fim && ( int(m)%int(intervalo))==0 && m != minuto ) {
    minuto=m;
    //Lendo TSC34725
    uint16_t r=0, g=0, b=0, c=0;
    while ( true ) {
      if( resolucao == 1 ){
        float r=0, g=0, b=0;
        TCS.getRGB( &r, &g, &b);
      }  
      else { 
        TCS.getRawData( &r, &g, &b, &c ); 
      }     
      if( !((r+g+b+c)==0) ) { break; }
      Serial.println( "Erro na leitura do sensor");
      TCS.disable();
      delay( 1000 );
      TCS.enable();      
    }
    wdt_reset();
  
    uint8_t data[ lenReg ];
    data[0] = uint8_t( hora );
    data[1] = uint8_t( minuto );
    if( resolucao == 1 ){
      data[2] = uint8_t( r );
      data[3] = uint8_t( g );
      data[4] = uint8_t( b );
    } else {
      data[2] = uint8_t( r/255 );
      data[3] = uint8_t( r - (data[2]*255));
      data[4] = uint8_t( g/255 );
      data[5] = uint8_t( g - (data[4]*255));
      data[6] = uint8_t( b/255 );
      data[7] = uint8_t( b - (data[6]*255));
    }
    
    //Registra amostragem na EEprom
    reg.write( data );

    String msg = "";
    msg+=String(int(hora))+":"+String(int(minuto));
    msg+=" | R: "+String(r)+" | G: "+String(g)+" | B: "+String(b);
  
    Serial.println( msg );

  }
  delay( 30 ); 
  char input = Serial.read();
  if ( String(input)=="D"){ descarrega(); }
  if ( String(input)=="C"){ config(); }
  if ( String(input)=="R"){
    Serial.println( "R para sair..");
    s1.setTarget(90);
    s2.setTarget(90);
    while( Serial.read() != "R"){ delay(30);}
  }
 
}
