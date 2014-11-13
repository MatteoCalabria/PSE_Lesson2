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
  	static sc_lv<52> Prodotto_significand;
  	static sc_lv<12> Prodotto_exponent;
  	static sc_lv<1> Prodotto_sign;


	static sc_lv<107> mantissa;
	static sc_lv<106> temp_num_uno;
	static sc_lv<106> temp_num_due;
	static sc_lv<53> temp;
	
	static int exp1;
	static int sign1;
	static int exp2;
	static int sign2;
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
        break;

      case ST_1: // leggiamo i due numeri e identifichiamo i loro segni, esponenti e mantisse

	// primo numero

        Number_one = number_port_one.read();
	Number_one_sign[0] = Number_one[63];	

	/*exp1 = 10;
	for(i = 62 ; i>=52; i--)
		Number_one_exponent[exp1--] = Number_one[i];*/

	Number_one_exponent = Number_one.range(62, 52);

	/*sign1 = 51;
	for(j = 51; j>=0; j--)
		Number_one_significand[sign1--] = Number_one[j];*/
	
	Number_one_significand = Number_one.range(51, 0);

	//cout <<"\nsegno" << Number_one_sign ;
	//cout <<"\nesponente" << Number_one_exponent ;
	//cout <<"\nmantissa" << Number_one_significand ;

	// secondo numero	

	Number_two = number_port_two.read();
	Number_two_sign[0] = Number_two[63];	

	//cout<<"\nsegno 1:" << Number_one_sign;

	/*exp2 = 10;
	for(i = 62 ; i>=52; i--)
		Number_two_exponent[exp2--] = Number_two[i];*/

	Number_two_exponent = Number_two.range(62, 52);

	/*sign2 = 51;
	for(j = 51; j>=0; j--)
		Number_two_significand[sign2--] = Number_two[j];*/

	Number_two_significand = Number_two.range(51, 0);

	//cout <<"\nsegno" << Number_one_sign ;
	//cout <<"\nesponente" << Number_one_exponent ;
	//cout <<"\nmantissa" << Number_one_significand ;

        break;

      case ST_2: 
	// somma degli esponenti
	i = 0;
	carry[0] = 0;
	for(i; i<12; i++){

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

	/*exp_intero = 0;
	i = 0;
	for(i; i<11; i++)
		if(Prodotto_exponent[i]==1)
			exp_intero += pow(2,i);*/

	
	//cout<<"\nexp intero prima di togliere il bias: " << exp_intero;
	//cout<<"\nexp logic vector prima di togliere il bias: " << Prodotto_exponent;
	

	exp_intero = (int) static_cast< sc_uint<12> >(Prodotto_exponent);
	

	//cout<<"\nexp intero prima di togliere il bias: " << exp_intero;
	//cout<<"\nexp logic vector prima di togliere il bias: " << Prodotto_exponent;
	
	exp_intero = exp_intero - BIAS;

	Prodotto_exponent = static_cast< sc_lv<12> >(exp_intero);

	//cout <<"\nesponente1: " << Number_one_exponent ;
	//cout <<"\nesponente2: " << Number_two_exponent ;
	//cout << "\nexp intero dopo aver tolto bias: " << exp_intero;			
	//cout << "\nexp logic vector dopo aver tolto bias: " << Prodotto_exponent;
	

	//cout<<"\nesponente uno: " << Number_one_exponent;
	//cout<<"\nesponente due: " << Number_two_exponent;
	//cout << "\nsomma esponenti: " << Prodotto_exponent;
	//cout <<"\nesponente in intero" << exp_intero;

        break;

      case ST_3: // moltiplicazione delle mantisse

	/*i = 0;
	for(i; i<107; i++)
		mantissa[i] = 0;*/
	mantissa = "0";

	//cout << "\nmantissa inizializzata a zero:\n" << mantissa;
	
	i = 0;
	for(i; i<53; i++)
		temp_num_uno[i] = Number_one_significand[i];
	temp_num_uno[52] = 1;
 

	i = 53;
	for(i; i<106; i++)
		temp_num_uno[i] = 0;

	i = 0;
	for(i; i<52; i++)
		temp_num_due[i] = Number_two_significand[i];
	temp_num_due[52] = 1;
	
	
	//cout<<"\ntemp uno inizializzato su 106 bit\n" << temp_num_uno;

	i = 0;
	carry = 0;
	for(i; i<53; i++){
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

	mantissa[106] = carry[0];

	/*cout << "\nCONTROLLO";
	cout <<"\nmantissa1: " << Number_one_significand << endl;
	cout <<"\nmantissa2: " << Number_two_significand << endl;
	cout <<"\nprodottoo: " << mantissa << endl;*/
	
	
		 

      case ST_4: // controlli e normalizzazione

	// togliere 1 in valore dai primi 3 bit convertiti in intero
	// normalizzare, cioè portar nella forma di 1,...
	// ,... va riportato bit a bit nell'esponente

	/* es:
	1,5 x 1,5 = 2,25
	2,25 = 11,01 
	--> tolgo 1 
	--> 10,01 
	--> porta in forma 1,001 aumentando di 1 l'exp 
	--> l'1 va omesso, ho quindi 0,001 
	--> 1*2(-3) = 0,125  
	--> 1+0,125 = 1,125
	--> 1,125 * 2^(1) = 2,25
	*/

	


	//cout <<"\nprodotto mantissa: " << mantissa << endl;
	

	// devo togliere 1 alla parte intera della mantissa ora !!!




	
	/*emp_exp = 0;
	i = 103;
	for(i; i>=52; i--)
		if(mantissa[i]==1){
			mantissa = mantissa >> 1;
			temp_exp++;
		}*/

	//mantissa = mantissa >> 52;

	if(mantissa[105]==1){
		mantissa = mantissa >> 1;
		Prodotto_exponent = static_cast< sc_lv<11> >(exp_intero+1);
	}


	//cout <<"\nprodotto mantissa:\n" << mantissa << endl;
	//cout <<"\nabbiamo fatto tot shift: " << temp_exp;

	
      break;

      case ST_5:
        

      break;

      case ST_6:
        

      break;

      case ST_7:
        

      break;

      case ST_8:

	i = 0;
	for(i; i<64; i++)
		Prodotto[i] = 0;

	// segno
	if(Number_one_sign[0] == Number_two_sign[0])
		Prodotto[63] = 0;
	else
		Prodotto[63] = 1; 
	
	
	i = 62;
	j = 10;
	for(i; i>=51; i--)
		Prodotto[i] = Prodotto_exponent[j--];


	i = 0;
	for(i; i<52; i++)
		Prodotto_significand[i] = mantissa[i+52];
	
	i = 51;
	j = 51;
	for(i; i>=0; i--)
		Prodotto[i] = Prodotto_significand[j--];

	//cout<<"\nCONTROLLO FINALE:";
	//cout<<"\nsegno: " << Prodotto[63];
	//cout<<"\nesponente: " << Prodotto_exponent;
	//cout<<"\nmantissa: " << Prodotto_significand;
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
