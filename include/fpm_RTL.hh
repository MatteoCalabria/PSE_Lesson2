#ifndef fpm_RTL_H
#define fpm_RTL_H

#include <systemc.h>

SC_MODULE(fpm_RTL){

  sc_in<bool>           reset;
  sc_in< sc_dt::sc_logic > clk;
  sc_in<sc_uint<1> >    numbers_areready;
  sc_in<sc_uint<64> >   number_port_one;
  sc_in<sc_uint<64> >   number_port_two;
  sc_out<sc_uint<64> >  result_port;
  sc_out<sc_uint<1> >   result_isready;

  typedef enum {Reset_ST, ST_0, ST_1, ST_2, ST_2_1, ST_3, ST_4, ST_5, ST_6, ST_7, ST_8, ST_9} STATES;

  sc_signal<STATES> STATUS, NEXT_STATUS;
  sc_signal< sc_uint<8> > Counter;
  /*sc_signal<sc_uint<64> > Number_one;
  sc_signal<sc_uint<64> > Number_one_significand;
  sc_signal<sc_uint<64> > Number_one_exponent;
  sc_signal<sc_uint<1> > Number_one_sign;*/

  /*sc_signal<sc_uint<64> > Number_two;
  sc_signal<sc_uint<64> > Number_two_significand;
  sc_signal<sc_uint<64> > Number_two_exponent;
  sc_signal<sc_uint<1> > Number_two_sign;*/

  //sc_signal<sc_uint<64> > Prodotto;
  /*sc_signal<sc_uint<64> > Prodotto_significand;
  sc_signal<sc_uint<64> > Prodotto_exponent;
  sc_signal<sc_uint<1> > Prodotto_sign;*/

  void elaborate_MULT(void);
  void elaborate_MULT_FSM(void);

  SC_CTOR(fpm_RTL){
    SC_METHOD(elaborate_MULT_FSM);
    sensitive << reset.neg();
    sensitive << clk.pos();

    SC_METHOD(elaborate_MULT);
    sensitive << STATUS << numbers_areready << number_port_one << number_port_two << Counter;
  };
};

#endif
