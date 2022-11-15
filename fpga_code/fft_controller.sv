// Top level module
module fft_controller (input logic 				clk, reset, start, load,
                       input logic [5:0] 		load_address,
                       input logic [31:0] 	data_in,
                       output logic 			done,
                       output logic [31:0] 	data_out);

   logic				slow_clk = 0;
   logic				processing, write_0, write_1;
   logic [5:0]		fft_level, fly_iter, address_0_a, address_0_b, address_1_a, address_1_b, address_out;
   logic [4:0]		twiddle_address;
   logic [31:0]	twiddle, a, b, a_out, b_out, write_data_a, write_data_b;
   logic [31:0]	read_data_0_a, read_data_0_b, read_data_1_a, read_data_1_b;

	// clock to fix 1-cycle latency in BRAM
	always_ff @(posedge clk) begin
		slow_clk = ~slow_clk;
   end

	// start 'processing' with a pulse from 'start'
	always_ff @(posedge clk) begin
		if      (start) processing <= 1;
		else if (reset || done)  processing <= 0;
   end

	fft_counter counter(slow_clk, processing, reset, 
							  fft_level, butterfly_iter);

	// output logic
   assign data_out = a;
	assign done = fft_level == 6;

	// output counter for address
	always_ff @(posedge slow_clk) begin
     if      (reset) out_address <= 0;
     else if (done)  out_address <= out_address + 1'b1;
	end

	agu address_generator(load, processing, done, fft_level, butterfly_iter, load_address, out_address,
								 address_0_a, address_1_a, address_0_b, address_1_b, twiddle_address);

	// if load is high, write data_in to ram 0
   assign write_data_a = load ? data_in : a_out;
   assign write_data_b = load ? data_in : b_out;

	ram ram0(clk, write_0, address_0_a, address_0_b, write_data_a, write_data_b,
				read_data_0_a, read_data_0_b);

	ram ram1(clk, write_1, address_1_a, address_1_b, write_data_a, write_data_b,
				read_data_1_a, read_data_1_b);

   // read from correct ram for butterfly input
   assign a = fft_level[0] ? read_data_1_a : read_data_0_a;
   assign b = fft_level[0] ? read_data_1_b : read_data_0_b;

	// get our twiddle factors
	twiddle_rom twiddle_gen(clk, twiddle_address,
									twiddle);

	// perform the operation
	butterfly_unit butt(a, b, twiddle,
							  a_out, b_out);

	assign write_0 =  (fftLevel[0] & processing) | load;
   assign write_1 =  ~fftLevel[0] & processing;


endmodule

// Counts the level of the fft (log(N)) and the butterfly index (N/2)
module fft_counter (	input logic clk, processing, reset,
							output logic [5:0] fft_level, butterfly_iter);

endmodule