`timescale 1ns / 1ps

module mul(
    input clk,
    input rst,
    input start,
    
    input [7:0] a_in,
    input [7:0] b_in,
    
    output reg busy,
    output reg [15:0] y
);
    reg [7:0] a, b;
    reg [2:0] step;
    
    localparam FinalStep = 3'h7;
    
    wire [7:0] part_sum;
    wire [15:0] shifted_part_sum;
    
    assign part_sum = a & {8{b[step]}};
    assign shifted_part_sum = part_sum << step;
    
    always@(posedge clk)
        if (rst)
            busy <= 0;
        else if (start && !busy) begin
            busy <= 1;
            step <= 4'h0;
            a <= a_in;
            b <= b_in;
            y <= 0;
        end
        else if (busy) begin
            y = y + shifted_part_sum;
            
            if (step == FinalStep)
                busy <= 0;
            else
                step <= step + 1;
        end
endmodule