// Classe de controle de servo motor
// Esta classe controla o reposicionamento um servo motor, para que o movimento aconteça de forma gradual (smooth).
//
// Inicializacao: svctrl servoObj( pino_saida ) 
// Metodos:   
// srvctrl( int pin ) = Instancia um objeto de controle do servo ligado no pino informado
// setTarget( int angulo ) = Move o servo para o angulo duty ( 0 - 180 ) ou a velocidade e sentido de rotação, se o servo for 360o
// getTarget() = Retorna o duty atual do servo 
// getDuty() = Retorna a largura de pulso do alvo atual
// pulse() = Envia pulso de controle para o servo
// step() = Reposiciona o servo conforme avançando/retrocendendo o angulo definido por pace
// stable() = Retorna verdadeiro se o servo estiver posicionado no angulo definido como alvo
// setPace( int passo ) = Define o pace para o valor informado em passo
// setDutyMin( int pulso ) = Define a largura de pulso (em ms) para o angulo mínimo do servo
// setDutyMax( int pulso ) = Define a largura de pulso (em ms) para o angulo máximo do servo
// setAnguloMin = Define o angulo mínimo do servo
// setAnguloMax = Define o angulo máximo do servo
// getAnguloMin = Define o angulo mínimo do servo
// getAnguloMax = Define o angulo máximo do servo
// setSync( long tempo ) = Define o tempo de sincronia do servo (intervalo de tempo para envio do pulso de controle)
// getSync() = Retorna o tempo de sincronia do servo
// stop() = Desliga o servo
// start() = Liga o servo e posiciona no último angulo definido como alvo
//
// Os valores definidos por padrão na classe atendem as especificações de micro servos do tipo Futaba de 180o modelos SG90 e MG995;
//
// Para evitar o flicker dos servos, faremos o controle do tempo de pulso dos servos através do interrupt do timer 2 do arduino
// 
// A base de tempo:
//
// O Timer 2 será ajustado para um pre scaler de 1:64 (fast pwm). Por padrão, o pre scaler é de 1024.
// A resolução do timer 2 é de 256 (8 bits).
// O ciclo de máquina do arduino é se 62.5ns (1/16Mhz)
// A base de tempo do timer interrupt será, portanto, de aproximados 1ms: tempo = Resoluçao x Pre scaler x tempo de Ciclo de máquina = 256 * 64 * 62.5ns = 1024us 
//
// O controle do servo motor:
//
// O período típico de pulso de controle do servo é aproximadamente 20ms, que é igual a aproximados 20 cliclos de timer interrupt
// Este período não é critico. É aceitável uma variação razoável do período, mas vamos assumir uma variação máxima de 20% ou 4ms
// O Duty cycle do pulso determina o angulo de posicão do servo (modelo de ref.), variando entre 600us e 2400us, correspondendo respectivamente à 0 e 180 graus
// Assim, a cada aproximados 20ms é necessário enviar um pulso para o servo, com a duração (duty cycle) correspondente ao angulo de posição desejado.
//
// Para fazer isso implementei o metodo pulse com um contador que é chamado pela rotina de timer interrupt. como ajustamos o overflow do timer para 
// cada aproximados 1ms, temos que contar 20 chamadas do método para então gerar um pulso. Assim, incrementamos o contador a cada chamada e, quando
// ele chega em 20, produzimos o pulso de controle do serve e zeraramos o contador, reiniciando o ciclo.
//  

class Srvctrl {

    int pin;             //Pino de controle do servo (tem que ser PWM)
    int target;          //Posição alvo do servo
    int pos;             //Posição atual
    int duty;            //Largura em ms do duty cycle relativo ao angulo atual do servo
    int pace = 1;        //Passo (angulo) de reposicionamento do servo.
    int pulse_min = 400; //largura em us do duty cycle para o angulo 0 do servo
    int pulse_max = 2400;//largura em us do duty cycle para o angulo 180 do servo
    int angulo_min = 0;  //Angulo mínimo para o servo
    int angulo_max = 180;//Angulo máximo para o servo
    long syncT = 20;     //tempo de sincronia do servo em ms
    int cSync =0;        //Contador de sincronia

    public:

    // construtor da classe
    Srvctrl( int _pin ){
      pin = _pin;
      pinMode( pin, OUTPUT);
      digitalWrite( pin, LOW );
      duty = 0;
      pos = 0;
      target = -1;
    };

    // CONTROLES

    // Envia pulso de controle para o servo
    void pulse() {
      if( duty > 0 ){
        cSync++;
        if( cSync >= syncT ) {
          digitalWrite( pin, HIGH);
          delayMicroseconds(duty);
          digitalWrite( pin,  LOW);
          cSync=0;
          step();
        }
      }
    };

    // Define o duty para o reposicionamento do servo
    void step() {
      if ( duty > 0 ) {
        if( pos+pace <= target ) { pos += pace; }
        else if( pos-pace >= target ) { pos -= pace; }
        else { pos=target; }
        duty = map( pos, angulo_min, angulo_max, pulse_min, pulse_max);
      }
    };

    // verifica se o servo está na posição alvo
    bool stable() { return (pos == target); };

  // Para o servo
    void stop() {
      duty = 0;
      digitalWrite( pin, LOW );
    };               

    // Inicia o servo
    void start() { duty = map( pos, angulo_min, angulo_max, pulse_min, pulse_max); };

    //SETS

    // Define o angulo alvo do servo, ou velocidade e direcao de rotacao, se servo 360o
    void setTarget( int angulo ) {
      //while( !stable() ){ delay(1);}
      if( angulo >=angulo_min and angulo <=angulo_max ) {
        target = angulo;        
      }
    };

    // define o passo de reposicionamento do servo (default = 1)
    void setPace( int passo = 1 ) {
      if (passo > 1 && passo <= angulo_max) { pace = passo; }
    };

    // Ajusta o duty cycle para o angulo mínimo do servo (em us)
    void setDutyMin( int _pulso = 0 ) { pulse_min = _pulso; };

    // Ajusta o duty cycle para o angulo máximo do servo (em us)
    void setDutyMax( int _pulso = 2400 ) { pulse_max = _pulso; };

    //Define o angulo mínimo para o servo
    void setAnguloMin( int angulo = 0 ){ angulo_min = angulo; };

    //Define o angulo mánimo para o servo
    void setAnguloMax( int angulo = 180 ){ angulo_max = angulo; };

    //GETS

    //Retorna o tempo de sincronia do servo (em ms)
    long getSync() {return syncT;};

    // Ajusta o tempo de sincronia do servo (em ms)
    void setSync( long T = 20) {syncT = T;};

    // Retorna o angulo alvo de posição do servo
    int getTarget() {return int(target);};

    // Retorna o valor corrente do duty Cycle do servo 
    int getDuty() {return duty;};

    // Retorna o passo de reposicionamento do servo
    int getPace() {return pace;};

    // Retorna o duty cycle para o angulo mínimo do servo (em us)
    int getDutyMin( ) {return pulse_min;};

    // Retorna o duty cycle para o angulo máximo do servo (em us)
    int getDutyMax() {return pulse_max;};

    //Retorna o angulo mínimo para o servo
    int getAnguloMin(){ return angulo_min;};

    //Define o angulo mánimo para o servo
    int getAnguloMax(){ return angulo_max;};
  
};