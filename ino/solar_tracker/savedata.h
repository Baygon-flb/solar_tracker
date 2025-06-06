// Registra configuracao de temperatura na ROM
class reg_conf_temp {

    private:

    float readMem( uint8_t pos ) {
        float parte_inteira = 0;
        float parte_decimal = 0;
        parte_inteira = EEPROM.read( pos );
        parte_decimal = EEPROM.read( pos+1 );
        return parte_inteira+(parte_decimal/100);
    };

    void writeMem( uint8_t pos, float val ) {
      uint8_t parte_inteira = (uint8_t) val;
      uint8_t parte_decimal = uint8_t ( val - (long) val ) * 100;
      EEPROM.write( pos ,parte_inteira);
      EEPROM.write( pos+1 ,parte_decimal);
    };
    
    public:
    reg_conf_temp;

    // Carrega parâmetros gravados na Eprom
    void getConf()
    {

      //float default_min = 25.0;
      //float default_max = 28.0;
      float default_target = 26.5;

      // Temperatura minima
      //temp_min = readMem( 0 );
      //if ((temp_min < limite_min) or (temp_min > limite_max)) { temp_min = default_min; };
      // Temperatura maxima
      //temp_max = readMem( 2 );
      //if ((temp_max < limite_min) or (temp_max > limite_max)) { temp_max = default_max; };
      // Temperatura alvo
      targetTemp = readMem( 4 );
      if ((targetTemp < limite_min ) or (targetTemp > limite_max)) { targetTemp = default_target; };
      // Precisao
      deltaTemp = readMem( 6 );
      if ((deltaTemp < .1 ) or (deltaTemp > 1)) { deltaTemp = 0.5; };
      // Calibracao
      calibracao = readMem( 8 );
      if ((calibracao < -5.0 ) or (calibracao > 5.0)) { calibracao = 0.0; };
      
    };

    // Salva parâmetros na Eprom
    void setConf()
    { 
      // Temperatura minima
      //writeMem( 0, temp_min );
      //Temperatura máxima      
      //writeMem( 2, temp_max );
      //Temperatura alvo      
      writeMem( 4, targetTemp );
      //Precisao     
      writeMem( 6, deltaTemp );
      //Calibracao     
      writeMem( 8, calibracao );
       
    };

};
