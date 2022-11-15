// Testbench taken from https://github.com/AlecVercruysse/fft_tutorial
// and modified for a 64-point fft that uses ICE40 BRAM (1 cycle of latency per iteration)
module fft_testbench();
   
   logic clk, slow_clk;
   logic start, load, done, reset;
   logic signed [15:0] expected_re, expected_im, wd_re, wd_im;
   logic [31:0]        rd, wd;
   logic [31:0]        idx, out_idx, expected;

   logic [5:0]            rd_adr;
   assign rd_adr = idx[5:0];
   
   logic [31:0]          input_data [0:63];
   logic [31:0]        expected_out [0:63];

   integer             f; // file pointer

   fft_controller dut(clk, reset, start, load, rd_adr, rd, done, wd);
   
   // clk
   always
     begin
	    clk = 1; #5; clk=0; #5;
     end

   always
     begin
	    slow_clk = 1; #10; slow_clk=0; #10;
     end
   
   // start of test: load `input_data`, `expected_out`, open output file, reset fft module.
   initial
     begin
	$readmemh("rom/test_in.memh", input_data);
	$readmemh("rom/gt_test_out.memh", expected_out);
        f = $fopen("rom/test_out.memh", "w"); // write computed values.
	idx=0; reset=1; #40; reset=0;
     end	

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
   
   // load/start logic
   assign rd = load ? input_data[idx[5:0]] : 0;  // read in test data by addressing `input_data` with `idx`.
   assign expected = expected_out[out_idx[5:0]]; // get test output by addressing `expected_out` with `idx`.
   assign expected_re = expected[31:16];   // get real      part of `expected` (gt output)
   assign expected_im = expected[15:0];         // get imaginary part of `expected` (gt output)
   assign wd_re = wd[31:16];               // get real      part of `wd` (computed output)
   assign wd_im = wd[15:0];                     // get imaginary part of `wd` (computed output)

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
           $stop;
	end
     end
endmodule // fft_testbench
