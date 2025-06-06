// Solar_tracker
// Seguidor solar
// Este projeto implementa um seguidor solar com 2 eixos, que rastreia o posicionamento do sol 
// (ou outra fonte de luz) de forma a manter um sensor apontado penperdicularmente à fonte de luz.
// Neste caso específico, o sensor utilizado é para medir a variação do espectro, intensidade e 
// temperatura da luz emitida pela fonte, utilizando um módulo sensor RGB TCS34725. 
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
#include "tracker.h"

#define PIN_S1 = 5
#define PIN_S2 = 6
#define PIN_UP = A0
#define PIN_DOWN = A1
#define PIN_RIGH = A2
#define PIN_LEFT = A3

#define COLETA 1
#define AJUSTE 0
#define SERIAL_RATE 9600;

//Instanciando as classes
Adafruit_TCS34725 TCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_300MS, TCS34725_GAIN_1X);
RTC_DS1307 rtc;
Tracker tracker( PIN_UP, PIN_DOWN, PIN_RIGHT, PIN_LEFT );


// variáveis de controle
int modo = COLETA;
bool debug = false;
String hora_inicio = "5:30";
String hora_final = "18:30";

function debug( String msg ) {
  if( Serial.begin( SERIAL_RATE )) && debug {
    Serial.println( msg );
  }
} 

void setup() {
  debug(F("SOLAR TRACKER \n ______________________"));
  debug(F("Inicializando o RTC..."));
  while(!rtc.begin()) { delay(50); }
  if (! rtc.isrunning() ) {
    Serial.println(F("RTC não inicializado. Ajustando horário"));
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
}

void loop() {
  
  //Lendo TSC34725
  uint16_t r, g, b, c, TempCor, LUX;

  //O sensor lê os valores do Vermelho (R), Verde(G), Azul(B)
  sensTCS.getRawData(&r, &g, &b, &c);
  
  //Calcula a temperatura da cor 
  TempCor = sensTCS.calculateColorTemperature(r, g, b);
  
  //Calcula a luminosidade 
  LUX = sensTCS.calculateLux(r, g, b);


}
