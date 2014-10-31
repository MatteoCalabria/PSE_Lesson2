#include "fpm_RTL_testbench.hh"

  fpm_RTL_testbench::fpm_RTL_testbench(sc_module_name name_)
: sc_module(name_)
{

  SC_THREAD(run);
  sensitive << clk.pos();

  SC_THREAD(clk_gen);
}

void fpm_RTL_testbench::clk_gen()
{
  while( true )
  {
    clk.write( sc_dt::SC_LOGIC_1 );
    wait(PERIOD / 2, sc_core::SC_NS);
    clk.write( sc_dt::SC_LOGIC_0 );
    wait(PERIOD / 2, sc_core::SC_NS);
  }
}

void fpm_RTL_testbench::run()
{

  sc_uint<64> temp_data_in_one, temp_data_in_two, result = 0;

  cout<<"Calcoliamo la Floating Point Multiplication di 100 coppie di numeri"<<endl;

  for (int i = 1; i <= 20; i++){
    temp_data_in_one = (rand() % 256);  	//creo le coppie di numeri per la simulazione del sistema
    temp_data_in_two = (rand() % 256);

    cout<<"\tThe multiplication of "<<temp_data_in_one;
    cout<<" and "<<temp_data_in_two;

    reset_to_RTL.write(1);
    p_Out_data_one.write(temp_data_in_one);
    p_Out_data_two.write(temp_data_in_two);

    p_Out_enable.write(1);
    wait();

    while(p_In_enable.read() != 1) wait();
    result=p_In_data.read();
    cout << "\t is: " << result << endl;

  }

  reset_to_RTL.write(0);
  p_Out_enable.write(0);
  p_Out_data_one.write(0);

  sc_stop();

}


