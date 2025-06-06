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
// stop() = Desliga o servo
// start() = Liga o servo e posiciona no último angulo definido como alvo
//
// Os valores definidos por padrão na classe atendem as especificações de micro servos do tipo Futaba de 180o modelos SG90 e MG995;
//

class srvctrl {

    int pin;             //Pino de controle do servo (tem que ser PWM)
    int target;          //Posição alvo do servo
    int pos;             //Posição atual
    int duty;            //Largura em ms do duty cycle relativo ao angulo atual do servo
    int pace = 1;        //Passo (angulo) de reposicionamento do servo.
    int pulse_min = 400  //largura em ms do duty cycle para o angulo 0 do servo
    int pulse_max = 2400 //largura em ms do duty cycle para o angulo 180 do servo
    int angulo_min = 0   //Angulo mínimo para o servo
    int angulo_max = 180 //Angulo máximo para o servo

    public:

    // construtor da classe
    srvctrl( int _pin ){
      pin = _pin;
      pinMode( pin, OUTPUT);
      digitalWrite( pin, LOW );
      duty = 0;
      pos = 0;
      target = -1;
    };

    // Define o angulo alvo do servo, ou velocidade e direcao de rotacao, se servo 360o
    void setTarget( int angulo ) {
      while( !stable() ){}
      if( angulo >=angulo_min and angulo <=angulo_max ) {
        target = angulo;        
      }
    };

    // Retorna o angulo alvo de posição do servo
    int getTarget() {
      return target;
    };

    // Retorna o valor corrente do duty Cycle do servo 
    int getDuty() {
      return duty;
    };

    // Envia pulso de controle para o servo
    void pulse() {
      if( duty > 0 ){
          digitalWrite( pin, HIGH);
            delayMicroseconds(duty);
          digitalWrite( pin,  LOW);
      }
    };

    // Define o duty para o reposicionamento do servo
    void step() {
      if ( duty > 0 ) {
        if( pos+pace <= dutyCur ) { pos += pace; }
        else if( pos-pace >= dutyCur ) { pos -= pace; }
        else { pos=dutyCur; }
        duty = map( pos, angulo_min, angulo_max, pulse_min, pulse_max);
      }
    }

    // verifica se o servo está na posição alvo
    bool stable() {
      return (pos == dutyCur); 
    }

    // define o passo de reposicionamento do servo (default = 1)
    void setPace( int passo = 1 ) {
      if (passo > 1 && passo <= angulo_max) { 
        pace = passo;
      }
    }

    // Ajusta o duty cycle para o angulo mínimo do servo
    void setDutyMin( int _pulso = 0 ) {
      pulse_min = _pulso;
    }

    // Ajusta o duty cycle para o angulo máximo do servo
    void setDutyMax( int _pulso = 2400 ) {
      pulse_max = _pulso;
    }

    //Define o angulo mínimo para o servo
    void setAnguloMin( int angulo = 0 ){ 
      angulo_min = angulo;
    }

    //Define o angulo mánimo para o servo
    void setAnguloMin( int angulo = 180 ){ 
      angulo_max = angulo;
    }

    // Para o servo
    void srvStop() {
      duty = 0;
      digitalWrite( pin, LOW );
    };               

    // Inicia o servo
    void srvStart() {
      duty = map( pos, angulo_min, angulo_max, pulse_min, pulse_max);
    };

};