#ifndef fpm_RTL_TESTBENCH_H
#define fpm_RTL_TESTBENCH_H

#define PERIOD 10

#include <systemc.h>

class fpm_RTL_testbench : public sc_core::sc_module
{

private:

  void run();
  void clk_gen();

public:

  sc_out< bool >        reset_to_RTL;
  sc_out< sc_logic >    clk;
  sc_in<sc_uint<64> > 	p_In_data_one;
  sc_in<sc_uint<64> > 	p_In_data_two;
  sc_out<sc_uint<64> > 	p_Out_data_one;
  sc_out<sc_uint<64> > 	p_Out_data_two;
  sc_out<sc_uint<1> >  	p_Out_enable_testbench;
  sc_in<sc_uint<1> >  	p_In_enable_testbench;
  sc_in<sc_uint<64> > 	p_In_result;

  SC_HAS_PROCESS(fpm_RTL_testbench);
  fpm_RTL_testbench(sc_module_name name);

};

#endif
