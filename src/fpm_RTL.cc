#include "fpm_RTL.hh"

int BIAS = 1023;


void fpm_RTL :: elaborate_MULT_FSM(void){

	sc_lv <64> Number_one;
  	sc_lv<52> Number_one_significand;
  	sc_lv<11> Number_one_exponent;
  	sc_lv<1> Number_one_sign;

	sc_lv <64> Number_two;
  	sc_lv<52> Number_two_significand;
  	sc_lv<11> Number_two_exponent;
  	sc_lv<1> Number_two_sign;
	
	static int exp1;
	static int i;	
	static int sign1;
	static int j;


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

      case ST_1: // leggiamo i due numeri e identifichiamo i loro segni, esponenti e mantisse
	// primo numero

        Number_one = number_port_one.read();
	Number_one_sign[0] = Number_one[63];	

	exp1 = 10;
	for(i = 62 ; i>=52; i--)
		Number_one_exponent[exp1--] = Number_one[i];

	sign1 = 51;
	for(j = 51; j>=0; j--)
		Number_one_significand[sign1--] = Number_one[j];

	cout <<"\nsegno" << Number_one_sign ;
	cout <<"\nesponente" << Number_one_exponent ;
	cout <<"\nmantissa" << Number_one_significand ;

	// secondo numero	

	/*Number_two = number_port_two.read();
	Number_two_sign[0] = Number_two[63];		
	for(int exp2 = 62; exp2>=52; exp2--)
		Number_two_exponent[exp2] = Number_two[exp2];
	for(int sign2 = 51; sign2>=0; sign2--)
		Number_two_significand[sign2] = Number_two[sign2];*/




	/*Number_one_sign[0] = Number_one[0];
	int exp1 = 1;	
	for(exp1; exp1<=12; exp1++)
		Number_one_exponent[exp1] = Number_one[exp1];
	int sign1 = 13;	
	for(sign1; sign1<=63; sign1++)
		Number_one_significand[sign1] = Number_one[sign1];*/



	/*Number_two_sign[0] = Number_two[0];
	int exp2 = 1;	
	for(exp2; exp2<=11; exp2++)
		Number_two_exponent[exp2] = Number_two[exp2];
	int sign2 = 12;	
	for(sign2; sign2<=63; sign2++)
		Number_two_significand[sign2] = Number_two[sign2];*/
        break;

      case ST_2: 
	// somma degli esponenti
	/*int i = 1;
	int carry = 0;
	for(i; i<=12; i++){
		if(Number_one_exponent[i] + Number_two_exponent[i] + carry == 0){
			Prodotto_exponent[i] = 0;
			carry = 0;
		}

		if(Number_one_exponent[i] + Number_two_exponent[i] + carry == 1){
			Prodotto_exponent[i] = 1;
			carry = 0;
		}

		if(Number_one_exponent[i] + Number_two_exponent[i] + carry == 2){
			Prodotto_exponent[i] = 0;
			carry = 1;
		}

		if(Number_one_exponent[i] + Number_two_exponent[i] + carry > 2){
			Prodotto_exponent[i] = 1;
			carry = 1;
		}
	}

	sc_signal< sc_int<64> >  temp_exp = logicVectorToDouble(Prodotto_exponent) - BIAS;
	Prodotto_exponent = doubleToLogicVector(temp_exp);*/

        break;

      case ST_3: // moltiplichiamo le mantisse



      case ST_4: // controllo normalizzazione mantissa
	


        break;
      case ST_5:
        

      break;
      case ST_6:
        

      break;
      case ST_7:
        

      break;
      case ST_8:
	// Prodotto.write( Number_one.read() * Number_two.read() );
        

      break;
      case ST_9:
        result_isready.write(1);
	// così funziona, ma non uso Prodotto
        // result_port.write( number_port_one.read() + number_port_two.read() );
	
	// in uno di questi due modi funziona solo al primo colpo, non cambia il valore
	// result_port.write( Number_one.read() );
	result_port.write( Prodotto.read() );        

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
      if (numbers_areready.read() == 1){
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
	NEXT_STATUS = ST_5;
      break;

    // se vado nello stato ST_5, manderò errore e il prossimo stato sarà lo stato di reset
    case ST_5:
      	NEXT_STATUS = ST_6;
      break;

    case ST_6:
      NEXT_STATUS = ST_7;
      break;

    // stato di controllo, controlla sul segnale not_normalized se è a 1
    // il numero non è normalizzato allora torno allo stato ST_3
    case ST_7:
	NEXT_STATUS = ST_8;
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
