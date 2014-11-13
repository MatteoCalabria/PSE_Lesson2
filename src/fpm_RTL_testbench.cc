#include "fpm_RTL_testbench.hh"
#include "support.hh"


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

  //sc_uint<64> temp_data_in_one, temp_data_in_two, result = 0;
  sc_lv<64> temp_data_in_one, temp_data_in_two, result = 0;

  cout<<"Calcoliamo la Floating Point Multiplication di 20 coppie di numeri"<<endl;

  for (int i = 1; i <= 20; i++){

	double first = ((rand() % 21) - 10) * 0.1;
	double second = ((rand() % 21) - 10) * 0.1;
	//double first = 1.5;
	//double second = 1.5;
	temp_data_in_one = doubleToLogicVector(first); 
    	temp_data_in_two = doubleToLogicVector(second);

 
	// in binario
	//cout<<"\nThe multiplication of "<<temp_data_in_one << endl;
	//cout<<"\nand "<<temp_data_in_two << endl;
	
	// in double
	cout<<"\nThe multiplication of "<< first;
	cout<<" and "<< second;

   /* cout<<"\nin double:";
    cout<<"\nThe multiplication of "<< logicVectorToDouble(temp_data_in_one) <<endl;
    cout<<"\nand "<< logicVectorToDouble(temp_data_in_two) <<endl;*/

    reset_to_RTL.write(1);
    p_Out_data_one.write(temp_data_in_one);
    p_Out_data_two.write(temp_data_in_two);

    p_Out_enable_testbench.write(1);
    wait();

    while(p_In_enable_testbench.read() != 1) wait();
    result=p_In_result.read();
    double stampa = logicVectorToDouble(result);
    cout << " is: " << stampa << endl;
   // cout << "\n is: " << result << endl;



  }

  reset_to_RTL.write(0);
  p_Out_enable_testbench.write(0);
  p_Out_data_one.write(0);
  p_Out_data_two.write(0);

  sc_stop();

}


