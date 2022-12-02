// Testbench taken from https://github.com/AlecVercruysse/fft_tutorial
// and modified for a 64-point fft that uses ICE40 BRAM (1 cycle of latency per iteration)
module fft_testbench_spi();
   
   logic clk, ram_clk, slow_clk;
   logic start, load, done, reset;
   logic signed [15:0] expected_re, expected_im, wd_re, wd_im;
   logic [31:0]        rd, wd;
   logic [31:0]        idx, out_idx, expected;

   logic [5:0]            rd_adr, idxIN, idxOUT;
   //assign rd_adr = idx[5:0];
   
   logic [31:0]          input_data [0:63];
   logic [31:0]        expected_out [0:63];

   logic fft_out_flop_done;
   logic [1023:0] fftout1024;
   logic [1023:0] fftin1024;
   logic bufempty;
   logic cs; 
   logic processing;

   integer             f; // file pointer

   fft_controller dut(clk, reset, start, load, rd_adr, rd, done,processing, wd);
   fft_out_flop fftout(.idx(idxOUT),.clk(slow_clk), .reset(reset), .fft_done(done), .buf_ready(fft_out_flop_done), .fft_out1024(fftout1024), .fft_load(load), .fft_out32(wd), .buf_empty(bufempty));
   fft_in_flop fftin(.idx(idxIN),.clk(slow_clk), .reset(reset), .fft_in1024(fftin1024), .fft_processing(processing), .cs(cs), .out_buf_ready(fft_out_flop_done), .fft_in32(rd), .fft_load(load), .fft_start(start), .out_buf_empty(bufempty) );
   // clk
   assign rd_adr = (load) ? idxIN : idxOUT;
   always
     begin
	    clk = 1; #5; clk=0; #5;
     end

   always
     begin
	    ram_clk = 1; #10; ram_clk=0; #10;
     end

   always
     begin
	    slow_clk = 1; #20; slow_clk=0; #20;
     end
   
   // start of test: load `input_data`, `expected_out`, open output file, reset fft module.
   initial
     begin
	$readmemh("simulation/test_in_square.memh", input_data);

	$readmemh("simulation/gt_test_out_square.memh", expected_out);
        f = $fopen("simulation/test_out_square.memh", "w"); // write computed values.
	idx=0; reset=1; #40; reset=0; cs = 0;
     end	

genvar i;
//generate hell
generate
	for (i=0; i<64; i = i +1) begin
      		assign fftin1024[((i+1)*16-1):(i*16)] = input_data[63-i][31:16];
   	end
endgenerate
/*
   // increment testbench counter and derive load/start signals
   always @(posedge slow_clk)
     if (~reset) idx <= idx + 1;
     else idx <= idx;
   assign load =  idx < 64;
   assign start = idx === 64;

   // increment output address if done, reset if restarting FFT
   always @(posedge slow_clk)
     if (load) out_idx <= 0;
     else if (done) out_idx <= out_idx + 1;
*/
  /* 
   // load/start logic
   assign rd = load ? input_data[idx[5:0]] : 0;  // read in test data by addressing `input_data` with `idx`.
   assign expected = expected_out[out_idx[5:0]]; // get test output by addressing `expected_out` with `idx`.
   assign expected_re = expected[31:16];   // get real      part of `expected` (gt output)
   assign expected_im = expected[15:0];         // get imaginary part of `expected` (gt output)
   assign wd_re = wd[31:16];               // get real      part of `wd` (computed output)
   assign wd_im = wd[15:0];                     // get imaginary part of `wd` (computed output)

*/
/*
   // if FFT is done, compare gt to computed output, and write computed output to file.
   always @(posedge slow_clk)
     if (done) begin
	if (out_idx <= (63)) begin
           $fwrite(f, "%h\n", wd);
	   if (wd !== expected) begin
	      $display("Error @ out_idx %d: expected %b (got %b)    expected: %d+j%d, got %d+j%d", 
                       out_idx, expected, wd, expected_re, expected_im, wd_re, wd_im);
	   end
	end else begin
	   $display("FFT test complete.");
           $fclose(f);
           //$stop;
	end
     end
*/
endmodule // fft_testbench
