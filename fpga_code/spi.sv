module fft_spi(input  logic sck, //from mcu
               input  logic sdi, //from mcu
               input logic reset,
					input logic done,
               output logic sdo,//to mcu
               output logic [1023:0] fft_in, //fft input
               output logic dataReady, //to fft input flop
               input  logic [1023:0] fft_out); //fft output

	 logic [10:0]   cnt;
	 logic qdelayed;
		 
	 always_ff @(negedge sck, posedge reset) begin
		  if (reset) cnt <= 0;
		  else 		 cnt <= cnt + 11'b1;
	 end
		 
    always_ff @(posedge sck) begin
		  fft_in <= (cnt == 0) ? {fft_out[1022:0], sdi} : {fft_in[1022:0], sdi};
	 end
	 


    // sdo should change on the negative edge of sck
    always_ff @(negedge sck)
			qdelayed = fft_in[1023];
    
    // when cs is first asserted, shift out msb before clock edge
    assign sdo = (cnt === 0) ? fft_out[1023]: qdelayed;
    assign dataReady = (cnt === 1024);
endmodule

//every pulse of slow_clk, we will have a new 32 bit output from FFT
module fft_out_flop(
    input logic clk, //from FPGA
    input logic [31:0] fft_out32, //from FFT
	 input logic fft_start, // to reset cnt
    input logic fft_done, //from FFT
    input logic fft_load, //from FFT
    input logic full_reset,
    output logic [1023:0] fft_out1024, //to SPI
    output logic buf_ready,
    output logic buf_empty
	 );

logic [6:0] cnt = 0;
logic [1023:0] q;
logic [1023:0] d;
logic [1023:0] d_shift;



always_ff @(negedge clk) begin
    /*if (full_reset || fft_load || !fft_done) cnt <= 0; //NOTE: done signal sketch here
    else if (cnt == 32) cnt <= cnt;
	 else cnt <= cnt + 1;*/
	 if (full_reset || fft_start) cnt <= 0;
	 else if (fft_done) cnt <= ((cnt == 32) ? cnt : cnt + 7'b1);
	 else cnt <= cnt;
end

always_ff @(negedge clk) begin
    if (full_reset) q <= 0;
    else q <= d;
end

always_comb begin
	d_shift = (cnt == 32) ? q : q << 32;
	d = (cnt == 32) ? q : {d_shift[1023:32], fft_out32}; //NOTE: we don't need the (cnt == 0) here if we pulse reset before doing everything
end


assign fft_out1024 = q;
assign buf_ready = (cnt == 32);
assign buf_empty = (cnt == 0);

endmodule


//every pulse of slow_clk, we will send a new 32 bit input to the FFT
module fft_in_flop(
    input logic clk, //from FPGA
    input logic [1023:0] fft_in1024, //from SPI
    input logic fft_processing,
    input logic reset,
    input logic dataReady,
	 input logic fft_done,
    input logic out_buf_empty, //from fft_out_flop
    input logic out_buf_ready, //from fft_out_flop
    output logic [31:0] fft_in32, //to FFT
    output logic fft_load, //to FFT
    output logic fft_start, //to FFT
    output logic [5:0] idx //to FFT
);

typedef enum logic {WAIT,SEND} state;

logic [6:0] cnt;
logic [1023:0] q;
logic [1023:0] d;
logic [1023:0] d_shift;
state currState;
state nextState;
logic [15:0] re16; //16 bit real component we are grabbing from q

assign re16 = q[1023:1008]; //16 bit real is 16 most significant bits of data stored in flop.

assign d_shift = (cnt == 63) ? q : q << 16;
assign d = d_shift;
assign sendReady = !fft_processing && dataReady && !fft_done; //we are ready to SEND data to FFT unit when the FFT is not in processing AND not sending to output buffer

// counter flip flop
always_ff @(posedge clk) begin
    if (currState == WAIT) cnt <= 0; //if in WAIT state, don't increment counter
    else cnt <= cnt + 7'b1; //only increment counter when we are in SEND state
end

// data flip flop
always_ff @(posedge clk) begin
    if (currState == WAIT) q <= fft_in1024;
    else q <= d;
end

// next state flip flop
always_ff @(posedge clk) begin
    if (reset) currState <= WAIT;
    else currState <= nextState;
end

//next state logic
always_comb begin
    case (currState)
        WAIT: nextState = (sendReady && cnt != 64) ? SEND : WAIT;
        SEND: nextState = (cnt === 63) ? WAIT : SEND;
        default: nextState = WAIT;
    endcase
end


assign fft_start = (cnt == 64);
assign fft_load = (currState == SEND && !fft_processing);
assign idx = cnt[5:0];
Extend32 extend(.a(re16), .b(fft_in32)); //extend 16 bit real into 32 bit complex


endmodule

//pads 16 0's to a 16 bit input signal, creating a 32 bit signal
module Extend32(
    input logic [15:0] a,
    output logic [31:0] b
);

assign b = {a, {16'b0}};

endmodule


