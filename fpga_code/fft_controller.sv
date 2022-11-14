module fft_controller (input logic 				clk, start, reset, load,
                       input logic [5:0] 		read_address,
                       input logic [31:0] 	data_in,
                       output logic 			done,
                       output logic [31:0] 	data_out);
    
   

   logic				read_select, write_0, write_1;
   logic [5:0]		address_a_0, address_b_0, address_a_1, address_b_1;
   logic [4:0]		twiddleadr; // twiddle ROM adr
   logic [31:0]	twiddle, butterfly_a, butterfly_b, aout, bout, write_data_a_0, write_data_b_0;
   logic [31:0]	read_data_a_0, read_data_b_0, read_data_a_1, read_data_b_1;