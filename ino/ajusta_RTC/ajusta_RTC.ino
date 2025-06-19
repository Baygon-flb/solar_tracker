#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>  //Adafruit RTClib

#define SERIAL_RATE 115200

//Instanciando as classes
RTC_DS1307 rtc;

// retorna o ultimo dia do mes
int ultimoDiaMes( int m, int ano ) {
  int ultimoDia[12][2] = {{1,31},{2,28},{3,31},{4,30},{5,31},{6,30},{7,31},{8,31},{9,30},{10,31},{11,30},{12,31}};
  //verifica se o ano é bisexto 
  if ((ano%4)==0) { ultimoDia[2][1]=29; }
  return ultimoDia[m][1];
}

// Funcao para ajustar um parametro do horário
// incrementa ou decrementa o parametro passado por referência, 
// conforme comando recebido pela serial (+ ou -)
// Verifica se o parâmetro está dentro dos limites informados
// Retorna true se o parametro sofreu alguma alteracao
bool ajusta( int &par, String titulo, int min, int max ) {
  Serial.println( "Ajustando o "+titulo);
  Serial.println( "Pressione + ou - para ajustar, s para sair");
  Serial.println( par );
  int parant = par;
  bool mudou = false;
  while (true) { 
    // verifica se o parametro sofreu alteraçao
    if (parant != par) { 
      Serial.println( par );
      parant = par;
      // ajusta variavel de controle para sinalizar a mudanca
      mudou = true;
    }
    while(!Serial.available()){ delay(30); }
    char input = Serial.read();
    if( String(input)=="s" ) { break; }
    if( String(input)=="+" ) { par++; }
    if( String(input)=="-" ) { par--; }
    if (par>max) { par = min; }
    if (par<min) { par = max; }
  }
  return mudou;
}

void setup() {
  Serial.begin( SERIAL_RATE );
  Serial.println(F("AJUSTE DO RTC"));
  Serial.println(F("-------------"));
  
  Serial.println(F("Inicializando o RTC..."));
  while(!rtc.begin()) { delay(50); }
  if (! rtc.isrunning() ) {
    Serial.println(F("RTC não inicializado. Sincronizando com o horário do sistema"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println(F("Comande..."));
  Serial.println(F("d - Para ajustar o dia"));
  Serial.println(F("M - Para ajustar o mês"));
  Serial.println(F("a - Para ajustar o ano"));
  Serial.println(F("h - Para ajustar a hora"));
  Serial.println(F("m - Para ajustar o minuto"));
  Serial.println();
   
}

int s = 0;
long tempo = 0;

void loop() {

  //variaveis de horário para ajuste do RTC
  int dia, mes, ano, hora, minuto, segundo;

  if ( abs(millis()-tempo) > 10000 ) {
    DateTime now = rtc.now();
    segundo = now.second();
    dia = now.day();
    mes = now.month();
    ano = now.year();
    hora = now.hour();
    minuto = now.minute();
    Serial.println( "Horário atual: "+String(dia)+"-"+String(mes)+"-"+String(ano)+"  "+String(hora)+":"+String(minuto)+":"+String(segundo));
    tempo = millis();
  }
  
  delay( 30 ); 

  // Verifica se recebeu algum comando pela serial
  bool mudou = false;

  if ( Serial.available() ) { 
    char input = Serial.read(); 
    if ( String(input)=="d" ){ mudou=ajusta( dia,"dia", 1, ultimoDiaMes( mes, ano ) ); }
    if ( String(input)=="M" ){ mudou=ajusta( mes, "mês", 1, 12 ); }
    if ( String(input)=="a" ){ mudou=ajusta( ano, "ano", 1970, 2200 ); }
    if ( String(input)=="h" ){ mudou=ajusta( hora, "hora", 0, 23 ); }
    if ( String(input)=="m" ){ mudou=ajusta( minuto,"minuto", 0, 59 ); }
  }

  // Se houve alguma mudança, ajusta o horário do RTC
  if( mudou ) {
    rtc.adjust( DateTime(ano,mes,dia,hora,minuto,segundo) );
    tempo = 0;
  }
 
}
