`timescale 10ns/1ns
/////////////////////////////////////////////
// testbench_aes_spi
// Tests AES with cases from FIPS-197 appendix
// Simulates full system with SPI load
/////////////////////////////////////////////

module testbench_main();
    logic clk, done, sck, sdi, sdo, reset, full_reset, done_temp;
    logic [1023:0] expected, fftin1024, fftout1024;
    logic [31:0] input_data [63:0];

    // Added delay
    logic delay, reset_delay;
    logic [11:0] i;
    
    // device under test
    fft fft(.clk(clk), .sck(sck), .sdi(sdi), .reset(reset), .full_reset(full_reset), .sdo(sdo), .done(done));
    
    // test case
    initial begin   
      $readmemh("simulation/test_in_square.memh", input_data);
    end


    
    // generate clock and load signals
    initial 
        forever begin
            clk = 1'b0; #5;
            clk = 1'b1; #5;
        end
        
    initial begin
      i = 0;
      // set delay to true
      delay = 1;
      reset_delay = 1;
      sck = 0;
      done_temp = 0;
      //pulse reset
      reset = 1; full_reset = 1; #40;
      reset = 0; full_reset = 0;
    end
  
  genvar x;
  //generate hell
  generate
    for (x=0; x<64; x = x +1) begin
            assign fftin1024[((x+1)*16-1):(x*16)] = input_data[63-x][31:16];
      end
  endgenerate

    
    // shift in test vectors, wait until done, and shift out result
    always @(posedge clk) begin
      if (reset_delay) begin
        #40;
        reset_delay = 0;
      end else if (i<1024) begin
        #1; sdi = fftin1024[1023-i];
        #1; sck = 1; #5; sck = 0;
        i = i + 1;
      end else if (delay) begin
        #100; // Delay to make sure that the answer is held correctly on the cyphertext before shifting out
        delay = 0;
      end else if (done && !done_temp) begin
	#40; reset = 1; #40; reset = 0; #40; done_temp = 1;
      end else if (done_temp && i < 2048) begin
        #1; sck = 1; 
        #1; fftout1024[2047-i] = sdo;
        #4; sck = 0;
        i = i + 1;
      end else if (i == 2048) begin
            //if (cyphertext == expected)
                //$display("Testbench ran successfully");
            //else $display("Error: cyphertext = %h, expected %h",
                //cyphertext, expected);
            #40; reset = 1; #40; reset = 0; #40; i = 0; fftout1024 = 0; delay = 1; done_temp = 0;
      
      end
    end
    
endmodule