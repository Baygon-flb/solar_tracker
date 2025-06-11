# solar_tracker
## Seguidor solar

Este projeto implementa um experimento de um seguidor de luz, que localiza uma fonte de luz e
mantém um sensor RGB apontado perpendicularmente para esta fonte, utilizando um arduíno Nano
como controlador. 

O objetivo é fazer a medição das intensidades dos componentes expectrais cromáticos RGB da luz 
solar ao longo de um dia, a partir do que é possível também calcular a temperatura de cor e a
intensidade total. 

Para essa medição é utilizado um módulo TCS34725 RGB Light Sensor.

O projeto conta com dois servos motores, um que controla a coordenada de altura e outro a 
coordenada de azimute do sensor, além de 4 LDR's, dispostos nas extremidades da placa de 
suporte do sensor, posicionados nos quadrantes presumidos como os pontos cardeais N,S,L e O. 

Com base na diferença de leitura dos LDR's, os motores são acionados, reposicionando a placa de
suporte e o sensor, objetivando reorientar o conjunto até estabilizar os valores lidos dos LDR's 
dentro de uma margem de erro estipulada no programa, garantindo assim que o sensor estará, o 
máximo possível, apontado de forma perpendicular à fonte de luz. 

É realizada uma leitura dos dados do sensor a cada 10 minutos. Essas leituras são armazenadas na 
memória EEprom do arduíno e depois baixadas, via conexão serial, para um terminal, afim de que 
possam ser então capturados e tratados. Para controlar o tempo de coleta dos dados do sensor é 
utilizado um módulo RTC (DS1307).

Os servos são controlados sem uso de bilbioteca, por uma rotina de timer interrupt, afim de 
garantir que não ocorrerão flickers ou jammers que comprometam a estabilidade da posição do sensor.

O detalhamento completo da elaboração e construção do projeto pode ser encontrado na pasta de 
documentação (pasta DOC).

Os componentes utilizados no projeto são:

* Arduino nano V3
* Módulo sensor TCS34725
* Módulo RTC DS1307
* Módulo StepDown DC/DC LM2596

As pastas do projeto são:
* CAD - Desenhos DWG dos componentes de montagem
* KICAD - Esquemático eletrônico e PCB
* INO - Arquivos de Sketch para a IDE do arduino
* STL - Arquivos para impressão das partes dos componentes de montagem
* DOC - Documentação do projeto

@baygon-flb
