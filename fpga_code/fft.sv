/*module fft(input logic sck, sdi, reset,
			  output logic sdo, done);
			  
	logic dataReady; //flag goes high when SPI buffer is full
	logic [1023:0] fft_output1024, fft_input1024;
	logic [31:0] fft_output32, fft_input32;
	logic clk; //FPGA on-board oscillator
	logic fft_processing;
	logic out_buf_empty, out_buf_ready;
	logic [31:0] idxIn, idxOut;
	logic [5:0] rd_adr;
	logic load, processing, start, fft_done; //FFT flags
	logic fft_reset, fft_reset_pulse;
	// Create spi module, input module, output module
	
	HSOSC #(.CLKHF_DIV ("0b10")) hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(clk));

	fft_spi spi(.sck(sck), .sdi(sdi), .reset(reset), .sdo(sdo), .dataReady(dataReady), .fft_out(fft_output1024), .fft_in(fft_input1024));

	// Route SPI buffer to FFT input
	fft_in_flop fftinReg(.clk(clk), .fft_in1024(fft_input1024), .fft_processing(fft_processing), 
	.reset(reset), .cs(!dataReady), .out_buf_empty(out_buf_empty), .out_buf_ready(out_buf_ready), .fft_in32(fft_input32), 
	.fft_load(load),.fft_start(start), .idx(idxIn));


	// Route FFT output to SPI buffer
	fft_out_flop fftoutReg(.clk(clk),.reset(reset), .fft_out32(fft_output32), .fft_done(fft_done) , .fft_load(load), 
	.fft_out1024(fft_output1024), .buf_ready(out_buf_ready), .buf_empty(out_buf_empty), .idx(idxOut));

	fft_controller fftUnit(.clk(clk), .reset(fft_reset_pulse), .start(start), .load(load), .load_address(rd_adr),
	.data_in(fft_input32), .done(fft_done), .processing(fft_processing), .data_out(fft_output32));
	
	// Have logic which waits until SPI is full before starting FFT inputting
	assign rd_adr = (load) ? idxIn : idxOut;
	
	// Have logic which, once FFT output is full, pulses reset in the FFT
	always_ff @(posedge(clk)) begin
		if (reset) fft_reset_pulse <= 1;
		else fft_reset_pulse <= fft_reset;
	end 

	//next reset pulse logic
	assign fft_reset = !fft_reset_pulse && out_buf_ready;



	assign done = out_buf_ready;

	

endmodule*/

//FOR USE IN TESTBENCH
module fft(input logic clk, sck, sdi, reset, full_reset,
			  output logic sdo, done);
			  
	logic dataReady; //flag goes high when SPI buffer is full
	logic [1023:0] fft_output1024, fft_input1024;
	logic [31:0] fft_output32, fft_input32;
	logic fft_processing;
	logic out_buf_empty, out_buf_ready;
	logic [5:0] idxIn, rd_adr;
	logic load, processing, start, fft_done; //FFT flags
	logic fft_reset, fft_reset_pulse;
	
	logic [1:0]    clk_counter = 0;

	always_ff @(posedge clk) begin
		clk_counter = clk_counter + 1'b1;
	end
	
	// clock to multiplex RAM input
	assign ram_clk = clk_counter[0];
	
	// clock to fix latency in ERAM
	assign slow_clk = clk_counter[1];
	
	// Create spi module, input module, output module

	fft_spi spi(.sck(sck), .sdi(sdi), .reset(reset), .done(done), .sdo(sdo), .dataReady(dataReady), .fft_out(fft_output1024), .fft_in(fft_input1024));

	// Route SPI buffer to FFT input
	fft_in_flop fftinReg(.clk(slow_clk), .fft_in1024(fft_input1024), .fft_processing(fft_processing), 
	.reset(reset), .dataReady(dataReady), .out_buf_empty(out_buf_empty), .out_buf_ready(out_buf_ready), .fft_in32(fft_input32), 
	.fft_load(load),.fft_start(start), .fft_done(fft_done), .idx(idxIn));


	// Route FFT output to SPI buffer
	fft_out_flop fftoutReg(.clk(slow_clk),.full_reset(full_reset), .fft_out32(fft_output32), .fft_done(fft_done) , .fft_load(load), 
	.fft_out1024(fft_output1024), .buf_ready(out_buf_ready), .buf_empty(out_buf_empty));

	fft_controller fftUnit(.clk(clk), .ram_clk(ram_clk), .slow_clk(slow_clk), .reset(reset), .start(start), .load(load), .load_address(rd_adr),
	.data_in(fft_input32), .done(fft_done), .processing(fft_processing), .data_out(fft_output32));
	
	// Have logic which waits until SPI is full before starting FFT inputting
	assign rd_adr = idxIn;



	assign done = out_buf_ready;

	

endmodule