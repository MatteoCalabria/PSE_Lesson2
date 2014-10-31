
#include "fpm_RTL_testbench.hh"
#include "fpm_RTL.hh"

int sc_main(int argc, char **argv)
{
  sc_signal< sc_logic >         clock;
  
  sc_signal < bool >        reset_signal;
  sc_signal<sc_uint<64> >   p_In_data;
  sc_signal<sc_uint<64> >   p_In_data_one;
  sc_signal<sc_uint<64> >   p_In_data_two;
  sc_signal<sc_uint<1> >    p_In_enable;
  sc_signal<sc_uint<1> >    p_Out_enable; 
  sc_signal<sc_uint<64> >   p_Out_data;
  sc_signal<sc_uint<64> >   p_Out_data_one;
  sc_signal<sc_uint<64> >   p_Out_data_two;
  
  //creo i due oggetti che simulano i moduliche devono comunicare tra loro
  fpm_RTL_testbench        i_src_RTL("src_RTL");             // source module
  fpm_RTL                  i_fpm_RTL("fpm_RTL");    // root RTL module

  
  i_fpm_RTL.clk(clock);
  i_fpm_RTL.reset(reset_signal);
  i_fpm_RTL.number_isready(p_In_enable);
  i_fpm_RTL.number_port_one(p_In_data_one);
  i_fpm_RTL.number_port_two(p_In_data_two);
  i_fpm_RTL.result_port(p_Out_data);
  i_fpm_RTL.result_isready(p_Out_enable);
  
  i_src_RTL.clk(clock);
  i_src_RTL.p_In_enable(p_Out_enable);
  i_src_RTL.p_In_data_one(p_Out_data_one);
  i_src_RTL.p_In_data_two(p_Out_data_two);
  i_src_RTL.p_Out_enable(p_In_enable);
  i_src_RTL.p_Out_data_one(p_In_data_one);
  i_src_RTL.p_Out_data_two(p_In_data_two);
  i_src_RTL.reset_to_RTL( reset_signal );
  i_src_RTL.p_In_data(p_Out_data);
  
  sc_start();
  
  return 0;
};

