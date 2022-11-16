// based on Edwards from Columbia and Harris from E85
// 6-bit addresses for 64-point FFT
// will introduce 1 cycle of lag
module ram (input logic          clk, write,
            input logic [5:0]    address_a, address_b,
            input logic [31:0]   d_a, d_b,
            output logic [31:0]  q_a, q_b);

   logic [31:0] mem [63:0];

   always_ff @(posedge clk)
     if (write) begin
          mem[address_a] <= d_a;
          mem[address_b] <= d_b;
       end

   assign q_a = mem[address_a];
   assign q_b = mem[address_b];

endmodule

// taken from sbox, synchronous - extra cycle of latency
module twiddle_rom (input logic clk,
                    input logic [4:0] twiddle_address,
                    output logic [31:0] twiddle);
            
   // twiddle factors are generated with msb on left side
   logic [31:0] mem [0:31];

   initial   $readmemh("./rom/twiddle.vectors", mem);
	
	// Synchronous version
	always_ff @(posedge clk) begin
		twiddle <= mem[twiddle_address];
	end

endmodule
