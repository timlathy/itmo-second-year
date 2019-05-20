`timescale 1ns / 1ps

module sqrt(
    input clk,
    input rst,
    input start,
    
    input [15:0] x_in,
    
    output reg busy,
    output reg [15:0] out
);
    reg [15:0] x, m;
    wire [15:0] b;
    
    assign b = out | m;
    
    always@(posedge clk)
        if (rst)
            busy <= 0;
        else if (start && !busy) begin
            busy <= 1;
            out <= 0;
            m <= 15'h4000;
            x <= x_in;
        end
        else if (busy) begin
            if (m == 0)
                busy <= 0;
            else begin
                if (x >= b) begin
                    x = x - b;
                    out = (out >> 1) | m;
                end
                else begin
                    out = out >> 1;
                end
                m = m >> 2;
            end
        end
            
endmodule
