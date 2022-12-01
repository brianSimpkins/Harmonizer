module fft_spi(input  logic sck,//from mcu
               input  logic sdi,//from mcu
               output logic sdo,//to mcu
               input  logic cs, //from mcu
               output logic [1023:0] q, //fft input
               input  logic [1023:0] d); //fft output

    logic         qdelayed, wasdone;
               
    always_ff @(posedge sck)
        if (cs) q <= {q[1022:0], sdi};
        else    q <= {d[1022:0],sdi};

    // sdo should change on the negative edge of sck
    always_ff @(negedge sck) begin
        qdelayed = q[1023];
    end
    
    // when cs is first asserted, shift out msb before clock edge
    assign sdo = (cs) ? qdelayed : d[1023];
endmodule

//every pulse of slow_clk, we will have a new 32 bit output from FFT
module fft_out_flop(
    input logic clk, //from FPGA
    input logic [31:0] fft_out32, //from FFT
    input logic fft_done, //from FFT
    input logic fft_load, //from FFT
    input logic reset,
    output logic [1023:0] fft_out1024, 
    output logic buf_ready
);

logic [6:0] cnt;
logic [1023:0] q;
logic [1023:0] d;
logic [1023:0] d_shift;

assign d_shift = (cnt == 63) ? q : q << 32;
assign d = (cnt==0) ? 0 : {d_shift[1023:32], fft_out32}; //NOTE: we don't need the (cnt == 0) here if we pulse reset before doing everything

always_ff @(negedge clk) begin
    if (reset || fft_load || !fft_done || cnt == 64) cnt <= 0; //NOTE: done signal sketch here
    else cnt <= cnt + 1;
end

always_ff @(negedge clk) begin
    if (reset) q <= 0;
    else q <= d;
end


assign fft_out1024 = (cnt == 0) ? 0 : q;
assign buf_ready = (cnt == 64);

endmodule