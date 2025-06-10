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
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <BH1750.h>
#include "Adafruit_TCS34725.h"
#include <RTClib.h>  //Adafruit RTClib
#include <TimeLib.h>
#include "srvctrl.h"

#define PIN_S1 5
#define PIN_S2 6
#define PIN_L A0 //LDR1 leste 
#define PIN_N A1 //LDR2 Norte
#define PIN_S A2 //LDR3 Sul
#define PIN_O A3 //LDR4 oeste

#define COLETA 1
#define AJUSTE 0
#define SERIAL_RATE 9600
#define DEBUG 0

//Instanciando as classes
Adafruit_TCS34725 TCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_300MS, TCS34725_GAIN_1X);
RTC_DS1307 rtc;
Srvctrl s1( PIN_S1 );
Srvctrl s2( PIN_S2 );

// variáveis de controle
int modo = COLETA;
float minuto = 0;
long tempo = 0;

//horarios de inicio e fim
float hora_inicio = 5;
float minuto_inicio=30;
float hora_fim = 18;
float minuto_fim=30;
float inicio = hora_inicio+(minuto_inicio/60);
float fim = hora_fim+(minuto_fim/60);

//controle dos servos
int azimuteMin = 10;
int azimuteMax = 170;
int alturaMin = 15;
int alturaMAx = 170;
int S1angulo = 90;
int S2angulo = 90;

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


void setup() {
  Serial.begin( SERIAL_RATE );
  debug(F("SOLAR TRACKER \n ______________________"));
  debug(F("Inicializando o RTC..."));
  while(!rtc.begin()) { delay(50); }
  if (! rtc.isrunning() ) {
    debug(F("RTC não inicializado. Ajustando horário"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  debug(F("Ajustando o horário do sistema..."));
  setSyncProvider(rtc.now()); 
  long tempo = millis();
  while (timeStatus()!= timeSet && abs(millis()-tempo) < 5000 ) { delay 100 };
  if (timeStatus()!= timeSet) {
     debug(F("Unable to sync with the RTC"));
  }
  else {
     debug(F("RTC has set the system time")); 
  }

  DateTime now = rtc.now();
  minuto = int(now.minute()/10)*10;

  // Configurando os LDRS
  pinMode( PIN_L, INPUT_PULLUP );    
  pinMode( PIN_N, INPUT_PULLUP );
  pinMode( PIN_S, INPUT_PULLUP );
  pinMode( PIN_O, INPUT_PULLUP );

  //configura os servos
  s1.setAnguloMax(azimuteMax);
  s1.setAnguloMin(azimuteMin);
  s1.setTarget( S1angulo );
  s2.setAnguloMax(alturaMax);
  s2.setAnguloMin(alturaMin);
  s2.setTarget( S2angulo );

  //configura o timer interrupt para enviar o sinal de controle dos servos
  TCCR2A = 0xA3;                                        //pwm não invertido, fast pwm 
  TCCR2B = 0x04;                                        //pre scaler 1:64
  TIMSK2 = 0x01;                                        //liga interrupção do Timer2
  sei();                                                //habilita interrupção global

  
}

void loop() {
  
  // Faz 10 medições dos valores dos LDR's e calcula a média
  float L=0, N=0, S=0, O=0;
  for (i=1, i<10, i++ ) {
    L += analogRead( PIN_L );
    N += analogRead( PIN_N );
    S += analogRead( PIN_S );
    O += analogRead( PIN_O );
  }
  L = L/10;
  N = N/10;
  S = S/10;
  O = O/10;
 
  int S2AnguloAnterior = S1angulo; //Guarda azimute anterior
  int S2AnguloAnterior = S2angulo; //Guarta altura anterior

  float _NE = N + L; //calcula luminosidade ao nordeste
  float _SE = S + L; //calcula luminosidade ao suldeste
  float _NO = N + O; //calcula luminosidade ao noroeste
  float _SO = S + O; //calcula luminosidade ao suldoeste

  // Ajuste do angulo de altura
  if ( abs( _SE - _SO) > tolerancia || abs( _SE - _SO) > tolerancia ) {
    if (_SE > _SO && _NE > _NO ) { S2angulo--; }
    if (_SE < _SO && _NE < _NO ) { S2angulo++; }
  }

  //Ajuste do angulo de azimute
  if ( abs( _SE - _NE) > tolerancia || abs( _SO - _NO) > tolerancia ) {
    if (_SE > _NE && _SO > _NO ) { S1angulo--; }
    if (_SE < _NE && _SO < _NO ) { S1angulo++; }
  }

  if( S1angulo > azimuteMax ) { S1angulo = azimuteMax; }
  if( S1angulo < azimuteMin ) { S1angulo = azimuteMin; }
  if( S2angulo > alturaMax ) { S2angulo = alturaMax; }
  if( S2angulo < alturaMin ) { S2angulo = alturaMin; }

  // if ( !( S1angulo == S1AnguloAnterior && S1angulo == S1AnguloAnterior  )) { mudou = true; }

  s1.setTarget( S1angulo );
  s2.setTarget( S2angulo );

  DateTime now = rtc.now();
  float agora = now.hour()+( now.minute()/60);
  float m = now.minuto();

  //Se estiver no intervalo de amostragem e o minuto for multiplo de 10, faz leitura e salva na memória.
  //if ( agora >= inicio && agora <= fim && m == minuto ) {
  //  ( minuto == 50 ) && minuto = 0 || minuto+=10;
  if (abs( millis()-tempo) > 2000
    
    //Lendo TSC34725
    uint16_t r, g, b, c;
    //O sensor lê os valores do Vermelho (R), Verde(G), Azul(B)
    TCS.getRawData(&r, &g, &b, &c);

    String msg = "";
    msg+=now.hour()+":"+now.minute();
    msg+=" | R: "+r+" | G: "+g+" | B: "+b

    debug( msg );
    tempo = millis();

  }
  else { delay( 18 ); };
 
}
