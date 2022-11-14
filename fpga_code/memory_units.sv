// based on Edwards from Columbia and Harris from E85
// 6-bit addresses for 64-point FFT
module ram (input logic          clk, write,
            input logic [5:0]    address_a, address_b,
            input logic [31:0]   d_a, d_b,
            output logic [31:0]  q_a, q_b);

   reg [31:0] mem [63:0];

   always_ff @(posedge clk)
     if (write) begin
          mem[address_a] <= d_a;
          mem[address_b] <= d_b;
       end

   assign q_a = mem[address_a];
   assign q_b = mem[address_b];

endmodule


module twiddle_rom ();

   // maybe do like sbox? maybe do like above, but write from file instead.

endmodule
