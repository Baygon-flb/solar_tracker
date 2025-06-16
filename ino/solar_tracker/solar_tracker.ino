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
Regdata reg;

// variáveis de controle
float minuto = 0;
float hora = 0;
long tempo = 0;
int erro = 2;
float tolerancia = (2.55*erro);
float intervalo = 1;

//horarios de inicio e fim
float hora_inicio = 5;
float minuto_inicio=30;
float hora_fim = 18;
float minuto_fim=30;
float inicio = hora_inicio+(minuto_inicio/60);
float fim = hora_fim+(minuto_fim/60);

//controle dos servos
int azimuteMin = 15;
int azimuteMax = 170;
int alturaMin = 10;
int alturaMax = 170;
int S1angulo = int((azimuteMax-azimuteMin)/2);
int S2angulo = int((alturaMax-alturaMin)/2);

//Calibragem LDR's
int offset_N = 8;
int offset_S = -8;
int offset_L = -10;
int offset_O = 10;

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
    int data[5];
    reg.read( p, data );
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
  Serial.println("var minuto: "+String( minuto ));
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
  
  //Verfica se houve um reset devido travamento
  if (MCUSR&8) {
    //recupera a última posição salva do ponteiro de memória
    //antes do travamento.
    reg.recover();    
  }

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
  L = L/10+offset_L;
  N = N/10+offset_N;
  S = S/10+offset_S;
  O = O/10+offset_O;
 
  float _NE = N + L; //calcula luminosidade ao nordeste
  float _SE = S + L; //calcula luminosidade ao suldeste
  float _NO = N + O; //calcula luminosidade ao noroeste
  float _SO = S + O; //calcula luminosidade ao suldoeste

  // Ajuste do angulo de azimute
  /*
  if ( abs( _SE - _SO ) > tolerancia || abs( _NE - _NO ) > tolerancia ) {
    if (_SE > _SO && _NE > _NO ) { S2angulo++; }
    if (_SE < _SO && _NE < _NO ) { S2angulo--; }
  }
  */ 
  if( abs( L - O ) > tolerancia) {
    S2angulo -= ( L - O )/2;
    //if ( L > O )  { S2angulo+=(l-); } else { S2angulo--; }
  }
  

  //Ajuste do angulo de altura
  /*
  if ( abs( _SE - _NE) > tolerancia || abs( _SO - _NO) > tolerancia ) {
    if (_SE > _NE && _SO > _NO ) { S1angulo++; }
    if (_SE < _NE && _SO < _NO ) { S1angulo--; }
  }
  */
  if( abs( N - S ) > tolerancia) {
    S1angulo -= ( N- S )/2;
    //if ( N < S )  { S1angulo++; } else { S1angulo--; }
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
    agora = now.hour()+( now.minute()/60);
    m = now.minute();
    tempo = millis();
  }

  //Se estiver no intervalo de amostragem, faz leitura e salva na memória.
  if ( agora >= inicio && agora <= fim && m == minuto ) {
  
  // if (abs( millis()-tempo) > 2000 ) {
    int hora=now.hour();
    //Lendo TSC34725
    float r, g, b, c;
    TCS.getRGB( &r, &g, &b); //Coleta os dados já mormalizados para 0 a 255.
    while ( r+g+b == 0 ) {
      Serial.println( "Erro na leitura do sensor");
      TCS.disable();
      delay( 1000 );
      TCS.enable();
      TCS.getRGB( &r, &g, &b);
      wdt_reset();
    }
    //Registra amostragem na EEprom
    reg.write( int(hora), int(minuto), r, g, b );

    String msg = "";
    msg+=String(int(hora))+":"+String(int(minuto));
    msg+=" | R: "+String(r)+" | G: "+String(g)+" | B: "+String(b);
  
    Serial.println( msg );

    // Ajusta o próximo intervalo de amostragem
    minuto+=intervalo;
    if ( minuto >= 60 ) { minuto = 0; }
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
