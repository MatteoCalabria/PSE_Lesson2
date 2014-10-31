#include "fpm_RTL.hh"

void fpm_RTL :: elaborate_MULT_FSM(void){
  if (reset.read() == 0){
    STATUS = Reset_ST;
  }
  else if (clk.read() == 1) {

    STATUS = NEXT_STATUS;

    switch(STATUS){
      case Reset_ST:
        result_port.write(0);
        result_isready.write(0);
        break;
      case ST_0:
        result_port.write(0);
        result_isready.write(0);
	Prodotto.write(0);
        break;
      case ST_1:
        Number_one = number_port_one.read();	// leggo sulla porta d'ingresso il primo numero
	Number_two = number_port_two.read();	// leggo sulla porta d'ingresso il primo numero
        break;
      case ST_2:
	/*
        if (Counter.read() < 16){
          Rem.write((Rem.read() << 2) + (Number.read() >> 30));
          Number.write(Number.read() << 2);
          Root.write(Root.read() << 1);
        }*/
        break;
      case ST_3:

	Prodotto.write( number_port_one.read() );
	/*
        if (Root.read() < Rem.read()){
          Rem.write(Rem.read() - Root.read() - 1);
          Root.write(Root.read() + 2);
        }
        Counter.write(Counter.read() + 1);*/
        break;
      case ST_4:
	
        result_isready.write(1);
        result_port.write( number_port_one.read() );
        break;
      case ST_5:
        

      break;
      case ST_6:
        

      break;
      case ST_7:
        

      break;
      case ST_8:
        

      break;
      case ST_9:
        

      break;
    } 
  }
}


void fpm_RTL :: elaborate_MULT(void){


  NEXT_STATUS = STATUS;

  switch(STATUS){
    case Reset_ST:	
	NEXT_STATUS = ST_0;
      break;

    // stato di controllo, se il segnale number_isready è a 1 allora ho i due numeri
    // e posso avanzare allo stato ST_1
    case ST_0:
      if (number_isready.read() == 1){
        NEXT_STATUS = ST_1;
      } else {
        NEXT_STATUS = ST_0;
      }	
      break;

    case ST_1:
	NEXT_STATUS = ST_2;
      break;

    case ST_2:
	NEXT_STATUS = ST_3;
      break;

    case ST_3:
	NEXT_STATUS = ST_4;
      break;

    // controllo il segnale overflow, se è a 1, allora vado ST_5 che darà errore
    // altrimenti avanza allo stato ST_6	
    case ST_4:
	/*if (overflow.read() == 1){
	        NEXT_STATUS = ST_5;
	} else {
	        NEXT_STATUS = ST_6;
        }*/
      break;

    // se vado nello stato ST_5, manderò errore e il prossimo stato sarà lo stato di reset
    case ST_5:
      	NEXT_STATUS = Reset_ST;
      break;

    case ST_6:
      NEXT_STATUS = ST_7;
      break;

    // stato di controllo, controlla sul segnale not_normalized se è a 1
    // il numero non è normalizzato allora torno allo stato ST_3
    case ST_7:
	/*if (not_normalized.read() == 1){
	        NEXT_STATUS = ST_3;
	} else {
	        NEXT_STATUS = ST_8;
        }*/
      break;

    case ST_8:
      NEXT_STATUS = ST_9;
      break;
	
    // stato finale, torno a allo stato di reset
    case ST_9:
      NEXT_STATUS = Reset_ST;
      break;
  }
}
