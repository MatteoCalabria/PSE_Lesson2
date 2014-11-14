#include "fpm_RTL.hh"
#include <cmath>



static int BIAS = 1023;


void fpm_RTL :: elaborate_MULT_FSM(void){

	static sc_lv<64> Number_one;
  	static sc_lv<52> Number_one_significand;
  	static sc_lv<11> Number_one_exponent;
  	static sc_lv<1> Number_one_sign;

	static sc_lv<64> Number_two;
  	static sc_lv<52> Number_two_significand;
  	static sc_lv<11> Number_two_exponent;
  	static sc_lv<1> Number_two_sign;

	static sc_lv<64> Prodotto;
  	static sc_lv<64> Prodotto_significand;
  	static sc_lv<64> Prodotto_exponent;
  	static sc_lv<1> Prodotto_sign;


	static sc_lv<107> mantissa;
	static sc_lv<106> temp_num_uno;
	static sc_lv<106> temp_num_due;
	static sc_lv<53> temp;
	static sc_lv<128> zeri;
	
	static int i;
	static int j;
	static sc_lv<1> carry;
	static double mant1;
	static double mant2;
	static double prod_mant;
	static int exp_intero;
	static int temp_exp;
	static int parte_intera_mantissa;
	static double parte_decimale_mantissa;
	static double temp_mantissa;
	static uint64_t tmp;



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
	Counter.write(0);
	Counter2.write(0);
	carry[0] = 0;
	zeri = "0";
        break;

      case ST_1: // leggiamo i due numeri e identifichiamo i loro segni, esponenti e mantisse

	// primo numero

        Number_one = number_port_one.read();
	Number_one_sign[0] = Number_one[63];	
	Number_one_exponent = Number_one.range(62, 52);
	Number_one_significand = Number_one.range(51, 0);

	// secondo numero	

	Number_two = number_port_two.read();
	Number_two_sign[0] = Number_two[63];
	Number_two_exponent = Number_two.range(62, 52);
	Number_two_significand = Number_two.range(51, 0);

        break;

      case ST_2: 
	// somma degli esponenti
	i = Counter.read();

	
	if(i<12){

		if(i==11)
			Prodotto_exponent[i] = carry[0];

		// risultato = 0
		else if( (Number_one_exponent[i]==0) && (Number_two_exponent[i]==0) && (carry[0]==0) ){
			Prodotto_exponent[i] = 0;
			carry[0] = 0;
		}

		// risultato = 1
		else if( ((Number_one_exponent[i]==1) && (Number_two_exponent[i]==0) && (carry[0] == 0)) ||
		((Number_one_exponent[i]==0) && (Number_two_exponent[i]==1) && (carry[0] == 0)) ||
		((Number_one_exponent[i]==0) && (Number_two_exponent[i]==0) && (carry[0] == 1)) ) {
			Prodotto_exponent[i] = 1;
			carry[0] = 0;
		}

		// risultato = 2
		else if( ((Number_one_exponent[i]==1) && (Number_two_exponent[i]==1) && (carry[0] == 0)) ||
		((Number_one_exponent[i]==1) && (Number_two_exponent[i]==0) && (carry[0] == 1)) ||
		((Number_one_exponent[i]==0) && (Number_two_exponent[i]==1) && (carry[0] == 1)) ) {
			Prodotto_exponent[i] = 0;
			carry[0] = 1;
		}

		// risultato > 2
		else if( (Number_one_exponent[i]==1) && (Number_two_exponent[i]==1) && (carry[0] == 1) ){
			Prodotto_exponent[i] = 1;
			carry[0] = 1;
		}	
	}

        break;


	case ST_2_1:

		Counter.write(Counter.read() + 1);

	break;

      case ST_3: // moltiplicazione delle mantisse

	exp_intero = (int) static_cast< sc_uint<12> >(Prodotto_exponent);
	exp_intero = exp_intero - BIAS;
	Prodotto_exponent = static_cast< sc_lv<12> >(exp_intero);


	mantissa = "0";
	
	temp_num_uno = "0";
	temp_num_uno = Number_one_significand.range(51, 0);
	temp_num_uno[52] = 1;

	temp_num_due = Number_two_significand.range(51,0);
	temp_num_due[52] = 1;

	carry = 0;

	break;

      case ST_4: // controlli e normalizzazione

	i = Counter2.read();

	if(i<53){
		j = i;
		if(temp_num_due[i]==1)
		for(j; j<=i+53; j++){
	
			// risultato = 0
			if( (mantissa[j]==0) && (temp_num_uno[j]==0) && (carry[0]==0) ){
				mantissa[j] = 0;
				carry[0] = 0;
			}

			// risultato = 1
			else if( ((mantissa[j]==1) && (temp_num_uno[j]==0) && (carry[0]==0)) ||
			((mantissa[j]==0) && (temp_num_uno[j]==1) && (carry[0]==0)) ||
			((mantissa[j]==0) && (temp_num_uno[j]==0) && (carry[0]==1)) ) {
				mantissa[j] = 1;
				carry[0] = 0;
			}

			// risultato = 2
			else if( ((mantissa[j]==1) && (temp_num_uno[j]==1) && (carry[0]==0)) ||
			((mantissa[j]==0) && (temp_num_uno[j]==1) && (carry[0]==1)) ||
			((mantissa[j]==1) && (temp_num_uno[j]==0) && (carry[0]==1)) ) {
				mantissa[j] = 0;
				carry[0] = 1;
			}

			// risultato > 2
			else if( (mantissa[j]==1) && (temp_num_uno[j]==1) && (carry[0]==1) ){
				mantissa[j] = 1;
				carry[0] = 1;
			}

		}	
		
		// shift
		temp_num_uno = temp_num_uno << 1; 
	}

	

      break;

      case ST_4_1:
	
		Counter2.write(Counter2.read() + 1);

      break;

      case ST_5:

	mantissa[106] = carry[0];

	if(mantissa[105]==1){
		mantissa = mantissa >> 1;
		Prodotto_exponent = static_cast< sc_lv<11> >(exp_intero+1);
	}
        

      break;

      case ST_6:
        

      break;

      case ST_7:
        

      break;

      case ST_8:

	Prodotto = "0";

	// segno
	if(Number_one_sign[0] == Number_two_sign[0])
		Prodotto[63] = 0;
	else
		Prodotto[63] = 1; 

	// esponente
	Prodotto_exponent = Prodotto_exponent << 52;
	Prodotto.range(62,52) = Prodotto_exponent.range(62, 52);

	// mantissa
	Prodotto_significand.range(51,0) = mantissa.range(103,52);
	Prodotto.range(51,0) = Prodotto_significand.range(51,0);

	//cout<<"\nCONTROLLO FINALE:";
	//cout<<"\nsegno:     " << Prodotto[63];
	//cout<<"\nesponente: " << Prodotto_exponent;
	//cout<<"\nmantissa : " << Prodotto_significand;
	//cout<<"\nrisultato: " << Prodotto;

      break;
      case ST_9:
        result_isready.write(1);
	result_port.write( Prodotto );        

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
	if(Counter.read()<12)
		NEXT_STATUS = ST_2_1;
	else
		NEXT_STATUS = ST_3;
      break;

   case ST_2_1:
	NEXT_STATUS = ST_2;
	break;

    case ST_3:
	NEXT_STATUS = ST_4;
      break;

   
    case ST_4:
	if(Counter2.read()<53)
		NEXT_STATUS = ST_4_1;
	else
		NEXT_STATUS = ST_5;
      break;

    case ST_4_1:
	NEXT_STATUS = ST_4;
      break;

    case ST_5:
      	NEXT_STATUS = ST_6;
      break;

    case ST_6:
      NEXT_STATUS = ST_7;
      break;

  
    case ST_7:
	NEXT_STATUS = ST_8;
      break;

    case ST_8:
      NEXT_STATUS = ST_9;
      break;
	
  
    case ST_9:
      NEXT_STATUS = Reset_ST;
      break;
  }
}
